#include <cs.h>

using namespace cs;

int main() {
	HashSet<int> set;
	for (int i = 0; i < 10; ++i) {
		set.add(i);
	}
	for (int i = 0; i < 10; ++i) {
		set.add(i);
	}
	FMT("Size: {}\r\n", set.size());

	const auto& cSet = set;
	for (auto item : cSet) {
		FMT("{}\r\n", item);
	}
	return 0;
}