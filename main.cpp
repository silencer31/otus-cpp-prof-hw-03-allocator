#include "lib.h"
#include <iostream>

int main (int, char **) {
	// ����������� ������ ���������� � std::map
	test_std_map();

	std::cout << std::endl;

	// ������������ ������ ���������� � ������ ����������
	test_custom_container();

    return 0;
}
