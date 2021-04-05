#include <cs.h>
#include <vector>
#include <iostream>

using namespace std;
using namespace cs;

int main() {
	Stack<int> stack;

	for (int i = 0; i < 10; ++i) {
		stack.push(i);
	}

	fprint("Top: {}\n", stack.top());

	while (!stack.empty()) {
		fprint("{}\n", stack.pop());
	}

	return 0;
}