#pragma once

#include "./drray.h"

namespace cs {

template <typename T, uint8_t N = 2, size_t InitCap = 6, typename Cmp = std::less<T>>
class Heap : public Drray<T, InitCap> {
   private:
	static_assert(N > 1, "cs:Heap: N > 1");

	virtual void go_up() {
		size_t ind = this->_size - 1;
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

	virtual void go_down() {
		size_t ind = 0;
		Cmp cmp;

		while (ind < this->_size) {
			auto lowInd = ind * N + 1;
			if (lowInd >= this->_size) {
				return;
			}

			T* minVP = this->_ptr + lowInd;
			auto minIdx = lowInd;
			for (auto j = lowInd + 1; j < lowInd + N; j++) {
				if (j >= this->_size) {
					break;
				}
				if (cmp(this->_ptr[j], *minVP)) {
					minVP = this->_ptr + j;
					minIdx = j;
				}
			}

			T& cV = this->_ptr[ind];
			if (cmp(cV, *minVP)) {
				return;
			}
			std::swap(cV, *minVP);
			ind = minIdx;
		}
	}

   public:
	Heap() = default;

	virtual ~Heap() = default;

	void push_back(const T& val) {
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

	void make() {
		if (this->empty()) {
			return;
		}

		T* nPtr = new T[this->_cap];
		T* ptr = this->_ptr;
		this->_ptr = nPtr;
		this->_size = 0;
		for (size_t i = 0; i < this->_size - 1; ++i) {
			push(ptr[i]);
		}
		delete[] ptr;
	}
};

}  // namespace cs