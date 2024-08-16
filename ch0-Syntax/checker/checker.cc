#include <iostream>
#include <filesystem>
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/CompilationDatabase.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"

namespace fs = std::filesystem;

namespace {
llvm::cl::OptionCategory TestCategory("Test project");
llvm::cl::extrahelp CommonHelp(clang::tooling::CommonOptionsParser::HelpMessage);
} // namespace

class SyntaxCheckAction : public clang::SyntaxOnlyAction {
public:
  SyntaxCheckAction() {}

protected:
  void EndSourceFileAction() override {
    clang::SyntaxOnlyAction::EndSourceFileAction();
    llvm::outs() << "Finished processing " << getCurrentFile() << "\n";
  }
};

class SyntaxCheckActionFactory : public clang::tooling::FrontendActionFactory {
public:
  std::unique_ptr<clang::FrontendAction> create() override {
    return std::make_unique<SyntaxCheckAction>();
  }
};

int main(int argc, const char** argv) {
    llvm::outs() << "Step1: Parsing options\n";
    auto expectedParser = clang::tooling::CommonOptionsParser::create(argc, argv, TestCategory);
    if (!expectedParser) {
        llvm::errs() << expectedParser.takeError();
        return 1;
    }
    clang::tooling::CommonOptionsParser& optionsParser = expectedParser.get();

    llvm::outs() << "Step2: Loading compilation database\n";
    std::string errorMessage;
    std::unique_ptr<clang::tooling::CompilationDatabase> compilations =
        clang::tooling::CompilationDatabase::loadFromDirectory(".", errorMessage);
    
    if (!compilations) {
        llvm::errs() << "Error loading compilation database: " << errorMessage << "\n";
        llvm::errs() << "Falling back to FixedCompilationDatabase\n";
        compilations = std::make_unique<clang::tooling::FixedCompilationDatabase>(".", std::vector<std::string>());
    }

    llvm::outs() << "Step3: Creating ClangTool\n";
    clang::tooling::ClangTool tool(*compilations, optionsParser.getSourcePathList());

    llvm::outs() << "Step4: Running the tool\n";
    SyntaxCheckActionFactory factory;
    int result = tool.run(&factory);
    
    if (result != 0) {
        llvm::errs() << "Error: Tool execution failed. Exit code: " << result << "\n";
    } else {
        llvm::outs() << "Syntax check completed successfully.\n";
    }

    return result;
}