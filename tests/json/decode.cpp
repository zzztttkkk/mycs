//
// Created by ztk on 2022/4/8.
//

#include <mycs/json/decoder.h>

int main() {
	mycs::json::Decoder decoder;
	decoder.feed("{\"a\": 12}");
	return 0;
}