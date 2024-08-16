#pragma once

#include "Visitor.hh"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include <memory>
namespace Aman {
    namespace DeclVisitor {
        class AmanConsumer: public clang::ASTConsumer {
            public:
                AmanConsumer() : amaaaaan_visitor(std::make_unique<AmanVisitor>()) {}
                void HandleTranslationUnit(clang::ASTContext &Ctx) {
                    llvm::outs() << "Handling Trans Unit" << Ctx.getTranslationUnitDecl() << "\n";
                    amaaaaan_visitor->Visit(Ctx.getTranslationUnitDecl());
                }
            private:
                std::unique_ptr<AmanVisitor> amaaaaan_visitor;
        };
    }
}