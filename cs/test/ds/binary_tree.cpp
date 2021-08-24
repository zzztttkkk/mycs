#include <cs.h>
#include <gtest/gtest.h>

#include <vector>

using namespace cs;
using namespace std;

TEST(BinaryTree, PushAndRemove) {
	for (int k = 0; k < 10000; ++k) {
		BinaryTree<int> binaryTree;
		Drray<int> data;

		for (int i = 0; i < 10; ++i) {
			auto v = cs::rand();
			data.push_back(v);
			binaryTree.append(v);
		}

		Drray<int> drray;
		binaryTree.expand_to(drray);
		ASSERT_EQ(Drray<int>::is_sorted(drray), true);

		int v = data[cs::rand() % 10];
		binaryTree.remove(v);
		drray.clear();
		binaryTree.expand_to(drray);
		if (!Drray<int>::is_sorted(drray)) {
			Drray<int>::print_to(data, std::cout);
			cout << fmt::format("Remove: {}", v) << endl;
			FAIL();
		}
		SUCCEED();
	}
}

TEST(BinaryTree, Range) {
	const int min = 10;
	const int max = 20;

	for (int k = 0; k < 10000; ++k) {
		BinaryTree<int> binaryTree;
		Drray<int> data;
		set<int> rangeDataSet;

		for (int i = 0; i < 10; ++i) {
			auto v = cs::rand();
			data.push_back(v);
			binaryTree.append(v);
			if (v >= min && v <= max) {
				rangeDataSet.insert(v);
			}
		}
		vector<int> rangeData;
		rangeData.reserve(rangeDataSet.size());
		for (auto item : rangeDataSet) {
			rangeData.push_back(item);
		}
		std::sort(rangeData.begin(), rangeData.end());

		Drray<int> drray;
		binaryTree.range(drray, min, max);
		ASSERT_EQ(drray.size(), rangeData.size());
		for (int i = 0; i < rangeData.size(); ++i) {
			ASSERT_EQ(rangeData[i], drray[i]);
		}
	}
}

TEST(BinaryTree, InOrder) {
	BinaryTree<int> tree;
	for (int i = 0; i < 10; ++i) {
		tree.append(cs::rand());
	}

	tree.each([](int v) { FMT("{}\r\n", v); });
}



TestMain
