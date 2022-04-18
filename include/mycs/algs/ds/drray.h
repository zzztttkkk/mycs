//
// Created by ztk on 2022/4/8.
//

#pragma once

#include <cstring>
#include <functional>
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
		Drray::copy(np, _ptr, 0, static_cast<int>(_size));
		delete[] (_ptr);
		_ptr = np;
		_cap = ncap;
	}

	size_t ncap(size_t incr) {
		size_t v = _cap > 0 ? _cap : 8;
		while (_size + incr > v) v = v << 1;
		return v;
	}

	void grow(size_t n) {
		auto nc = ncap(n);
		if (nc == _cap) return;
		move(nc);
	}

	template <bool IsConst = false>
	class IterBase {
	   private:
		friend class Drray<T>;

		Drray<T>* drray;
		size_t idx;

		IterBase(Drray<T>* dp, size_t _idx) {
			drray = dp;
			idx = _idx;
		}

	   public:
		IterBase& operator++() {
			this->idx++;
			return *this;
		}

		IterBase& operator--() {
			this->idx = this->idx > 0 ? this->idx - 1 : std::string::npos;
			return *this;
		}

		IterBase& operator+(int v) {
			this->idx += v;
			return *this;
		}

		IterBase& operator-(int v) {
			this->idx += v;
			return *this;
		}

		bool operator==(const IterBase& another) const { return this->idx == another.idx; }

		template <bool WasConst = IsConst, typename = std::enable_if<!WasConst>>
		T& operator*() {
			return *(drray->_ptr + idx);
		}

		template <bool WasConst = IsConst, typename = std::enable_if<WasConst>>
		const T& operator*() const {
			return *(drray->_ptr + idx);
		}
	};

   public:
	typedef IterBase<false> Iter;
	typedef IterBase<true> ConstIter;

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

	T& at(size_t idx) { return *(_ptr + idx); }
	T& operator[](size_t idx) { return at(idx); }
	const T& at(size_t idx) const { return *(_ptr + idx); }
	const T& operator[](size_t idx) const { return at(idx); }

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
		Drray::copy(np, _ptr, 0, static_cast<int>(idx));
		for (int i = 0; i < count; ++i) np[idx + i] = ele;
		Drray::copy(np + idx + count, _ptr, idx, static_cast<int>(_size - idx));

		delete[] (_ptr);
		_cap = nc;
		_size += count;
		_ptr = np;
	}

	void erase(size_t idx) {
		if (idx >= _size) throw std::runtime_error("algs::Drray, index out of range");
		_size = idx;
	}

	Iter begin() { return Iter(this, 0); }

	Iter end() { return Iter(this, _size); }

	ConstIter cbegin() { return ConstIter(this, 0); }

	ConstIter cend() { return ConstIter(this, _size); }

	Iter rbegin() { return Iter(this, _size - 1); }

	Iter rend() { return Iter(this, std::string::npos); }

	ConstIter crbegin() { return ConstIter(this, _size - 1); }

	ConstIter crend() { return ConstIter(this, std::string::npos); }

	template <typename RT>
	Drray<RT> map(const std::function<RT(const T&, size_t)>& fn) {
		Drray<RT> drray;
		drray.reserve(_size);
		for (ConstIter& item : *this) {
			drray.push_back(fn(*item, item.idx));
		}
		return std::move(drray);
	}
};

}  // namespace mycs::algs