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
	headers.append("a", "rtg");

	headers.each([](const auto& a, std::string& b) {
		fmt::print("{} {}\n", a, b);
		b += "qwe";
		return true;
	});

	fmt::print("{}\r\n", mycs::simplehttp::httpversion2string(mycs::simplehttp::HttpVersion::H11));
	return 0;
}
