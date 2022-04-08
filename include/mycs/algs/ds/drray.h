//
// Created by ztk on 2022/4/8.
//

#pragma once

namespace mycs::algs {

template <typename T>
class Drray {
   private:
	T* _ptr = nullptr;
	size_t _cap = 0;
	size_t _size = 0;

   public:
	Drray() = default;
	virtual ~Drray() { delete[](_ptr); }
};

}  // namespace mycs::algs