// RUN: %check_clang_tidy %s misc-classchecker %t

class Simple {
    public:
        void func(){};
        void func2(){};
};

// CHECK-MESSAGES: :[[@LINE+1]]:{{[0-9]+}}: warning: class Complex is too complex: method count = 6 [misc-classchecker] 
class Complex {
    public:
        void func(){};
        void func2(){};
        void func3(){};
        void func4(){};
};
