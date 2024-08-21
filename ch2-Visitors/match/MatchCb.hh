
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"

namespace Aman {
    namespace Finder {
        using namespace clang::ast_matchers;
        static const char *MatchID = "match-id";

        DeclarationMatcher M = functionDecl(decl().bind(MatchID), matchesName("max"));

        class MatchCB : public MatchFinder::MatchCallback {
            virtual void run(const MatchFinder::MatchResult &Result) final {
                if (const auto *FD = Result.Nodes.getNodeAs<clang::FunctionDecl>(MatchID)) {
                    const auto &SM = *Result.SourceManager;
                    const auto &Loc = FD->getLocation();
                    llvm::outs().changeColor(llvm::raw_ostream::Colors::MAGENTA, false) << "Found 'max' function at " << SM.getFilename(Loc) << ":"
                                << SM.getSpellingLineNumber(Loc) << ":"
                                << SM.getSpellingColumnNumber(Loc) << "\n";
                } else {
                    llvm::outs() << "Couldn't find function \"max\"\n";
                }
            }
        };
    }
}