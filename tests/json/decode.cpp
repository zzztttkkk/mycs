//
// Created by ztk on 2022/4/8.
//

#include <mycs.hpp>

int main() {
	mycs::json::Decoder decoder;
	decoder.feed(R"({"a":"""b":45})");
	if (decoder.result()) {
		mycs::json::Encoder encoder(std::cout);
		encoder.encode(decoder.result());
		std::cout << std::endl;
	}
	return 0;
}