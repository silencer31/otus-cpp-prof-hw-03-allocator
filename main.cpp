#include "lib.h"
#include <iostream>

int main (int, char **) {
	// ����������� ������ ���������� � std::map
	test_std_map_with_custom_allocator();

	std::cout << std::endl;

	// ������������ ������ ���������� � ������ ����������
	test_custom_container_with_custom_allocator();

	std::cout << std::endl;

	// ������������ std ���������� � ������ ����������
	test_custom_container_with_std_allocator();

    return 0;
}
