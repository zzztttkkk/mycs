//
// Created by ztk on 2022/4/4.
//

#include <json/value.h>

#include <iostream>

using namespace mycs::json;

int main() {
	NumberValue v(12.96);
	std::cout << v.integer() << std::endl;
	std::cout << v.is_none() << std::endl;

	if (False) {
		std::cout << "AA" << std::endl;
	}

	return 0;
}