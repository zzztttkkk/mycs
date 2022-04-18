//
// Created by ztk on 2022/4/8.
//

#pragma once

#include <cstring>
#include <initializer_list>
#include <stdexcept>

namespace mycs::algs {

template <typename T>
class Drray {
   private:
	T* _ptr = nullptr;
	size_t _cap = 0;
	size_t _size = 0;

	static inline void copy(T* dist, T* src, size_t begin, int size) {
		if (src == nullptr || size < 1) return;
		memcpy(dist, src + begin, sizeof(T) * size);
	}

	inline void move(size_t ncap) {
		T* np = new T[ncap];
		Drray::copy(np, _ptr, 0, _size);
		delete[] (_ptr);
		_ptr = np;
		_cap = ncap;
	}

	size_t ncap(size_t incr) {
		size_t v = _cap > 0 ? _cap : 8;
		while (_size + incr > v) v = v << 1;
		return v;
	}

	void grow(size_t n) { move(ncap(n)); }

   public:
	Drray() = default;

	Drray(const std::initializer_list<T>& list) {
		grow(list.size());
		for (auto& item : list) this->push_back(item);
	}

	virtual ~Drray() { delete[] (_ptr); }

	[[nodiscard]] size_t size() const { return _size; }

	[[nodiscard]] size_t cap() const { return _cap; }

	[[nodiscard]] bool empty() const { return _size == 0; }

	T* data() { return _ptr; }

	const T* data() const { return _ptr; }

	void clear() { _size = 0; }

	inline void reserve(size_t ts) {
		if (ts < _cap) return;
		move(ts);
	};

	inline void shrink_to_fit() {
		if (empty()) return;
		move(_size);
	};

	void push_back(const T& ele) {
		grow(1);
		_ptr[_size] = ele;
		_size++;
	}

	template <class... Args>
	void emplace_back(Args&&... args) {
		grow(1);
		_ptr[_size] = T(std::forward<Args>(args)...);
		_size++;
	}

	void insert(size_t idx, size_t count, const T& ele) {
		if (idx >= _size) throw std::runtime_error("algs::Drray, index out of range");
		if (count == 0) return;

		size_t nc = ncap(count);
		T* np = new T[nc];
		Drray::copy(nc, _ptr, 0, idx);
		for (int i = 0; i < count; ++i) np[idx + i] = ele;
		Drray::copy(nc + idx + count, _ptr, idx + 1, static_cast<int>(_size - idx));

		delete[] (_ptr);
		_cap = nc;
		_size += count;
		_ptr = np;
	}
};

}  // namespace mycs::algs