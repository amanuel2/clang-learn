#include "llvm/Support/CommandLine.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "MatchCb.hh"

namespace {
    llvm::cl::OptionCategory Cat("Test Category");
    llvm::cl::extrahelp CommonHelp(clang::tooling::CommonOptionsParser::HelpMessage);
}

int main(int argc, const char** argv) {
    auto OptionsParser = clang::tooling::CommonOptionsParser::create(argc, argv, Cat);

    std::string err("couldnt locate");
    auto compilation = clang::tooling::CompilationDatabase::loadFromDirectory(".", err);
    if (!compilation) {
        llvm::errs() << "Couldnt detect comp db\n";
        return 1;
    }


    clang::tooling::ClangTool tool(*compilation, OptionsParser->getSourcePathList());
    
    // our custom cb
    Aman::Finder::MatchCB cb;

    // add it to clang
    clang::ast_matchers::MatchFinder Finder;
    Finder.addMatcher(Aman::Finder::M, &cb);

    return tool.run(clang::tooling::newFrontendActionFactory(&Finder).get());
}