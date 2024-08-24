# Clang tidy simple module checker

Files placed under clang-tools-extra/clang-tidy/ will be automatically picked up by the clang-tidy tool. This module is a simple checker that will check for the presence of a module declaration in the source file.

Also for tests under clang-tools-extra/clang-tidy/test/clang-tidy/ the module declaration will be checked.

To generate modules for clang-tidy run the following command:

```bash
python3 build/clang-tidy/add_new_check.py moduele checker_name
```