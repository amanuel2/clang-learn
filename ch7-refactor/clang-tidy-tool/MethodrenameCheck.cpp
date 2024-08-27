//===--- MethodrenameCheck.cpp - clang-tidy -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MethodrenameCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang::tidy::misc {

/// From Clang Docs
/// ---------------
/// AST_MATCHER(Type, DefineMatcher) { ... }
/// defines a zero parameter function named DefineMatcher() that returns a
/// Matcher<Type> object.
///
/// The code between the curly braces has access to the following variables:
///
///   Node:                  the AST node being matched; its type is Type.
///   Finder:                an ASTMatchFinder*.
///   Builder:               a BoundNodesTreeBuilder*.
///
/// The code should return true if 'Node' matches.
AST_MATCHER(CXXMethodDecl, isNotTestMethod) {
  if (Node.getAccess() != clang::AS_public) return false;
  if (llvm::isa<clang::CXXConstructorDecl>(&Node)) return false;
  if (llvm::isa<clang::CXXDestructorDecl>(&Node)) return false;
  if (!Node.getIdentifier() || Node.getName().startswith("test_")) return false;

  return true;
}

void MethodrenameCheck::registerMatchers(MatchFinder *Finder) {
  using namespace clang::ast_matchers::internal;
  ArgumentAdaptingMatcherFuncAdaptor ClassMatcher  = hasAncestor(cxxRecordDecl(matchesName("::Test.*$")));
  BindableMatcher MethodMatcher = cxxMethodDecl(isNotTestMethod(), ClassMatcher);
  BindableMatcher CallMatcher = cxxMemberCallExpr(callee(MethodMatcher));

  Finder->addMatcher(MethodMatcher.bind("method"), this);
  Finder->addMatcher(CallMatcher.bind("call"), this);
}

void MethodrenameCheck::check(const MatchFinder::MatchResult &Result) {
  if (const auto *Method = Result.Nodes.getNodeAs<CXXMethodDecl>("method")) {
    processMethod(Method, Method->getLocation(), "Method");
  } 

  if (const auto *Call = Result.Nodes.getNodeAs<CXXMemberCallExpr>("call")) {
    processMethod(Call->getMethodDecl(), Call->getExprLoc(), "Call");
  }
}

void MethodrenameCheck::processMethod(const clang::CXXMethodDecl *Method,
                                      clang::SourceLocation StartLoc,
                                      const char *LogMessage) {
  diag(StartLoc, "%0 %1 does not have 'test_' prefix", DiagnosticIDs::Warning) << LogMessage << Method;
  diag(StartLoc, "insert 'test_'", DiagnosticIDs::Note)
      << FixItHint::CreateInsertion(StartLoc, "test_");

      /*
        The Fix It Int prints out the:

         12 |   test.pos();
      |        ^
      |        test_
      */
}

} // namespace clang::tidy::misc
