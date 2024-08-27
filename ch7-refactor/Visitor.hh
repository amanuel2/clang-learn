#include "clang/AST/RecursiveASTVisitor.h"
#include "llvm/Support/WithColor.h"

#include "clang/Tooling/Refactoring.h"
using Replacements = clang::tooling::Replacement;

namespace Aman::MethodRename
{
    class AmanVisitor : public clang::RecursiveASTVisitor<AmanVisitor> {
        public:
            explicit AmanVisitor(clang::ASTContext *Ctx) : Ctx(Ctx) {};

            bool VisitCXXRecordDecl(clang::CXXRecordDecl *Decl) {
                if (!Decl->isClass()) return true;

                if (!Decl->isThisDeclarationADefinition()) return true;
                if (!Decl->getIdentifier() || !Decl->getName().startswith("Test")) return true; // Class Name

                // now we can process methods through class
                for (const clang::CXXMethodDecl *Method : Decl->methods()) {
                    if (Method->getIdentifier() && !Method->getName().starts_with("test_")) {
                        llvm::outs() << "  Found method: " << Method->getName() << "\n";
                        RenameMethod(*Method, Method->getLocation(), "Renamed Method Expr");
                    }
                }

                return true;
            }

            bool VisitCXXMemberCallExpr(clang::CXXMemberCallExpr *Call) {
                auto CXXMethodDecl = Call->getMethodDecl();
                if (!CXXMethodDecl) {
                    llvm::WithColor::warning() << "⚠️ Warning: Couldn't find Method Decl from Call Expression";
                    return true;
                }

                if (!CXXMethodDecl->getIdentifier()) {
                    llvm::WithColor::warning() << "⚠️ Warning: Method has no identifier\n";
                    return true;
                }

                llvm::outs() << "Visiting member call: " << CXXMethodDecl->getName() << "\n";

                auto* CXXDecl = CXXMethodDecl->getParent();
                if (!CXXDecl) {
                    llvm::WithColor::warning() << "⚠️ Warning: Couldn't find Class Decl from Method Decl";
                    return true;
                } else if (!CXXDecl->getIdentifier() || !CXXDecl->getName().starts_with("Test")) return true;

                clang::SourceLocation Start = Call->getExprLoc();
                return RenameMethod(*CXXMethodDecl, Start, "Renamed Method Call");
            }

            // cant friend since double dep
            std::vector<Replacements> getRepls() { return this->Repls; }
        private:
            clang::ASTContext* Ctx;
            std::vector<Replacements> Repls;

        bool RenameMethod(const clang::CXXMethodDecl &Decl, 
                          clang::SourceLocation StartLoc, 
                          const llvm::StringLiteral &LogMessage)
        {
            if (Decl.getAccess() != clang::AS_public) return true;
            if (llvm::isa<clang::CXXConstructorDecl>(Decl)) return true;
            if (llvm::isa<clang::CXXDestructorDecl>(Decl)) return true;

            if (!Decl.getIdentifier() || Decl.getName().startswith("test_")) return true;

            std::string newName = "test_" + Decl.getName().str();
            Replacements Repl(Ctx->getSourceManager(), 
                              StartLoc,
                              Decl.getName().size(),
                              newName
                              );
            Repls.push_back(Repl);

            llvm::WithColor::remark() << "✅ Replaced Non test_ abbreviated method: " << Decl.getName() << " to " << newName << "\n";
            return true;
        }
    };
}