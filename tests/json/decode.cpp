//
// Created by ztk on 2022/4/17.
//

#include <iostream>
#include <mycs.hpp>

using namespace mycs;

int main() {
	json::Decoder decoder;

	auto result = decoder.decode("\"\n\"");
	if (result != nullptr) {
		json::Encoder encoder(std::cout);
		encoder.encode(result);
		std::cout << std::endl;
	} else {
		std::cout << decoder.error()->what() << std::endl;
	}
	return 0;
}