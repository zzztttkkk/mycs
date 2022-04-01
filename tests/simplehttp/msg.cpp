//
// Created by ztk on 2022/4/1.
//

#include <simplehttp.hpp>

namespace http = mycs::simplehttp;

int main() {
	std::string buf;
	std::string key = " A ";
	http::trimlowercopy(buf, key);
	std::cout << buf << std::endl;
	buf.clear();
	http::trimcopy(buf, key);
	std::cout << buf << std::endl;
	return 0;
}