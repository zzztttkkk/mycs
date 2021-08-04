#pragma once

#include "./drray.h"

namespace cs {

class Bitmap {
   private:
	Drray<uint64_t> data;

   public:
	Bitmap() = default;
	virtual ~Bitmap() = default;

	void add(size_t idx) {}
};

}  // namespace cs