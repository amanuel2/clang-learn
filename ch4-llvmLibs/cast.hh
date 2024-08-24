#include <memory>
#include "llvm/Support/Casting.h"
#include "llvm/Support/WithColor.h"

namespace Aman {

    class Animal {
        public:
            Animal() = default;
    };

    class Dog : public Animal {
        public:
            Dog() = default;
            static bool classof(const Animal *) { return true; };
    };

    class Cat : public Animal {
        public:
            Cat() = default;
            static bool classof(const Animal *) { return true; };
    };

}

void testCasting()
{
    std::unique_ptr<Aman::Animal> anim_ptr = std::make_unique<Aman::Dog>();
    auto isaTest = llvm::isa<Aman::Dog>(anim_ptr);

    if (!isaTest) {
        llvm::WithColor::error() << "ISA Failed\n";
        return;
    }

    std::unique_ptr<Aman::Dog> dog = llvm::dyn_cast<Aman::Dog>(anim_ptr);
    if (dog == nullptr)
        llvm::WithColor::error() << "Dynamic cast failed\n";
    else
        llvm::WithColor::remark() << "Casting success\n";
}