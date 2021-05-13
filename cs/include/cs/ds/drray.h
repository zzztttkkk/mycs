#pragma once

#include "../common/index.h"

namespace cs {

template<typename T, size_t InitCap = 6>
class Drray {
	static_assert(InitCap > 0, "cs.ds.Drray: Zero InitCap");

private:
	template<bool IsConst>
	class BaseIterator {
	private:
		T* ptr;

	public:
		explicit BaseIterator(T* p) : ptr(p) {};

		virtual ~BaseIterator() = default;

		BaseIterator& operator++() {
			ptr++;
			return *this;
		}

		BaseIterator& operator--() {
			ptr--;
			return *this;
		}

		bool operator!=(const BaseIterator& other) const { return ptr != other.ptr; }

		template<bool WasConst = IsConst, typename = typename std::enable_if<WasConst, void>::type>
		const T& operator*() {
			return *ptr;
		}

		template<bool WasConst = IsConst, typename = typename std::enable_if<WasConst, void>::type>
		const T* operator->() {
			return ptr;
		}

		template<bool WasConst = IsConst, typename = typename std::enable_if<!WasConst, void>::type>
		T& operator*() {
			return *ptr;
		}

		template<bool WasConst = IsConst, typename = typename std::enable_if<!WasConst, void>::type>
		T* operator->() {
			return ptr;
		}
	};

public:
	typedef std::function<bool(size_t, T&)> VisitorFunc;
	typedef std::function<bool(size_t, const T&)> ConstVisitorFunc;
	typedef BaseIterator<true> ConstIterator;
	typedef BaseIterator<false> Iterator;

protected:
	T* _ptr = nullptr;
	size_t _size = 0;
	size_t _cap = 0;

	void ncopy(size_t n) {
		auto nPtr = new T[n];
		memcpy(nPtr, _ptr, sizeof(T) * (std::min(_size, n)));
		delete[] _ptr;
		_ptr = nPtr;
		_cap = n;
	}

	void allocate(size_t t) {
		if (t <= _cap) {
			return;
		}

		if (_cap == 0) {
			ncopy(t);
			return;
		}

		auto nCap = _cap << 1;
		while (nCap < t) {
			nCap = nCap << 1;
		}
		ncopy(nCap);
	}

	void auto_allocate() {
		if (_ptr == nullptr) {
			_ptr = new T[InitCap];
			_cap = InitCap;
			return;
		}
		allocate(_size + 1);
	}

public:
	Drray() = default;

	virtual ~Drray() {
		if (_ptr == nullptr) {
			return;
		}
		delete[] _ptr;
	}

	[[nodiscard]] size_t size() const { return _size; }

	void resize(size_t ns) {
		if (ns <= _size) {
			_size = ns;
			return;
		}
		allocate(ns);
		_size = ns;
	}

	void reserve(size_t nCap) {
		if (nCap == _cap) {
			return;
		}
		ncopy(nCap);
		_size = std::min(nCap, _size);
	}

	[[nodiscard]] size_t cap() const { return _cap; }

	T* data() const { return _ptr; }

	[[nodiscard]] bool empty() const { return _size == 0; }

	Iterator begin() { return Iterator(this->_ptr); }

	Iterator end() { return Iterator(this->_ptr + this->_size); }

	ConstIterator begin() const { return ConstIterator(this->_ptr); }

	ConstIterator end() const { return ConstIterator(this->_ptr + this->_size); }

	Iterator rbegin() { return Iterator(this->_ptr + (_size - 1)); }

	Iterator rend() { return Iterator(this->_ptr - 1); }

	ConstIterator rbegin() const { return ConstIterator(this->_ptr + (_size - 1)); }

	ConstIterator rend() const { return ConstIterator(this->_ptr - 1); }

	void each(const VisitorFunc& func) {
		for (size_t i = 0; i < _size; ++i) {
			if (!func(i, _ptr[i])) break;
		}
	}

	void reach(const VisitorFunc& func) {
		if (empty()) return;
		for (size_t i = _size - 1; i >= 0; --i) {
			if (!func(i, _ptr[i])) break;
		}
	}

	void each(const ConstVisitorFunc& func) const {
		for (size_t i = 0; i < _size; ++i) {
			if (!func(i, _ptr[i])) break;
		}
	}

	void reach(const ConstVisitorFunc& func) const {
		if (empty()) return;
		for (size_t i = _size - 1; i >= 0; --i) {
			if (!func(i, _ptr[i])) break;
		}
	}

	void clear() { _size = 0; }

	virtual void push_back(const T& val) {
		auto_allocate();
		_ptr[_size++] = val;
	}

	template<typename SeqContainer>
	void copy_to(SeqContainer& seq) const {
		each([&seq](size_t i, const T& v) -> bool {
			seq.push_back(v);
			return true;
		});
	}

	template<typename SeqContainer>
	void copy_from(const SeqContainer& seq) {
		for (auto item : seq) push_back(item);
	}

#define CsDrrayCheckIndex \
    if (index >= _size) throw std::runtime_error("cs.Drray: out of range")

	T& at(size_t index) {
		CsDrrayCheckIndex;
		return _ptr[index];
	}

	const T& at(size_t index) const {
		CsDrrayCheckIndex;
		return _ptr[index];
	}

	T& operator[](size_t index) { return at(index); }

	const T& operator[](size_t index) const { return at(index); }

	T& front() { return at(0); }

	const T& front() const { return at(0); }

	T& back() { return at(_size - 1); }

	const T& back() const { return at(_size - 1); }

	virtual T pop_back() { return erase(_size - 1); }

	T erase(size_t index) {
		CsDrrayCheckIndex;
		T v = at(index);
		if (index == _size - 1) {
			_size--;
			return v;
		}

		auto nPtr = new T[_cap];
		T* temp = nPtr;

		if (index > 0) {
			memcpy(nPtr, _ptr, sizeof(T) * index);
			temp = nPtr + index;
		}
		if (index < _size - 1) {
			memcpy(temp, _ptr + index + 1, sizeof(T) * (_size - index - 1));
		}
		delete[] _ptr;
		_ptr = nPtr;
		_size--;
		return v;
	}

	void insert(size_t index, const T& val) {
		if (index == _size) {
			push_back(val);
			return;
		}

		CsDrrayCheckIndex;

		if (_size + 1 > _cap) {
			auto nCap = _cap << 1;
			auto nPtr = new T[nCap];
			memcpy(nPtr, _ptr, sizeof(T) * index);
			memcpy(nPtr + index + 1, _ptr + index, sizeof(T) * (_size - index));
			delete[] _ptr;
			_cap = nCap;
			_ptr = nPtr;
		} else {
			for (size_t j = _size; j >= index; j--) _ptr[j] = _ptr[j - 1];
		}
		_size++;
		_ptr[index] = val;
	}

	void fill(const T& val) { fill(val, 0, -1); }

	void fill(const T& val, size_t index, int count) {
		if (_size == 0) return;
		for (size_t i = index; i < _size; ++i) {
			if (count == 0) return;
			_ptr[i] = val;
			count--;
		}
	}

	void fill_zero() { fill_zero(0, -1); }

	void fill_zero(size_t index, int count) {
		if (_size == 0) return;
		if (count < 1) {
			count = static_cast<int>(_size - index);
		}
		memset(_ptr + index, 0, sizeof(T) * count);
	}

#undef CsDrrayCheckIndex

	template<typename Cmp = std::less<T>>
	static bool is_sorted(const Drray<T, InitCap>& drray) {
		if (drray.empty()) return true;
		T* prev;
		Cmp cmp;
		for (size_t i = 0; i < drray._size; ++i) {
			if (i == 0) {
				prev = drray._ptr;
				continue;
			}
			if (cmp(*prev, drray._ptr[i])) {
				prev = drray._ptr + i;
				continue;
			}
			return false;
		}
		return true;
	}

	static void print_to(const Drray<T, InitCap>& drray, std::ostream& output) {
		output << fmt::format("Drray<{}, size: {}>: ", (void*) (const_cast<Drray<T, InitCap>*>(&drray)), drray.size());
		for (auto item : drray) {
			output << fmt::format("{}, ", item);
		}
		output << std::endl;
	}
};

}  // namespace cs