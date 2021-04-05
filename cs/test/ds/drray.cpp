#include <cs.h>
#include <gtest/gtest.h>

using namespace cs;

TEST(DSDrray, PushAndAt) {
	Drray<int> drray;
	for (int i = 0; i < 10; ++i) {
		drray.push_back(i);
	}
	ASSERT_EQ(drray.size(), 10);
	for (int i = 0; i < 10; ++i) {
		ASSERT_EQ(drray.at(i), i);
	}
};

TestMain