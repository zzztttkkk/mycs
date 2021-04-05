#include <cs.h>

using namespace cs;

int main() {
	Set<int> set;
	for (int i = 0; i < 10; ++i) {
		set.add(i);
	}
	for (int i = 0; i < 10; ++i) {
		set.add(i);
	}
	fprint("Size: {}\r\n", set.size());
	for (auto item : set) {
		fprint("{}\r\n", item);
	}

	const Set<int>& cSet = set;
	for (auto item : cSet) {
		fprint("{}\r\n", item);
	}
	return 0;
}