#include "lib.h"
#include "version.h"
#include "custom_alloc.h"
#include "one_way_list_container.h"

#include <iostream>
#include <map>

using AllocatorInt  = CustomAllocator<int, 10>;
using AllocatorPair = CustomAllocator<std::pair<const int, int>, 10>;

long long calculate_factorial(int n)
{
    long long factorial = 1;
    
    for (int i = 1; i <= n; ++i) {
        factorial *= i;
    }

    return factorial;
}

// Тетирование своего аллокатора и std::map
void test_std_map() {
    using TestMap = std::map<int, int, std::less<int>, AllocatorPair>;

    auto test_map = TestMap{};

    for (int i = 0; i < 10; ++i) {
        test_map[i] = static_cast<int>(calculate_factorial(i));
    }

    std::cout << std::endl;
    for (auto test_pair : test_map) {
        std::cout << test_pair.first << " " << test_pair.second << std::endl;
    }
    std::cout << std::endl;
}

// Тестирование своего аллокатора и своего контейнера
void test_custom_container() {
    using TestList = OneWayList<int, AllocatorInt>;

    TestList owl_cont;
    
    for (int i = 0; i < 10; ++i) {
        owl_cont.push_back(i);
    }
    
    std::cout << std::endl;
    std::cout << owl_cont << std::endl;
    std::cout << std::endl;
}

int version() {
    return PROJECT_VERSION_PATCH;
}