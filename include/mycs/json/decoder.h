//
// Created by ztk on 2022/4/6.
//

#pragma once

#include <iostream>
#include <stack>

#include "../_utils/index.h"
#include "./value.h"

namespace mycs::json {

class Decoder {
   private:
	std::stack<Value*> stack;
	std::string temp;

	std::string keytemp;
	bool keytempisactive = false;

	bool _done = false;
	Value* _result = nullptr;
	bool skipws = true;	 // skip white space
	bool instring = false;
	bool escaped = false;
	bool isstring = false;

	bool on_map_begin();

	bool on_map_end();

	bool on_array_begin();

	bool on_array_end();

	bool on_value_sep();

	bool on_kv_sep();

	bool on_value_done(Value* val);

   public:
	Decoder() = default;

	virtual ~Decoder() {
		while (!stack.empty()) {
			auto ele = stack.top();
			Value::free_value(ele);
			stack.pop();
		}
	}

	bool feed(char c) {
		if (_done) return false;

		if (skipws) {
			if (std::isspace(c)) return true;
			skipws = false;
		}

		if (escaped) {
			temp.push_back(c);
			escaped = false;
			return true;
		}

		if (c == '\\') {
			escaped = true;
			return true;
		}

		switch (c) {
			case '"': {
				if (instring) {
					instring = false;
					skipws = true;
					isstring = true;
					return true;
				}
				instring = true;
				skipws = false;
				temp.clear();
				return true;
			}
			case '{': {
				return on_map_begin();
			}
			case '}': {
				return on_map_end();
			}
			case '[': {
				return on_array_begin();
			}
			case ']': {
				return on_array_end();
			}
			case ',': {
				return on_value_sep();
			}
			case ':': {
				return on_kv_sep();
			}
			default: {
				temp.push_back(c);
				return true;
			}
		}
	}

	bool feed(const char* p, size_t s) {
		for (int i = 0; i < s; ++i) {
			if (!feed(*(p + i))) return false;
		}
		return true;
	}

	bool feed(const std::string& s) { return feed(s.c_str(), s.size()); }

	Value* result() { return _result; }
};

}  // namespace mycs::json
