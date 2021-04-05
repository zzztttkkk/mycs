#include <cs.h>

#include <vector>

using namespace std;
using namespace cs;

int main() {
	LinkedList<int> ll;

	for (int i = 0; i < 10; ++i) {
		ll.push_front(i);
	}
	for (auto item : ll) {
		fprint("{}\r\n", item);
	}
	return 0;
}
