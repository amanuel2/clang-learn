#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Twine.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringMap.h"
#include <chrono>

static constexpr std::uint32_t ITER = 100000000;

void testStr()
{
    llvm::StringRef str("Aman");
    llvm::StringRef sub = str.substr(1, 2);

    llvm::WithColor::remark() << "str: " << &str << "\n";
    llvm::WithColor::remark() << "sub: " << &sub << "\n"; // 1 byte off
}

void testConcat()
{
    llvm::StringRef str("Mom");
    llvm::StringRef str2("Dad");

    std::string str3("Mom");
    std::string str4("Dad");

    llvm::SmallString<3> str5("Mom");
    llvm::SmallString<3> str6("Dad");

    // Regular
    auto t1 = std::chrono::high_resolution_clock::now();
    for(int i=0; i<ITER; i++)
        std::string t = str3 + str4;
    auto t2 = std::chrono::high_resolution_clock::now();

    llvm::WithColor::remark() << "Regular perf: " <<
                    std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count() << "\n";

    // Twine
    auto t3 = std::chrono::high_resolution_clock::now();
    for(int i=0; i<ITER; i++)
        llvm::Twine t = str + str2;
    auto t4 = std::chrono::high_resolution_clock::now();

    llvm::WithColor::remark() << "Twine perf: " <<
                    std::chrono::duration_cast<std::chrono::milliseconds>(t4-t3).count() << "\n";

    // With Small String + Twine
    auto t5 = std::chrono::high_resolution_clock::now();
    for(int i=0; i<ITER; i++)
        llvm::Twine t = str5 + str6;
    auto t6 = std::chrono::high_resolution_clock::now();

    llvm::WithColor::remark() << "Small+Twine perf: " <<
                    std::chrono::duration_cast<std::chrono::milliseconds>(t6-t5).count() << "\n";
}