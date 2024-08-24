#pragma once

#include "clang/AST/ASTConsumer.h"
#include "llvm/Support/WithColor.h"

namespace Aman {
    namespace ClassChecker {
        class AmanConsumer : public clang::ASTConsumer {
            public:
            explicit AmanConsumer(clang::ASTContext* Ctx, int T) : Visitor(Ctx, T) {};

            virtual void HandleTranslationUnit(clang::ASTContext &Ctx) {
                llvm::WithColor(llvm::outs(), llvm::HighlightColor::Remark) << "Traversing Decl\n";
                Visitor.TraverseDecl(Ctx.getTranslationUnitDecl());
            }
            
            // friend class AmanPlugin;
            private:
                AmanVisitor Visitor;
        };
    }
}

