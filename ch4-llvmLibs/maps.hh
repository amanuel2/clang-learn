#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/DenseMap.h"

void testDenseMap()
{
    llvm::DenseMap<int, int> mp1;
    auto t1 = std::chrono::high_resolution_clock::now();
    for (int i=0; i<ITER; i++) mp1.insert({i, i});
    auto t2 = std::chrono::high_resolution_clock::now();
    mp1.clear();

    llvm::WithColor::remark() << "DenseMap insert perf: " <<
                    std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count() << "\n";

    // regular unordered_map
    std::unordered_map<int, int> mp2;
    auto t3 = std::chrono::high_resolution_clock::now();
    for (int i=0; i<ITER; i++) mp2.insert({i, i});
    auto t4 = std::chrono::high_resolution_clock::now();
    mp2.clear();

    llvm::WithColor::remark() << "unordered_map insert perf: " <<
                    std::chrono::duration_cast<std::chrono::milliseconds>(t4-t3).count() << "\n";
}

void testMaps()
{
    llvm::StringMap<int> mp1;
    auto t1 = std::chrono::high_resolution_clock::now();
    for (int i=0; i<ITER; i++) mp1.insert({"a", i});
    auto t2 = std::chrono::high_resolution_clock::now();

    mp1.clear();
    llvm::WithColor::remark() << "StringMap insert perf: " <<
                    std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count() << "\n";

    // regular unordered_map
    std::unordered_map<std::string, int> mp2;
    auto t3 = std::chrono::high_resolution_clock::now();
    for (int i=0; i<ITER; i++) mp2.insert({"a", i});
    auto t4 = std::chrono::high_resolution_clock::now();
    mp2.clear();

    llvm::WithColor::remark() << "unordered_map insert perf: " <<
                    std::chrono::duration_cast<std::chrono::milliseconds>(t4-t3).count() << "\n";
}