#pragma once

#include "Consumer.hh"
#include "clang/AST/ASTConsumer.h"
#include "clang/Frontend/FrontendAction.h"
#include "llvm/ADT/StringRef.h"
#include <memory>

namespace Aman {
    namespace DeclVisitor {
        class AmanFrontendAction: public clang::ASTFrontendAction {
            public:
                explicit AmanFrontendAction() {}
                virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &inst, llvm::StringRef file) override {
                    return std::make_unique<AmanConsumer>();
                }

                // virtual bool usesPreprocessorOnly() const override { return false; }
                // virtual void ExecuteAction() override { 
                //     // auto &CI = getCompilerInstance();
                //     // CI.createASTContext();
                //     // CI.createSema(getTranslationUnitKind(), nullptr);
                //     // CI.getPreprocessor().EnterMainSourceFile();
                //         // clang::FrontendAction::ExecuteAction();
                //     // Execute the AmanConsumer here
                //     // clang::FrontendAction::BeginSourceFileAction();
                //  }
        };
    }
}