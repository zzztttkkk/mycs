#pragma once

#include "../common/index.h"

namespace cs {

template <typename T, typename Less = std::less<T>, typename Equal = std::equal_to<T>>
class BinaryTree {
   private:
	class Node;

	typedef std::function<void(Node*)> VisitorFunc;
	typedef std::function<void(const Node*)> ConstVisitorFunc;
	typedef std::pair<Node*, Node*> FindResult;

	class Node {
	   private:
		friend class BinaryTree<T>;

		Node* lChild = nullptr;
		Node* rChild = nullptr;
		T val;

	   public:
		explicit Node(const T& v) : val(v) {}

		~Node() = default;

		void pre_order(const VisitorFunc& func) {
			func(this);
			if (lChild != nullptr) lChild->pre_order(func);
			if (rChild != nullptr) rChild->pre_order(func);
		}

		void in_order(const VisitorFunc& func) {
			if (lChild != nullptr) lChild->in_order(func);
			func(this);
			if (rChild != nullptr) rChild->in_order(func);
		}

		void post_order(const VisitorFunc& func) {
			if (lChild != nullptr) lChild->post_order(func);
			if (rChild != nullptr) rChild->post_order(func);
			func(this);
		}

		void pre_order(const ConstVisitorFunc& func) const {
			func(this);
			if (lChild != nullptr) lChild->pre_order(func);
			if (rChild != nullptr) rChild->pre_order(func);
		}

		void range(const ConstVisitorFunc& func, const T& min, const T& max) const {
			Less less;

			if (less(min, val) && lChild != nullptr) lChild->range(func, min, max);

			Equal equal;
			if (equal(min, val) || equal(max, val) || (less(min, val) && less(val, max))) {
				func(this);
			}

			if (less(val, max) && rChild != nullptr) rChild->range(func, min, max);
		}

		void in_order(const ConstVisitorFunc& func) const {
			if (lChild != nullptr) lChild->in_order(func);
			func(this);
			if (rChild != nullptr) rChild->in_order(func);
		}

		void post_order(const ConstVisitorFunc& func) const {
			if (lChild != nullptr) lChild->post_order(func);
			if (rChild != nullptr) rChild->post_order(func);
			func(this);
		}

		Node* append(const T& v) {
			Equal equal;
			if (equal(val, v)) {
				return nullptr;
			}
			Less less;
			if (less(val, v)) {
				if (rChild != nullptr) return rChild->append(v);
				rChild = new Node(v);
				return rChild;
			}
			if (lChild != nullptr) return lChild->append(v);
			lChild = new Node(v);
			return lChild;
		}

		void find(const T& v, FindResult& findResult) const {
			Equal equal;
			if (equal(val, v)) {
				findResult.second = const_cast<Node*>(this);
				return;
			}
			Less less;
			if (less(val, v)) {
				if (rChild != nullptr) {
					findResult.first = const_cast<Node*>(this);
					return rChild->find(v, findResult);
				}
				return;
			}
			if (lChild != nullptr) {
				findResult.first = const_cast<Node*>(this);
				return lChild->find(v, findResult);
			}
		}

		Node* nearest_sub_node(Node** parent) {
			if (lChild == nullptr) {
				return nullptr;
			}
			Node* cursor = lChild;
			while (cursor != nullptr) {
				if (cursor->rChild != nullptr) {
					if (parent != nullptr) *parent = cursor;
					cursor = cursor->rChild;
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

	void append(const T& val) {
		_size++;
		if (root == nullptr) {
			root = new Node(val);
			return;
		}
		root->append(val);
	}

	bool contains(const T& val) const {
		if (root != nullptr) {
			FindResult findResult;
			root->find(findResult);
			return findResult.second != nullptr;
		}
		return false;
	}

	bool remove(const T& val) {
		if (root == nullptr) return false;
		FindResult findResult;
		root->find(val, findResult);
		Node* targetParent = findResult.first;
		Node* target = findResult.second;
		if (target == nullptr) return false;

		Node* nearestNodeParent = nullptr;
		Node* nearestNode = const_cast<Node*>(target)->nearest_sub_node(&nearestNodeParent);
		if (nearestNode == nullptr) {
			if (target->rChild == nullptr) {
				// has no children
			} else {
				// just has rChild
				nearestNode = target->rChild;
			}
		} else {
			if (nearestNodeParent == nullptr) {
				// has lChild, but lChild has not rChild
				nearestNode->rChild = target->rChild;
			} else {
				// has lChild and lChild has rChild
				if (nearestNodeParent->rChild == nearestNode) {
					nearestNodeParent->rChild = nearestNode->lChild;
				} else {
					nearestNodeParent->lChild = nearestNode->lChild;
				}
				nearestNode->lChild = target->lChild;
				nearestNode->rChild = target->rChild;
			}
		}

		if (targetParent == nullptr) {
			root = nearestNode;
		} else {
			if (targetParent->rChild == target) {
				targetParent->rChild = nearestNode;
			} else {
				targetParent->lChild = nearestNode;
			}
		}

		_size--;
		delete target;
		return true;
	}

	template <typename SeqContainer>
	void expand_to(SeqContainer& seq) const {
		if (root == nullptr) return;
		root->in_order([&seq](const Node* node) { seq.push_back(node->val); });
	}

	template <typename SeqContainer>
	void range(SeqContainer& seq, const T& min, const T& max) const {
		if (root == nullptr) return;
		root->range([&seq](const Node* node) { seq.push_back(node->val); }, min, max);
	}
};

}  // namespace cs