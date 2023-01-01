#include "lib.h"
#include <iostream>

int main (int, char **) {
	// Тетирование своего аллокатора и std::map
	test_std_map();

	std::cout << std::endl;

	// Тестирование своего аллокатора и своего контейнера
	test_custom_container();

    return 0;
}
