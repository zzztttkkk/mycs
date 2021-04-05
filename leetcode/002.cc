// https://leetcode-cn.com/problems/add-two-numbers/

#include "common.h"


class Solution {
public:
	ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
		ListNode* ret = nullptr;
		ListNode* cursor = nullptr;

		bool overflow = false;
		while (l1 != nullptr || l2 != nullptr) {
			int n1 = 0;
			int n2 = 0;

			if (l1 != nullptr) {
				n1 = l1->val;
				l1 = l1->next;
			}
			if (l2 != nullptr) {
				n2 = l2->val;
				l2 = l2->next;
			}

			int v = n1 + n2 + overflow;
			if (overflow) {
				overflow = false;
			}
			if (v > 9) {
				overflow = true;
				v -= 10;
			}

			if (cursor == nullptr) {
				ret = new ListNode(v, nullptr);
				cursor = ret;
				continue;
			}

			auto node = new ListNode(v, nullptr);
			cursor->next = node;
			cursor = node;
		}

		if (overflow) {
			auto node = new ListNode(1, nullptr);
			cursor->next = node;
		}
		return ret;
	}
};

int main() {
	Solution{}.addTwoNumbers(
			new ListNode(vector<int>{9}),
			new ListNode(vector<int>{9, 9})
	)->print();

	return 0;
}