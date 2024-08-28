### Explicit modules
$ <...>/llvm-project/install/bin/clang -cc1            \
        -emit-module -o header1.pcm                    \
        -fmodules module.modulemap -fmodule-name=header1 \
        -x c++-header -fno-implicit-modules

Explicitly generate PCM's for each headers pretaining a modulemap

### Implicit Modules

```
#include "header1.h" 
 
 
 
int main() { 
 

  int h = h1 + h2; 
 
  return 0; 
 
}
```

The code will not compile:

```
$ <...>/llvm-project/install/bin/clang  main.cpp -o main -lstdc++
main.cpp:4:16: error: use of undeclared identifier ’h2’
  int h = h1 + h2;
               ^
 error generated.
```

The error is obvious because we didn’t include the second header that contains a definition for the h2 variable.

The situation would be different if we were using implicit modules. Consider the following module.modulemap file:

```
module h1 { 
 
  header "header1.h" 
 
  export * 
 

  module h2 { 
 
    header "header2.h" 
 
    export * 
 

  } 
 
}
```


Then Compile... 

```
$ <...>/llvm-project/install/bin/clang -cc1 \
        -emit-obj main.cpp                \
        -fmodules                         \
        -fmodule-map-file=module.modulemap\
        -fmodules-cache-path=./cache      \
        -o main.o
$ <...>/llvm-project/install/bin/clang main.o -o main -lstdc++
```

```
$ tree ./cache
./cache
|-- 2AL78TH69W6HR
    |-- header1-R65CPR1VCRM1.pcm
    |-- header2-R65CPR1VCRM1.pcm
    |-- modules.idx
 directories, 3 files
```
