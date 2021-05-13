#pragma once

#include "../common/index.h"
#include "./binary_tree.h"

namespace cs {

template<typename T, typename Less = std::less<T>, typename Equal = std::equal_to<T>>
class AVLBinaryTree : public BinaryTree<T, Less, Equal> {
private:
	typedef BinaryTree<T, Less, Equal> BaseTree;
	typedef typename BaseTree::Node BaseNode;

	class Node : public BaseNode {
	private:
		BaseNode* newNode(const T& v) { return new Node(v); }

#define TBN(n) dynamic_cast<BaseNode*>(n)
#define TCN(n) dynamic_cast<Node*>(n)
#define L TCN(this->left)
#define R TCN(this->right)

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

	public:
		explicit Node(const T& v) : BaseNode(v) {}
	};

	Node* do_rotate(Node* node) {
		switch (node->factor()) {
			case -2:
				break;
			case 2:
				break;
			default:
				return node;
		}
	}

	BaseNode* newNode(const T& v) { return new Node(v); }

	BaseNode* do_append(BaseNode* _node, const T& v) override {
		BaseNode* root = BaseTree::do_append(_node, v);
		return do_rotate(TCN(root));
	}

public:

#undef TBN
#undef TCN
#undef L
#undef R
};

}  // namespace cs