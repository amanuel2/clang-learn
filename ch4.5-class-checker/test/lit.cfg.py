import lit.formats

config.name = 'classchecker'
config.test_format = lit.formats.ShTest(True)
config.suffixes = ['.cc']
config.test_source_root = os.path.dirname(__file__)

config.substitutions.append(('%clang-binary', config.ClangBinary))
config.substitutions.append(('%path-to-plugin', os.path.join(config.ClassComplexityChecker_obj_root, 'libclasschecker.so')))
config.substitutions.append(('%file-check-binary', config.FileCheck))