//
// Created by ztk on 2022/4/18.
//

#include <mycs.hpp>

using namespace mycs;

int main() {
	algs::Drray<int> drray({1, 2, 3});

	drray.reserve(200);
	drray.shrink_to_fit();

	Fmtp("{}\r\n", drray.size());
	return 0;
}