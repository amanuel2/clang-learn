#pragma once

#include "config.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclVisitor.h"
#include "clang/AST/RecursiveASTVisitor.h" 

namespace Aman {
    namespace DeclVisitor {
        #if RECURSIVE
        class AmanVisitor : public clang::RecursiveASTVisitor<AmanVisitor> {
        #else
        class AmanVisitor : public clang::DeclVisitor<AmanVisitor> {
        #endif
            public:
            #if RECURSIVE
                bool
            #else
                void
            #endif
                VisitFunctionDecl(const clang::FunctionDecl *FD) {
                llvm::outs() << "Function: " << FD->getName() << "\n";

                #if !RECURSIVE
                for (auto param: FD->parameters())
                    Visit(param);
                #else
                    return true;
                #endif
            };


            #if RECURSIVE
                bool
            #else
                void
            #endif
                VisitParmVarDecl(const clang::ParmVarDecl *PVD) {
                    llvm::outs() << "\t Param: " << PVD->getName() << "\n";
                    #if RECURSIVE
                        return true;
                    #endif
                }


            #if RECURSIVE
                bool
            #else
                void
            #endif
                VisitTranslationUnitDecl(const clang::TranslationUnitDecl *TVD) {
                    llvm::outs() << "Translation: " << "\n"; 
                    #if !RECURSIVE
                    for(auto decl: TVD->decls()) 
                        Visit(decl);
                    #else
                        llvm::outs() << "Going through trans unit\n";
                        return true;
                    #endif
                }
        };
    }
}