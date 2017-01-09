#include "bind.h"
#include <iostream>

int inc1(int a) {
	return a++;
}

void f() {
	int a = 11;
	std::cout << bind(inc1, _1)(a) << std::endl;
}
