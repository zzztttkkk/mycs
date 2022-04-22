//
// Created by ztk on 2022/4/22.
//

#pragma once

#include <functional>

namespace mycs::algs {

template <typename T>
class List {
   private:
	class Node {
	   private:
		friend class List<T>;
		T data;
		Node* prev = nullptr;
		Node* next = nullptr;

	   public:
		explicit Node(T& v) { this->data = v; }
	};

	Node* _head = nullptr;
	Node* _tail = nullptr;
	size_t _size = 0;

	void insert(Node* cursor, const T& v) {
		auto node = new Node(v);
		_size++;

		if (cursor == nullptr) {
			_head = node;
			_tail = node;
			return;
		}

		auto prev = cursor->prev;
		if (prev) {
			prev->next = node;
			node->prev = prev;
		}
		node->next = cursor;
		cursor->prev = node;
	}

	void append(Node* cursor, const T& v) {
		auto node = new Node(v);
		_size++;

		if (cursor == nullptr) {
			_head = node;
			_tail = node;
			return;
		}

		auto next = cursor->next;
		if (next) {
			next->prev = node;
			node->next = next;
		}
		node->prev = cursor;
		cursor->next = node;
	}

	void remove(Node* node) {
		_size--;
		auto prev = node->prev;
		auto next = node->next;

		if (prev) {
			prev->next = next;
		} else {
			_head = next;
		}

		if (next) {
			next->prev = prev;
		} else {
			_tail = prev;
		}
	}

   public:
	List() = default;

	virtual ~List() {
		Node* cursor = _tail;
		while (cursor != nullptr) {
			Node* next = cursor->next;
			delete (cursor);
			cursor = next;
		}
		_head = nullptr;
		_tail = nullptr;
		_size = 0;
	}

	[[nodiscard]] bool empty() const { return _head == nullptr; }

	[[nodiscard]] size_t size() const { return _size; }

	void push_back(const T& v) { append(_tail, v); }

	void push_front(const T& v) { insert(_head, v); }

#define MakeEmplace(name, mn)               \
	template <typename... Args>             \
	void name(Args&&... args) {             \
		mn(T(std::forward<Args>(args)...)); \
	}
	MakeEmplace(emplace_back, push_back);
	MakeEmplace(emplace_front, push_front);
#undef MakeEmplace


};

}  // namespace mycs::algs
