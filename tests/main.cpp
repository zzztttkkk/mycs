//
// Created by ztk on 2022/3/29.
//

#include <simplehttp.hpp>

using namespace mycs::simplehttp;

int main() {
	Server server;
	server.listen(5623);
	return server.run();
}
