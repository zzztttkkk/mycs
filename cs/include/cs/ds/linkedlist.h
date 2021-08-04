#pragma once

#include "../common/index.h"

namespace cs {

template <typename T>
class LinkedList {
   private:
	class Node {
	   public:
		Node* prev = nullptr;
		Node* next = nullptr;
		T val;

		explicit Node(const T& v) : val(v) {}
	};

	Node* _head = nullptr;
	Node* _tail = nullptr;
	size_t _size = 0;

	template <bool IsConst>
	class BaseIterator {
	   private:
		Node* ptr = nullptr;

	   public:
		BaseIterator() = default;
		explicit BaseIterator(Node* p) : ptr(p){};

		virtual ~BaseIterator() = default;

		BaseIterator& operator++() {
			ptr = ptr->next;
			return *this;
		}

		BaseIterator& operator--() {
			ptr = ptr->prev;
			return *this;
		}

		bool operator!=(const BaseIterator& other) const { return ptr != other.ptr; }

		template <bool WasConst = IsConst, typename = typename std::enable_if<WasConst, void>::type>
		const T& operator*() {
			return ptr->val;
		}

		template <bool WasConst = IsConst, typename = typename std::enable_if<WasConst, void>::type>
		const T* operator->() {
			return &(ptr->val);
		}

		template <bool WasConst = IsConst, typename = typename std::enable_if<!WasConst, void>::type>
		T& operator*() {
			return ptr->val;
		}

		template <bool WasConst = IsConst, typename = typename std::enable_if<!WasConst, void>::type>
		T* operator->() {
			return &(ptr->val);
		}
	};

   public:
	typedef BaseIterator<true> ConstIterator;
	typedef BaseIterator<false> Iterator;

	LinkedList() = default;

	virtual ~LinkedList() {
		Node* cursor = _head;
		Node* next = nullptr;
		while (cursor != nullptr) {
			next = cursor->next;
			delete cursor;
			cursor = next;
		}
		delete next;
	}

	void push_back(const T& v) {
		_size++;

		if (_head == nullptr) {
			_head = _tail = new Node(v);
			return;
		}
		_tail->next = new Node(v);
		_tail->next->prev = _tail;
		_tail = _tail->next;
	}

	void push_front(const T& v) {
		_size++;

		if (_head == nullptr) {
			_head = _tail = new Node(v);
			return;
		}
		_head->prev = new Node(v);
		_head->prev->next = _head;
		_head = _head->prev;
	}

#define CsLLCheckEmpty \
	if (_head == nullptr) throw std::runtime_error("cs.LinkedList: empty")
#define CsLLCheckIndex \
	if (index >= _size) throw std::runtime_error("cs.LinkedList: out of range")

   private:
	Node* get_node(size_t index) {
		CsLLCheckIndex;

		if (index < _size / 2) {
			auto cursor = _head;
			for (size_t i = 0; i <= index; ++i) {
				cursor = cursor->next;
			}
			return cursor;
		}

		auto cursor = _tail;
		for (size_t i = _size - index - 1; i >= 0; i--) {
			cursor = cursor->prev;
		}
		return cursor;
	}

	void remove_node(Node* node) {
		_size--;
		Node* prev = node->prev;
		if (prev != nullptr) {
			prev->next = node->next;
		}
		Node* next = node->next;
		if (next != nullptr) {
			next->prev = node->prev;
		}
		if (node == _head) {
			_head = node->next;
		}
		if (node == _tail) {
			_tail = node->prev;
		}
	}

   public:
	T pop_back() { return erase(_size - 1); }

	T pop_front() { return erase(0); }

	T& at(size_t index) { return get_node(index)->val; }

	T& operator[](size_t index) { return at(index); }

	T& erase(size_t index) {
		Node* node = get_node(index);
		auto val = node->val;
		remove_node(node);
		return val;
	}

	typedef std::function<bool(size_t idx, const T&)> RemoveIfFunc;

	int remove_if(const RemoveIfFunc& func, int count) {
		auto cursor = _head;
		int v = 0;
		size_t idx = 0;
		while (cursor != nullptr && count != 0) {
			if (func(idx, cursor->val)) {
				auto ptr = cursor;
				cursor = cursor->next;
				remove_node(ptr);
				count--;
				v++;
				continue;
			}
			cursor = cursor->next;
			idx++;
		}
		return v;
	}

	void insert(size_t index, const T& val) {
		if (index == _size) {
			push_back(val);
			return;
		}
		if (index == 0) {
			push_front(val);
			return;
		}

		Node* node = get_node(index);
		Node* prev = node->prev;

		Node* temp = new Node(val);

		prev->next = temp;
		temp->next = node;

		node->prev = temp;
		temp->prev = prev;
	}

#undef CsLLCheckEmpty
#undef CsLLCheckIndex

	Iterator begin() { return Iterator(this->_head); }

	Iterator end() { return Iterator(nullptr); }

	Iterator rbegin() { return Iterator(this->_tail); }

	Iterator rend() { return Iterator(nullptr); }

	ConstIterator begin() const { return ConstIterator(this->_head); }

	ConstIterator end() const { return ConstIterator(nullptr); }

	ConstIterator rbegin() const { return ConstIterator(this->_tail); }

	ConstIterator rend() const { return ConstIterator(nullptr); }
};

}  // namespace cs