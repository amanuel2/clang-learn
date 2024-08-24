#include "llvm/ADT/IntrusiveRefCntPtr.h"

void testPtrs()
{
    // // benchmark between shared_ptr and IntrusiveRefCntPtr
    // llvm::IntrusiveRefCntPtr<int> ptr1 = new int(10);
    // llvm::IntrusiveRefCntPtr<int> ptr2 = new int(20);
    // llvm::IntrusiveRefCntPtr<int> ptr3 = new int(30);
    // llvm::IntrusiveRefCntPtr<int> ptr4 = new int(40);
    // llvm::IntrusiveRefCntPtr<int> ptr5 = new int(50);
    // llvm::IntrusiveRefCntPtr<int> ptr6 = new int(60);
    // llvm::IntrusiveRefCntPtr<int> ptr7 = new int(70);
    // llvm::IntrusiveRefCntPtr<int> ptr8 = new int(80);
    // llvm::IntrusiveRefCntPtr<int> ptr9 = new int(90);
    // llvm::IntrusiveRefCntPtr<int> ptr10 = new int(100);

    // auto t1 = std::chrono::high_resolution_clock::now();
    // for(int i=0; i<ITER; i++)
    // {
    //     llvm::IntrusiveRefCntPtr<int> ptr = new int(10);
    //     ptr = ptr1;

    // }
    // auto t2 = std::chrono::high_resolution_clock::now();

    // llvm::WithColor::remark() << "IntrusiveRefCntPtr perf: " <<
    //                 std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count() << "\n";

    // auto t3 = std::chrono::high_resolution_clock::now();
    // for(int i=0; i<ITER; i++)
    // {
    //     std::shared_ptr<int> ptr = std::make_shared<int>(10);
    //     ptr = ptr1;
    //     ptr = ptr2;
    //     ptr = ptr3;
    //     ptr = ptr4;
    //     ptr = ptr5;
    //     ptr = ptr6;
    //     ptr = ptr7;
    //     ptr = ptr8;
    //     ptr = ptr9;
    //     ptr = ptr10;
    // }
    // auto t4 = std::chrono::high_resolution_clock::now();

    // llvm::WithColor::remark() << "shared_ptr perf: " <<
    //                 std::chrono::duration_cast<std::chrono::milliseconds>(t4-t3).count() << "\n";
}