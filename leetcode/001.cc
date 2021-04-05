#include "./common.h"

class Solution {
public:
	vector<int> twoSum(vector<int>& nums, int target) {
		vector<int> ret;
		unordered_map<int, int> temp;

		int i = 0;
		for (auto item: nums) {
			auto v = target - item;
			auto iter = temp.find(v);
			if (iter != temp.end()) {
				ret.push_back(iter->second);
				ret.push_back(i);
				break;
			}

			temp[item] = i;
			i++;
		}

		return ret;
	}
};

int main() {
	vector<int> sample = {2, 2, 2};

	for (auto item: Solution{}.twoSum(sample, 4)) {
		cout << item << endl;
	}
	return 0;
}