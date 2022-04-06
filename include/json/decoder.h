//
// Created by ztk on 2022/4/6.
//

#pragma once

#include <iostream>
#include <stack>

#include "./value.h"

namespace mycs::json {

class Decoder {
   private:
	std::stack<Value*> stack;
	std::string temp;
	std::string keytemp;

	bool skipws = true;	 // skip white space
	bool instring = false;

   public:
	Decoder() = default;

	virtual ~Decoder() {
		while (!stack.empty()) {
			auto ele = stack.top();
			Value::free_value(ele);
			stack.pop();
		}
	}

	bool feed(char c);

	bool feed(const char* p, size_t s) {
		for (int i = 0; i < s; ++i) {
			if (!feed(*(p + i))) return false;
		}
		return true;
	}

	bool feed(const std::string& s) { return feed(s.c_str(), s.size()); }
};

}  // namespace mycs::json
