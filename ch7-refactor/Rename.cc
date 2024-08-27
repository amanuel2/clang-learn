#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "FrontendAction.hh"

namespace {
    llvm::cl::OptionCategory Cat("Renamer");
    llvm::cl::extrahelp CommonHelp(clang::tooling::CommonOptionsParser::HelpMessage);
}

int main (int argc, const char** argv) 
{
  llvm::Expected<clang::tooling::CommonOptionsParser> OptionsParser =
      clang::tooling::CommonOptionsParser::create(argc, argv, Cat);
  if (!OptionsParser) {
    llvm::errs() << OptionsParser.takeError();
    return 1;
  }
  clang::tooling::ClangTool Tool(OptionsParser->getCompilations(),
                                 OptionsParser->getSourcePathList());

  return Tool.run(clang::tooling::
                      newFrontendActionFactory<Aman::MethodRename::AmanFrontendAction>()
                      .get());
}