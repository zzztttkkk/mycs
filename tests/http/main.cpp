//
// Created by ztk on 2022/3/29.
//

#include <mycs.hpp>

using namespace mycs::http;

int main() {
	Server server;
	server.listen(5623);
	return server.run();
}
