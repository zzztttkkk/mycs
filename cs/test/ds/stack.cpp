#include <cs.h>

using namespace std;
using namespace cs;

int main() {
	Stack<int> stack;

	for (int i = 0; i < 10; ++i) {
		stack.push(i);
	}

	FMT("Top: {}\n", stack.top());

	while (!stack.empty()) {
		FMT("{}\n", stack.pop());
	}

	return 0;
}