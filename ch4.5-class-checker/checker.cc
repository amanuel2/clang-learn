#include "visitor.hh"
#include "consumer.hh"
#include "plugin.hh"
#include "clang/Frontend/FrontendPluginRegistry.h"

/// Add a node to the Registry: this is the interface between the plugin and
/// the executable.
static clang::FrontendPluginRegistry::Add<Aman::ClassChecker::AmanPlugin> X("classchecker", "Checks complexity of class");