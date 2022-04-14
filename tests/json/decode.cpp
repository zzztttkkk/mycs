//
// Created by ztk on 2022/4/8.
//

#include <fstream>
#include <mycs.hpp>

int main() {
	InitConsole();

	mycs::json::Decoder decoder;
	std::ifstream f;
	f.open("../../../JSONTestSuite/test_parsing/y_array_empty.json", std::ios::in);
	if (!f.is_open()) return 1;
	mycs::Defer _([&f]() { f.close(); });

	auto result = decoder.decode(R"({
"$schema": "https://json-schema.org/draft/2020-12/schema",
"$id": "https://example.com/product.schema.json",
"title": "Product",
"description": "A product in the catalog",
"type": "object"
})");
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