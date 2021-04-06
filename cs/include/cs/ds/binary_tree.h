#pragma once

#include "../common/index.h"
#include "./stack.h"

namespace cs {

template <typename T, typename Less = std::less<T>, typename Equal = std::equal_to<T>>
class BinaryTree {
   protected:
	class Node;

	typedef std::function<void(Node*)> NodeVisitorFunc;
	typedef std::function<void(const Node*)> NodeConstVisitorFunc;
	typedef std::function<void(const T&)> VisitorFunc;

	class Node {
	   protected:
		friend class BinaryTree<T>;

		Node* left = nullptr;
		Node* right = nullptr;
		T val;

		virtual Node* newNode(const T& v) { return new Node(v); }

	   public:
		explicit Node(const T& v) : val(v) {}

		~Node() = default;

		void pre_order(const NodeVisitorFunc& func) {
			func(this);
			if (left != nullptr) left->pre_order(func);
			if (right != nullptr) right->pre_order(func);
		}

		void in_order(const NodeVisitorFunc& func) {
			if (left != nullptr) left->in_order(func);
			func(this);
			if (right != nullptr) right->in_order(func);
		}

		void post_order(const NodeVisitorFunc& func) {
			if (left != nullptr) left->post_order(func);
			if (right != nullptr) right->post_order(func);
			func(this);
		}

		void pre_order(const NodeConstVisitorFunc& func) const {
			func(this);
			if (left != nullptr) left->pre_order(func);
			if (right != nullptr) right->pre_order(func);
		}

		void range(const NodeConstVisitorFunc& func, const T& min, const T& max) const {
			Less less;

			if (less(min, val) && left != nullptr) left->range(func, min, max);

			Equal equal;
			if (equal(min, val) || equal(max, val) || (less(min, val) && less(val, max))) {
				func(this);
			}

			if (less(val, max) && right != nullptr) right->range(func, min, max);
		}

		void in_order(const NodeConstVisitorFunc& func) const {
			if (left != nullptr) left->in_order(func);
			func(this);
			if (right != nullptr) right->in_order(func);
		}

		void post_order(const NodeConstVisitorFunc& func) const {
			if (left != nullptr) left->post_order(func);
			if (right != nullptr) right->post_order(func);
			func(this);
		}

		Node* find(const T& v) {
			Equal equal;
			if (equal(val, v)) {
				return const_cast<Node*>(this);
			}
			Less less;
			if (less(val, v)) {
				if (right != nullptr) return right->find(v);
				return nullptr;
			}
			if (left != nullptr) return left->find(v);
			return nullptr;
		}

		Node* nearest_sub_node(Node** parent) {
			if (left == nullptr) {
				return nullptr;
			}
			Node* cursor = left;
			while (cursor != nullptr) {
				if (cursor->right != nullptr) {
					if (parent != nullptr) *parent = cursor;
					cursor = cursor->right;
					continue;
				}
				return cursor;
			}
			// can not reach this line, just for the warning.
			return nullptr;
		}
	};

	Node* root = nullptr;
	size_t _size = 0;

	virtual Node* newNode(const T& v) { return new Node(v); }

	virtual Node* do_append(Node* node, const T& v) {
		if (node == nullptr) {
			_size++;
			return newNode(v);
		}
		if (Equal{}(node->val, v)) return node;
		if (Less{}(node->val, v)) {
			node->right = do_append(node->right, v);
			return node;
		}
		node->left = do_append(node->left, v);
		return node;
	}

	virtual Node* do_remove(Node* node, const T& v) {
		if (node == nullptr) return nullptr;
		if (Equal{}(node->val, v)) {
			Node* nearestParent = nullptr;
			Node* nearest = node->nearest_sub_node(&nearestParent);
			if (nearest == nullptr) {
				if (node->right == nullptr) {
					// empty
					nearest = nullptr;
				} else {
					// just has right
					nearest = node->right;
				}
			} else {
				if (nearestParent == nullptr) {
					// has left, but left has not right
					nearest->right = node->right;
				} else {
					// has left and left has right
					if (nearestParent->left == nearest) {
						nearestParent->left = nearest->left;
					} else {
						nearest->right = nearest->left;
					}
					nearest->left = node->left;
					nearest->right = node->right;
				}
			}
			delete node;
			_size--;
			return nearest;
		}
		if (Less{}(node->val, v)) {
			node->right = do_remove(node->right, v);
			return node;
		}
		node->left = do_remove(node->left, v);
		return node;
	}

   public:
	BinaryTree() = default;

	virtual ~BinaryTree() {
		if (root != nullptr) {
			root->post_order([](Node* node) -> bool {
				delete node;
				return true;
			});
		}
	}

	virtual void append(const T& val) { root = do_append(root, val); }

	bool contains(const T& val) const {
		if (root != nullptr) return root->find(val) != nullptr;
		return false;
	}

	void remove(const T& val) { root = do_remove(root, val); }

	template <typename SeqContainer>
	void expand_to(SeqContainer& seq) const {
		if (root == nullptr) return;
		root->in_order([&seq](const Node* node) { seq.push_back(node->val); });
	}

	template <typename SeqContainer>
	void range(SeqContainer& seq, const T& left, const T& right) const {
		if (root == nullptr) return;
		root->range([&seq](const Node* node) { seq.push_back(node->val); }, left, right);
	}

	void each(const VisitorFunc& func) {
		if (empty()) return;

		Stack<Node*> stack;
		stack.reserve(_size / 2);

		Node* cursor = root;
		size_t count = 0;

		while (count != _size) {
			while (cursor != nullptr) {
				stack.push(cursor);
				cursor = cursor->left;
			}

			while (!stack.empty()) {
				cursor = stack.pop();
				func(cursor->val);
				count++;
				if (cursor->right != nullptr) {
					cursor = cursor->right;
					break;
				}
			}
		}
	}

	bool empty() { return _size == 0; }

	size_t size() { return _size; }

	const T& farleft() {
		if (empty()) throw std::runtime_error("cs.BinaryTree: empty tree");
		auto cursor = root;
		while (cursor != nullptr) {
			if (cursor->left == nullptr) {
				return cursor->val;
			}
			cursor = cursor->left;
		}
	}

	const T& farright() {
		if (empty()) throw std::runtime_error("cs.BinaryTree: empty tree");
		auto cursor = root;
		while (cursor != nullptr) {
			if (cursor->right == nullptr) {
				return cursor->val;
			}
			cursor = cursor->right;
		}
	}
};

}  // namespace cs