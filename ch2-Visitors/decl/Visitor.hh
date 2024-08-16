#pragma once

#include "clang/AST/Decl.h"
#include "clang/AST/DeclVisitor.h"


namespace Aman {
    namespace DeclVisitor {
        class AmanVisitor : public clang::DeclVisitor<AmanVisitor> {
            public:
            void VisitFunctionDecl(const clang::FunctionDecl *FD) {
                llvm::outs() << "Function: " << FD->getName() << "\n";

                for (auto param: FD->parameters()) {
                    Visit(param);
                }
            };

            void VisitParamVarDecl(const clang::ParmVarDecl *PVD) {
                llvm::outs() << "Var " << PVD->getName();
            }

            void VisitTranslationUnitDecl(const clang::TranslationUnitDecl *TVD) {
                for(auto decl: TVD->decls()) Visit(decl);
            }
        };
    }
}