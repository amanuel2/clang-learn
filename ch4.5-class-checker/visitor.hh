#pragma once

#include "clang/AST/RecursiveASTVisitor.h"
#include "llvm/Support/WithColor.h"

namespace Aman {
    namespace ClassChecker {
        class AmanVisitor : public clang::RecursiveASTVisitor<AmanVisitor> {
            public:
                explicit AmanVisitor(clang::ASTContext *Ctx, int T): Ctx(Ctx), thresh(T) {};

                bool VisitCXXRecordDecl(clang::CXXRecordDecl* Decl) {
                    llvm::WithColor::note() << "Visiting class " << Decl->getNameAsString() << "\n";

                    if (Decl->isThisDeclarationADefinition()) {
                        int cnt = std::distance(Decl->methods().begin(), Decl->methods().end());
                        if (cnt > thresh) {
                            clang::DiagnosticsEngine &DiagEngine = Ctx->getDiagnostics();
                            unsigned DiagID = DiagEngine.getCustomDiagID(clang::DiagnosticsEngine::Warning, "Class %0 has more than %1 methods");
                            clang::DiagnosticBuilder DiagBuilder = DiagEngine.Report(Decl->getLocation(), DiagID);
                            DiagBuilder << Decl->getNameAsString() << "With thresh: " << thresh << " But has method count " << cnt;
                        }
                    }

                    return true;
                }
            private:
                clang::ASTContext* Ctx;
                int thresh;
        };
    }
}