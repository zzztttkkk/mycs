#pragma once

#include "./drray.h"

namespace cs {

template <typename T, uint8_t N = 2, size_t InitCap = 6, typename Cmp = std::less<T>>
class Heap : public Drray<T, InitCap> {
   private:
	static_assert(N > 1, "cs:Heap: N > 1");
	const static size_t de = static_cast<size_t>(-1);

	void go_up(size_t start = de) {
		if (start == de) start = this->_size - 1;
		size_t ind = start;
		Cmp cmp;

		while (ind != 0) {
			auto pInd = (ind - 1) / N;
			T& a = this->_ptr[ind];
			T& b = this->_ptr[pInd];
			if (cmp(a, b)) {
				std::swap(a, b);
				ind = pInd;
				continue;
			}
			return;
		}
	}

	void go_down(size_t end = de) {
		size_t ind = 0;
		Cmp cmp;
		if (end == de) end = this->_size;

		while (ind < end) {
			auto lowInd = ind * N + 1;
			if (lowInd >= end) return;

			T* minVP = this->_ptr + lowInd;
			auto minIdx = lowInd;
			for (auto j = lowInd + 1; j < lowInd + N; j++) {
				if (j >= end) break;
				if (cmp(this->_ptr[j], *minVP)) {
					minVP = this->_ptr + j;
					minIdx = j;
				}
			}

			T& cV = this->_ptr[ind];
			if (cmp(cV, *minVP)) return;
			std::swap(cV, *minVP);
			ind = minIdx;
		}
	}

   public:
	Heap() = default;

	virtual ~Heap() = default;

	void push_back(const T& val) override {
		Drray<T, InitCap>::push_back(val);
		go_up();
	}

	T pop_front() {
		auto v = this->_ptr[0];
		this->_size--;
		this->_ptr[0] = this->_ptr[this->_size];

		go_down();
		return v;
	}
};

}  // namespace cs