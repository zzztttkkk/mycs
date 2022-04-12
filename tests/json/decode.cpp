//
// Created by ztk on 2022/4/8.
//

#include <mycs.hpp>

int main() {
	InitConsole();

	mycs::json::Decoder decoder;
	auto result = decoder.decode(
		R"({"a": {"b": "c","d": {"e": "f","g": {"h": {"i": "j","k": "l","m": "n","o": "p","q": "r","s": {"t": "u","v": ["w", "x"]},"y": "z"}}}}})");
	if (result) {
		mycs::json::Encoder encoder(std::cout);
		encoder.encode(result);
		std::cout << std::endl;
	}
	return 0;
}