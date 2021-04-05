#include "cs/common/index.h"

namespace cs {

int rand() { return rand(0, 100); }

int rand(int min, int max) {
	static std::random_device r;
	std::uniform_int_distribution<> uniform_dist(min, max);
	return uniform_dist(r);
}

} // namespace cs