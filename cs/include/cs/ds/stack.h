#pragma once

#include "../common/index.h"
#include "./drray.h"

namespace cs {

template <typename T, size_t InitCap = 6>
class Stack : public Drray<T, InitCap> {
   public:
	void push(const T& v) { this->push_back(v); }

	T pop() { return this->pop_back(); }

	T& top() { return this->front(); }
};

}  // namespace cs