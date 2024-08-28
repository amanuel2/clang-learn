
int clangdMain(int argc, char *argv[]) {
  // Clang could run on the main thread. e.g., when the flag '-check' or '-sync'
  // is enabled.
  clang::noteBottomOfStack();
  llvm::InitLLVM X(argc, argv);
  llvm::InitializeAllTargetInfos();
  llvm::sys::AddSignalHandler(
      [](void *) {
        ThreadCrashReporter::runCrashHandlers();
        // Ensure ThreadCrashReporter and PrintStackTrace output is visible.
        llvm::errs().flush();
      },
      nullptr);
  llvm::sys::SetInterruptFunction(&requestShutdown);
  llvm::cl::SetVersionPrinter([](llvm::raw_ostream &OS) {
    OS << versionString() << "\n"
       << "Features: " << featureString() << "\n"
       << "Platform: " << platformString() << "\n";
  });
  const char *FlagsEnvVar = "CLANGD_FLAGS";
  const char *Overview =
      R"(clangd is a language server that provides IDE-like features to editors.

It should be used via an editor plugin rather than invoked directly. For more information, see:
	https://clangd.llvm.org/
	https://microsoft.github.io/language-server-protocol/

clangd accepts flags on the commandline, and in the CLANGD_FLAGS environment variable.
)";
  llvm::cl::HideUnrelatedOptions(ClangdCategories);
  llvm::cl::ParseCommandLineOptions(argc, argv, Overview,
                                    /*Errs=*/nullptr, FlagsEnvVar);
  if (Test) {
    if (!Sync.getNumOccurrences())
      Sync = true;
    if (!CrashPragmas.getNumOccurrences())
      CrashPragmas = true;
    InputStyle = JSONStreamStyle::Delimited;
    LogLevel = Logger::Verbose;
    PrettyPrint = true;
    // Disable config system by default to avoid external reads.
    if (!EnableConfig.getNumOccurrences())
      EnableConfig = false;
    // Disable background index on lit tests by default to prevent disk writes.
    if (!EnableBackgroundIndex.getNumOccurrences())
      EnableBackgroundIndex = false;
    // Ensure background index makes progress.
    else if (EnableBackgroundIndex)
      BackgroundQueue::preventThreadStarvationInTests();
  }
  if (Test || EnableTestScheme) {
    static URISchemeRegistry::Add<TestScheme> X(
        "test", "Test scheme for clangd lit tests.");
  }
  if (CrashPragmas)
    allowCrashPragmasForTest();

  if (!Sync && WorkerThreadsCount == 0) {
    llvm::errs() << "A number of worker threads cannot be 0. Did you mean to "
                    "specify -sync?";
    return 1;
  }

  if (Sync) {
    if (WorkerThreadsCount.getNumOccurrences())
      llvm::errs() << "Ignoring -j because -sync is set.\n";
    WorkerThreadsCount = 0;
  }
  if (FallbackStyle.getNumOccurrences())
    clang::format::DefaultFallbackStyle = FallbackStyle.c_str();

  // Validate command line arguments.
  std::optional<llvm::raw_fd_ostream> InputMirrorStream;
  if (!InputMirrorFile.empty()) {
    std::error_code EC;
    InputMirrorStream.emplace(InputMirrorFile, /*ref*/ EC,
                              llvm::sys::fs::FA_Read | llvm::sys::fs::FA_Write);
    if (EC) {
      InputMirrorStream.reset();
      llvm::errs() << "Error while opening an input mirror file: "
                   << EC.message();
    } else {
      InputMirrorStream->SetUnbuffered();
    }
  }

#if !CLANGD_DECISION_FOREST
  if (RankingModel == clangd::CodeCompleteOptions::DecisionForest) {
    llvm::errs() << "Clangd was compiled without decision forest support.\n";
    return 1;
  }
#endif

  // Setup tracing facilities if CLANGD_TRACE is set. In practice enabling a
  // trace flag in your editor's config is annoying, launching with
  // `CLANGD_TRACE=trace.json vim` is easier.
  std::optional<llvm::raw_fd_ostream> TracerStream;
  std::unique_ptr<trace::EventTracer> Tracer;
  const char *JSONTraceFile = getenv("CLANGD_TRACE");
  const char *MetricsCSVFile = getenv("CLANGD_METRICS");
  const char *TracerFile = JSONTraceFile ? JSONTraceFile : MetricsCSVFile;
  if (TracerFile) {
    std::error_code EC;
    TracerStream.emplace(TracerFile, /*ref*/ EC,
                         llvm::sys::fs::FA_Read | llvm::sys::fs::FA_Write);
    if (EC) {
      TracerStream.reset();
      llvm::errs() << "Error while opening trace file " << TracerFile << ": "
                   << EC.message();
    } else {
      Tracer = (TracerFile == JSONTraceFile)
                   ? trace::createJSONTracer(*TracerStream, PrettyPrint)
                   : trace::createCSVMetricTracer(*TracerStream);
    }
  }

  std::optional<trace::Session> TracingSession;
  if (Tracer)
    TracingSession.emplace(*Tracer);

  // If a user ran `clangd` in a terminal without redirecting anything,
  // it's somewhat likely they're confused about how to use clangd.
  // Show them the help overview, which explains.
  if (llvm::outs().is_displayed() && llvm::errs().is_displayed() &&
      !CheckFile.getNumOccurrences())
    llvm::errs() << Overview << "\n";
  // Use buffered stream to stderr (we still flush each log message). Unbuffered
  // stream can cause significant (non-deterministic) latency for the logger.
  llvm::errs().SetBuffered();
  // Don't flush stdout when logging, this would be both slow and racy!
  llvm::errs().tie(nullptr);
  StreamLogger Logger(llvm::errs(), LogLevel);
  LoggingSession LoggingSession(Logger);
  // Write some initial logs before we start doing any real work.
  log("{0}", versionString());
  log("Features: {0}", featureString());
  log("PID: {0}", llvm::sys::Process::getProcessId());
  {
    SmallString<128> CWD;
    if (auto Err = llvm::sys::fs::current_path(CWD))
      log("Working directory unknown: {0}", Err.message());
    else
      log("Working directory: {0}", CWD);
  }
  for (int I = 0; I < argc; ++I)
    log("argv[{0}]: {1}", I, argv[I]);
  if (auto EnvFlags = llvm::sys::Process::GetEnv(FlagsEnvVar))
    log("{0}: {1}", FlagsEnvVar, *EnvFlags);

  ClangdLSPServer::Options Opts;
  Opts.UseDirBasedCDB = (CompileArgsFrom == FilesystemCompileArgs);

  switch (PCHStorage) {
  case PCHStorageFlag::Memory:
    Opts.StorePreamblesInMemory = true;
    break;
  case PCHStorageFlag::Disk:
    Opts.StorePreamblesInMemory = false;
    break;
  }
  if (!ResourceDir.empty())
    Opts.ResourceDir = ResourceDir;
  Opts.BuildDynamicSymbolIndex = true;
  std::vector<std::unique_ptr<SymbolIndex>> IdxStack;
#if CLANGD_ENABLE_REMOTE
  if (RemoteIndexAddress.empty() != ProjectRoot.empty()) {
    llvm::errs() << "remote-index-address and project-path have to be "
                    "specified at the same time.";
    return 1;
  }
  if (!RemoteIndexAddress.empty()) {
    if (IndexFile.empty()) {
      log("Connecting to remote index at {0}", RemoteIndexAddress);
    } else {
      elog("When enabling remote index, IndexFile should not be specified. "
           "Only one can be used at time. Remote index will ignored.");
    }
  }
#endif
  Opts.BackgroundIndex = EnableBackgroundIndex;
  Opts.BackgroundIndexPriority = BackgroundIndexPriority;
  Opts.ReferencesLimit = ReferencesLimit;
  Opts.Rename.LimitFiles = RenameFileLimit;
  auto PAI = createProjectAwareIndex(loadExternalIndex, Sync);
  Opts.StaticIndex = PAI.get();
  Opts.AsyncThreadsCount = WorkerThreadsCount;
  Opts.MemoryCleanup = getMemoryCleanupFunction();

  Opts.CodeComplete.IncludeIneligibleResults = IncludeIneligibleResults;
  Opts.CodeComplete.Limit = LimitResults;
  if (CompletionStyle.getNumOccurrences())
    Opts.CodeComplete.BundleOverloads = CompletionStyle != Detailed;
  Opts.CodeComplete.ShowOrigins = ShowOrigins;
  Opts.CodeComplete.InsertIncludes = HeaderInsertion;
  Opts.CodeComplete.ImportInsertions = ImportInsertions;
  if (!HeaderInsertionDecorators) {
    Opts.CodeComplete.IncludeIndicator.Insert.clear();
    Opts.CodeComplete.IncludeIndicator.NoInsert.clear();
  }
  Opts.CodeComplete.EnableFunctionArgSnippets = EnableFunctionArgSnippets;
  Opts.CodeComplete.RunParser = CodeCompletionParse;
  Opts.CodeComplete.RankingModel = RankingModel;

  RealThreadsafeFS TFS;
  std::vector<std::unique_ptr<config::Provider>> ProviderStack;
  std::unique_ptr<config::Provider> Config;
  if (EnableConfig) {
    ProviderStack.push_back(
        config::Provider::fromAncestorRelativeYAMLFiles(".clangd", TFS));
    llvm::SmallString<256> UserConfig;
    if (llvm::sys::path::user_config_directory(UserConfig)) {
      llvm::sys::path::append(UserConfig, "clangd", "config.yaml");
      vlog("User config file is {0}", UserConfig);
      ProviderStack.push_back(config::Provider::fromYAMLFile(
          UserConfig, /*Directory=*/"", TFS, /*Trusted=*/true));
    } else {
      elog("Couldn't determine user config file, not loading");
    }
  }
  ProviderStack.push_back(std::make_unique<FlagsConfigProvider>());
  std::vector<const config::Provider *> ProviderPointers;
  for (const auto &P : ProviderStack)
    ProviderPointers.push_back(P.get());
  Config = config::Provider::combine(std::move(ProviderPointers));
  Opts.ConfigProvider = Config.get();

  // Create an empty clang-tidy option.
  TidyProvider ClangTidyOptProvider;
  if (EnableClangTidy) {
    std::vector<TidyProvider> Providers;
    Providers.reserve(4 + EnableConfig);
    Providers.push_back(provideEnvironment());
    Providers.push_back(provideClangTidyFiles(TFS));
    if (EnableConfig)
      Providers.push_back(provideClangdConfig());
    Providers.push_back(provideDefaultChecks());
    Providers.push_back(disableUnusableChecks());
    ClangTidyOptProvider = combine(std::move(Providers));
    Opts.ClangTidyProvider = ClangTidyOptProvider;
  }
  Opts.UseDirtyHeaders = UseDirtyHeaders;
  Opts.PreambleParseForwardingFunctions = PreambleParseForwardingFunctions;
  Opts.ImportInsertions = ImportInsertions;
  Opts.QueryDriverGlobs = std::move(QueryDriverGlobs);
  Opts.TweakFilter = [&](const Tweak &T) {
    if (T.hidden() && !HiddenFeatures)
      return false;
    if (TweakList.getNumOccurrences())
      return llvm::is_contained(TweakList, T.id());
    return true;
  };
  if (ForceOffsetEncoding != OffsetEncoding::UnsupportedEncoding)
    Opts.Encoding = ForceOffsetEncoding;

  if (CheckFile.getNumOccurrences()) {
    llvm::SmallString<256> Path;
    if (auto Error =
            llvm::sys::fs::real_path(CheckFile, Path, /*expand_tilde=*/true)) {
      elog("Failed to resolve path {0}: {1}", CheckFile, Error.message());
      return 1;
    }
    log("Entering check mode (no LSP server)");
    return check(Path, TFS, Opts)
               ? 0
               : static_cast<int>(ErrorResultCode::CheckFailed);
  }

  // Initialize and run ClangdLSPServer.
  // Change stdin to binary to not lose \r\n on windows.
  llvm::sys::ChangeStdinToBinary();
  std::unique_ptr<Transport> TransportLayer;
  if (getenv("CLANGD_AS_XPC_SERVICE")) {
#if CLANGD_BUILD_XPC
    log("Starting LSP over XPC service");
    TransportLayer = newXPCTransport();
#else
    llvm::errs() << "This clangd binary wasn't built with XPC support.\n";
    return static_cast<int>(ErrorResultCode::CantRunAsXPCService);
#endif
  } else {
    log("Starting LSP over stdin/stdout");
    TransportLayer = newJSONTransport(
        stdin, llvm::outs(), InputMirrorStream ? &*InputMirrorStream : nullptr,
        PrettyPrint, InputStyle);
  }
  if (!PathMappingsArg.empty()) {
    auto Mappings = parsePathMappings(PathMappingsArg);
    if (!Mappings) {
      elog("Invalid -path-mappings: {0}", Mappings.takeError());
      return 1;
    }
    TransportLayer = createPathMappingTransport(std::move(TransportLayer),
                                                std::move(*Mappings));
  }

  ClangdLSPServer LSPServer(*TransportLayer, TFS, Opts);
  llvm::set_thread_name("clangd.main");
  int ExitCode = LSPServer.run()
                     ? 0
                     : static_cast<int>(ErrorResultCode::NoShutdownRequest);
  log("LSP finished, exiting with status {0}", ExitCode);

  // There may still be lingering background threads (e.g. slow requests
  // whose results will be dropped, background index shutting down).
  //
  // These should terminate quickly, and ~ClangdLSPServer blocks on them.
  // However if a bug causes them to run forever, we want to ensure the process
  // eventually exits. As clangd isn't directly user-facing, an editor can
  // "leak" clangd processes. Crashing in this case contains the damage.
  abortAfterTimeout(std::chrono::minutes(5));

  return ExitCode;
}
