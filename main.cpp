#include "lib.h"
#include <iostream>

int main (int, char **) {
	// Тетирование своего аллокатора и std::map
	test_std_map_with_custom_allocator();

	std::cout << std::endl;

	// Тестирование своего аллокатора и своего контейнера
	test_custom_container_with_custom_allocator();

	std::cout << std::endl;

	// Тестирование std аллокатора и своего контейнера
	test_custom_container_with_std_allocator();

    return 0;
}
