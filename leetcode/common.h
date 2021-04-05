#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <cassert>

using namespace std;

struct ListNode {
	int val;
	ListNode* next;
public:
	ListNode() : val(0), next(nullptr) {}

	explicit ListNode(int x) : val(x), next(nullptr) {}

	ListNode(int x, ListNode* next) : val(x), next(next) {}

	explicit ListNode(const vector<int>& vec) {
		assert(!vec.empty());
		next = nullptr;

		ListNode* prev = nullptr;
		for (auto item: vec) {
			assert(item < 10 && item >= 0);
			if (prev != nullptr) {
				prev->next = new ListNode(item, nullptr);
				prev = prev->next;
				continue;
			}
			prev = this;
			prev->val = item;
		}
	}

	void print() const {
		cout << this->val;
		if (this->next == nullptr) {
			return;
		}
		cout << "->";
		this->next->print();
	}
};