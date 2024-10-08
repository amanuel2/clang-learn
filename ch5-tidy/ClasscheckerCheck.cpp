//===--- ClasscheckerCheck.cpp - clang-tidy -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "ClasscheckerCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "llvm/Support/WithColor.h"

using namespace clang::ast_matchers;

namespace clang::tidy::misc {

void ClasscheckerCheck::registerMatchers(MatchFinder *Finder) {
  // FIXME: Add matchers.
  // Finder->addMatcher(functionDecl().bind("x"), this);
  Finder->addMatcher(cxxRecordDecl().bind("class"), this);
}

void ClasscheckerCheck::check(const MatchFinder::MatchResult &Result) {
  // FIXME: Add callback implementation.
  // const auto *MatchedDecl = Result.Nodes.getNodeAs<FunctionDecl>("x");
  // if (!MatchedDecl->getIdentifier() || MatchedDecl->getName().startswith("awesome_"))
  //   return;
  // diag(MatchedDecl->getLocation(), "function %0 is insufficiently awesome")
  //     << MatchedDecl
  //     << FixItHint::CreateInsertion(MatchedDecl->getLocation(), "awesome_");
  // diag(MatchedDecl->getLocation(), "insert 'awesome'", DiagnosticIDs::Note);

  const auto *ClassDecl = Result.Nodes.getNodeAs<CXXRecordDecl>("class");

  if (!ClassDecl) {
    llvm::WithColor::error() << "Couldn't gett class decl\n";
    return;
  }

  uint32_t cnt;

  // get decls inside class
  for (const auto* C : ClassDecl->decls()) {
    if (isa<CXXMethodDecl>(C)) cnt++;
  }

  uint32_t thresh = Options.get("Threshold", 5);
  if (cnt > thresh) {
    // llvm::WithColor::warning() << "WARNING 🛑 -> Surpasses more than threshold\n";
    diag(ClassDecl->getLocation(), "class %0 is too complex: method count = %1", DiagnosticIDs::Warning) << ClassDecl->getName() << cnt;
  }
  else llvm::WithColor::remark() << "SUCCESS ✅\n";
  
}

} // namespace clang::tidy::misc
