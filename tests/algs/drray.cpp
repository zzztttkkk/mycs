//
// Created by ztk on 2022/4/18.
//

#include <mycs.hpp>

using namespace mycs;

int main() {
	algs::Drray<int> drray({1, 2, 3});

	drray.push_back(12);
	drray.push_back(15);
	drray.emplace(1, 344);

	for (auto& item : drray) {
		Fmtp("{},", item);
	}
	Fmtp("\r\n{} {}\r\n", drray.front(), drray.back());
	return 0;
}