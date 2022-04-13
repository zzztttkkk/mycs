//
// Created by ztk on 2022/4/8.
//

#include <mycs.hpp>

int main() {
	InitConsole();

	mycs::json::Decoder decoder;
	// https://github.com/nst/JSONTestSuite/blob/master/test_parsing/n_array_inner_array_no_comma.json
	auto result = decoder.decode(R"([3[4]])");
	if (result) {
		mycs::json::Encoder encoder(std::cout);
		encoder.encode(result);
		std::cout << std::endl;
	} else {
		auto err = decoder.error();
		if (err.has_value()) {
			std::cout << err->what() << std::endl;
		}
	}
	return 0;
}