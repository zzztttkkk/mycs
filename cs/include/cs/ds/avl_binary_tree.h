#pragma once

#include "../common/index.h"
#include "./binary_tree.h"

namespace cs {

template <typename T, typename Less = std::less<T>, typename Equal = std::equal_to<T>>
class AVLBinaryTree : public BinaryTree<T, Less, Equal> {
   private:
	typedef BinaryTree<T, Less, Equal> BaseTree;
	typedef typename BaseTree::Node BaseNode;

	class Node : public BaseNode {
	   private:
		BaseNode* newNode(const T& v) { return new Node(v); }
#define L dynamic_cast<Node*>(this->left)
#define R dynamic_cast<Node*>(this->right)

		int height() {
			if (L == nullptr && R == nullptr) return 0;
			if (L == nullptr) return 1 + R->height();
			if (R == nullptr) return 1 + L->height();
			return std::max(L->height(), R->height());
		}

		int factor() {
			if (L == nullptr && R == nullptr) return 0;
			if (L == nullptr) return -R->height();
			if (R == nullptr) return L->height();
			return L->height() - R->height();
		}

		void append(const T& v) {
			BaseNode::append(v);
			switch (factor()) {
				case -2:
					break;
				case 2:
					break;
			}
		}

	   public:
		explicit Node(const T& v) : BaseNode(v) {}
	};

	Node* newNode(const T& v) { return new Node(v); }

   public:
#undef L
#undef R
};

}  // namespace cs