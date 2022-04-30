//
// Created by ztk on 2022/4/17.
//

#include <fstream>
#include <iostream>
#include <mycs.hpp>

using namespace mycs;

int main() {
	json::Decoder decoder;
	std::fstream f;
	f.open("../../../JSONTestSuite/test_parsing/a.json", std::ios::in);
	if (!f) return f.exceptions();

	auto result = decoder.decode(f);
	if (result != nullptr) {
		std::fstream of;
		of.open("../../../JSONTestSuite/test_parsing/b.json", std::ios::out);
		json::Encoder encoder(of);
		encoder.encode(result);
		of.close();
		if (of.exceptions() != 0) {
			return 1;
		}
	} else {
		std::cout << decoder.error()->what() << std::endl;
	}
	return 0;
}