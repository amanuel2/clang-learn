//===--- CfgcomplexityCheck.cpp - clang-tidy ------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "CfgcomplexityCheck.h"
#include "clang/AST/ASTContext.h"
#include "llvm/Support/WithColor.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang::tidy::misc {

void CfgcomplexityCheck::registerMatchers(MatchFinder *Finder) {
  // FIXME: Add matchers.
  Finder->addMatcher(functionDecl().bind("func"), this);
}

void CfgcomplexityCheck::check(const MatchFinder::MatchResult &Result) {
  // FIXME: Add callback implementation.
  // const auto *MatchedDecl = Result.Nodes.getNodeAs<FunctionDecl>("x");
  // if (!MatchedDecl->getIdentifier() || MatchedDecl->getName().startswith("awesome_"))
  //   return;
  // diag(MatchedDecl->getLocation(), "function %0 is insufficiently awesome")
  //     << MatchedDecl
  //     << FixItHint::CreateInsertion(MatchedDecl->getLocation(), "awesome_");
  // diag(MatchedDecl->getLocation(), "insert 'awesome'", DiagnosticIDs::Note);

  const auto *MatchedDecl = Result.Nodes.getNodeAs<FunctionDecl>("func");
  if(!MatchedDecl) {
    llvm::WithColor::error() << "⛔️ No Matched Func";
    return;
  }

  if (!MatchedDecl->hasBody()) {
    llvm::WithColor::error() << "⛔️ No Body";
    return;
  }

  auto cfg = clang::CFG::buildCFG(MatchedDecl, MatchedDecl->getBody(), Result.Context, CFG::BuildOptions());
  uint32_t thresh = Options.get("Threshold", 5);
  uint32_t complex = this->complexFunc(cfg.get());

  if (complex > thresh) {
    diag(MatchedDecl->getLocation(), "function %0 has high cyclomatic complexity (%1)")
        << MatchedDecl << complex;
  }
}

uint32_t CfgcomplexityCheck::complexFunc(clang::CFG * cfg) const
{
  uint32_t edges=0, nodes=0;
  
  for(const auto block: *cfg) {
    edges += block->size();
    nodes++;
  }

  return (edges-nodes)+2; // + start,end nodes
}

} // namespace clang::tidy::misc
