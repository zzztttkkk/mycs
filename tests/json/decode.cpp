//
// Created by ztk on 2022/4/8.
//

#include <filesystem>
#include <fstream>
#include <mycs.hpp>

namespace fs = std::filesystem;

void decode_one(mycs::json::Decoder& decoder, std::ifstream& f) {
	decoder.clear();

	auto result = decoder.decode(f);
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
	f.close();
}

int main() {
	InitConsole();

	mycs::json::Decoder decoder;
	fs::path path("../../../JSONTestSuite/test_parsing/");
	fs::directory_iterator end;

	for (auto iter = fs::directory_iterator(path); iter != end; iter++) {
		if (iter->path().filename().c_str()[0] != 'n') continue;
		std::wcout << "File:" << iter->path() << std::endl;

		std::ifstream f;
		f.open(iter->path(), std::ios::in);
		if (!f.is_open()) continue;

		decode_one(decoder, f);
	}
	return 0;
}