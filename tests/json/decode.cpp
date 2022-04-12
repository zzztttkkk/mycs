//
// Created by ztk on 2022/4/8.
//

#include <mycs.hpp>

int main() {
	InitConsole();

	mycs::json::Decoder decoder;
	auto result = decoder.decode(R"([""])");
	if (result) {
		mycs::json::Encoder encoder(std::cout);
		encoder.encode(result);
		std::cout << std::endl;
	}
	return 0;
}