#include <cs.h>
#include <gtest/gtest.h>

TEST(Common, rand) { FMT("{} {}\r\n", cs::rand(), cs::rand(0, 9999)); }

TEST(Common, now) { FMT("{}\n", cs::now().time_since_epoch().count()); }

TestMain;