#pragma once

#include "Consumer.hh"
#include "clang/AST/ASTConsumer.h"
#include "clang/Frontend/FrontendAction.h"
#include "llvm/ADT/StringRef.h"
#include <memory>

namespace Aman {
    namespace MethodRename {
        class AmanFrontendAction: public clang::ASTFrontendAction {
            public:
                // explicit AmanFrontendAction() {}
                virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &inst, llvm::StringRef file) override {
                    return std::make_unique<Aman::MethodRename::AmanConsumer>();
                }
        };
    }
}