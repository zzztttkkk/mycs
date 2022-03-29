//
// Created by ztk on 2022/3/29.
//

#include <simplehttp.hpp>
#include <fmt/core.h>

using namespace mycs::simplehttp;

int main() {
	Headers headers;

	headers.append("a", "12我");
	headers.reset("a", "45");

	auto ptr = headers.all("a");

	if (ptr != nullptr) {
		for (auto val: *ptr) {
			fmt::print("{}\r\n", *val);
		}
	}
	return 0;
}
