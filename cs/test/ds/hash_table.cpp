#include <cs.h>

#include <map>

using namespace cs;
using namespace std;

int f1() {
	HashTable<int, int> map;

	for (int i = 0; i < 10000; ++i) {
		map.set(i, i * i);
	}

	auto opt = map.find(789);
	if (opt.has_value()) {
		FMT("{}\r\n", opt.value());
	}
	FMT("{}\r\n", map[4506]);
	return 0;
}

void f2() {
	HashTable<int, int> map(100);
	for (int i = 0; i < 10; ++i) {
		map.set(i, i * i);
	}
	FMT("{}\r\n", map[3]);
	map[3] = 456;
	FMT("{}\r\n", map[3]);
	for (auto item : map) {
		FMT("{} {}\r\n", item->key(), item->val());
	}

	const auto& cf = map;
	for (auto item : cf) {
		FMT("{} {}\r\n", item->key(), item->val());
	}
}

int main() {
	f1();
	f2();
	return 0;
}