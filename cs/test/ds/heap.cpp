#include <cs.h>
#include <gtest/gtest.h>

#include <vector>

using namespace std;
using namespace cs;

TEST(NHeap, PushAndPop) {
	auto begin = now();
	for (int k = 0; k < 10000; ++k) {
		Heap<int, 4> heap;
		vector<int> data;
		for (int i = 0; i < 100; ++i) {
			int v = cs::rand();
			heap.push_back(v);
			data.push_back(v);
		}
		std::sort(data.begin(), data.end());
		int ind = 0;
		while (!heap.empty()) {
			int v = heap.pop_front();
			ASSERT_EQ(v, data[ind]);
			ind++;
		}
	}
	FMT("{}\r\n", (now() - begin).count());
}

TestMain