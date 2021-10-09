#include <cs.h>
#include <gtest/gtest.h>

TEST(Bitmap, Op) {
	auto bitmap = cs::Bitmap(1024);
	bitmap.add(0);
	ASSERT_EQ(bitmap.has(0), true);
	bitmap.del(0);
	ASSERT_EQ(bitmap.has(0), false);
}

TestMain