#pragma once

#include "./drray.h"

namespace cs {

class Bitmap {
#define BitmapCheckMax                                   \
	if (idx > max) throw std::exception("out of range"); \
	size_t x = idx / 64;                                 \
	size_t y = idx % 64

   private:
	Drray<uint64_t> data;
	size_t max;

   public:
	explicit Bitmap(size_t max) {
		auto size = max / 64;
		data.reserve(size);
		data.resize(size);
		data.fill_zero();
		this->max = max;
	};
	virtual ~Bitmap() = default;

	void add(size_t idx) {
		BitmapCheckMax;
		data[x] |= (size_t(1) << y);
	}

	void del(size_t idx) {
		BitmapCheckMax;
		data[x] ^= (size_t(1) << y);
	}

	bool has(size_t idx) {
		BitmapCheckMax;
		return ((data[x] >> y) & 1) != 0;
	}

#undef BitmapCheckMax
};

}  // namespace cs