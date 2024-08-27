#include "clang/Rewrite/Core/Rewriter.h"
#include "Visitor.hh"

namespace Aman::MethodRename {
    class AmanConsumer : public clang::ASTConsumer {
        public:

            virtual void HandleTranslationUnit(clang::ASTContext &Context) override
            {
                AmanVisitor Visitor(&Context);
                // Recursively visit a declaration, by dispatching to Traverse*Decl()
                Visitor.TraverseDecl(Context.getTranslationUnitDecl());

                // Apply transforms (replace changes)
                // Rewritter under important files
                clang::Rewriter Rewrite(Context.getSourceManager(), clang::LangOptions());
                auto Replacements = Visitor.getRepls();

                // Specific implementation of Repl under Tooling/Core/clang::Replacements
                for (const auto& Repl : Replacements)
                    if(Repl.isApplicable()) 
                        Repl.apply(Rewrite);
                
                // Apply Rewritter changes (save to disk)
                auto res = Rewrite.overwriteChangedFiles();
                if (!res) llvm::WithColor::error() << "❌ Failed to overrwrite changes to disk";
            }
    };
}