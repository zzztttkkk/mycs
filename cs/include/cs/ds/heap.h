#pragma once

#include "./drray.h"

namespace cs {

template <typename T, size_t InitCap = 6, typename Cmp = std::less<T>>
class Heap : public Drray<T, InitCap> {
   private:
	void go_up() {
		size_t ind = this->_size - 1;
		Cmp cmp;

		while (ind != 0) {
			auto pInd = (ind - 1) / 2;
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

	void go_down() {
		size_t ind = 0;
		Cmp cmp;

		while (ind < this->_size) {
			auto lInd = ind * 2 + 1;
			if (lInd >= this->_size) {
				return;
			}
			T& lV = this->_ptr[lInd];

			size_t mInd = lInd;
			T* mVP = &lV;

			auto rInd = ind * 2 + 2;
			if (rInd < this->_size) {
				T& rV = this->_ptr[rInd];
				if (!cmp(lV, rV)) {
					mInd = rInd;
					mVP = &rV;
				}
			}
			T& cV = this->_ptr[ind];
			if (cmp(*mVP, cV)) {
				std::swap(*mVP, cV);
				ind = mInd;
				continue;
			}
			return;
		}
	}

   public:
	Heap() {}

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