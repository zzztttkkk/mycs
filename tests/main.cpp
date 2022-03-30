//
// Created by ztk on 2022/3/29.
//

#include <fmt/core.h>

#include <simplehttp.hpp>

using namespace mycs::simplehttp;

int main() {
	Server server;

	server.listen(5623);
	server.run();
	return 0;
}
