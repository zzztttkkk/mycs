#include <cs.h>

#include <map>

using namespace cs;
using namespace std;

int f1() {
	Map<int, int> map;

	for (int i = 0; i < 10000; ++i) {
		map.set(i, i * i);
	}

	return 0;
}

void f2() {
	Map<int, int> map(100);
	for (int i = 0; i < 10; ++i) {
		map.set(i, i * i);
	}
	fprint("{}\r\n", map[3] + 10);
	for (auto item : map) {
		fprint("{} {}\r\n", item->key(), item->val());
	}
}

int main() {
	f2();
	return 0;
}