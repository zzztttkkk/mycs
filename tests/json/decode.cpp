//
// Created by ztk on 2022/4/8.
//

#include <mycs/json/decoder.h>
#include <mycs/json/encoder.h>

int main() {
	mycs::json::Decoder decoder;
	decoder.feed("[123, \"hello\"]");
	if (decoder.result()) {
		mycs::json::Encoder encoder(std::cout);
		encoder.encode(decoder.result());
		std::cout << std::endl;
	}
	return 0;
}