#include "FrontendAction.hh"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/CompilationDatabase.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"

namespace {
    llvm::cl::OptionCategory Cat("Test Category");
    llvm::cl::extrahelp Helper(clang::tooling::CommonOptionsParser::HelpMessage);
}

int main(int argc, const char** argv) {
    auto OptionsParser = clang::tooling::CommonOptionsParser::create(argc, argv, Cat);
    if (!OptionsParser) {
        llvm::errs() << "Error creating Option parser";
        return 1;
    }

    std::string errMsg = "";
    auto compilation = clang::tooling::CompilationDatabase::loadFromDirectory(".", errMsg);
    if(!compilation) {
        llvm::errs() << "Could not locate comp db";
        return 1;
    }

    clang::tooling::ClangTool tool(*compilation, OptionsParser->getSourcePathList());
    auto factory = clang::tooling::newFrontendActionFactory<Aman::DeclVisitor::AmanFrontendAction>();
    return tool.run(factory.get());
}