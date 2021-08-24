#include <cs.h>
#include <gtest/gtest.h>

#include <vector>

using namespace cs;
using namespace std;

TEST(DSDrray, PushAndAt) {
	Drray<int> drray;
	for (int i = 0; i < 10; ++i) {
		drray.push_back(i);
	}
	ASSERT_EQ(drray.size(), 10);
	for (int i = 0; i < 10; ++i) {
		ASSERT_EQ(drray.at(i), i);
	}

	vector<int> vet;
	drray.copy_to(vet);
	drray.copy_from(vet);
	FMT("Size: {}; Cap: {}", drray.size(), drray.cap());
}

TestMain;