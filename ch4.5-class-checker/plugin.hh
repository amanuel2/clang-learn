#pragma once

#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "consumer.hh"

namespace Aman {
    namespace ClassChecker {
        using clang::CompilerInstance;
        using clang::StringRef;

        class AmanPlugin : public clang::PluginASTAction {
            public:
                // explicit AmanPlugin(std::unique_ptr<AmanConsumer>) : Consumer(std::move(Consumer)) {};
                virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                                        StringRef InFile) override
                {
                    return std::make_unique<Aman::ClassChecker::AmanConsumer>(&CI.getASTContext(), T);
                }

                virtual bool ParseArgs(const CompilerInstance &CI,
                                       const std::vector<std::string> &args) override
                {
                    for (const auto& arg : args) {
                        if (arg.substr(0, 9) == "threshold") {
                            auto val = arg.substr(10);
                            T = std::stoi(val);
                            return true;
                        }
                        llvm::outs() << "Arg: " << arg << "\n";
                    }

                    llvm::outs() << "Couldnt find arg!\n";
                    return false;
                }
            
                virtual ActionType getActionType() override { return AddAfterMainAction; }

            private:
                // std::unique_ptr<AmanConsumer> Consumer;
                int T;
        };
    }
}