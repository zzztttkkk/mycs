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
	bool tempisactive = false;	// a flag for empty string value

	std::string keytemp;
	bool keytempisactive = false;

	bool _done = false;
	Value* _result = nullptr;
	bool skipws = true;	 // skip white space
	bool instring = false;
	bool escaped = false;
	bool isstring = false;
	char unicodestatus = 0;

	bool on_map_begin();

	bool on_map_end();

	bool on_array_begin();

	bool on_array_end();

	bool on_value_sep();

	bool on_kv_sep();

	bool on_value_done(Value* val);

	inline void clear_temp() {
		this->temp.clear();
		this->tempisactive = false;
	}

	// https://stackoverflow.com/a/19968992/6683474
	static void uint64_to_unicode(std::string& dist, uint64_t v) {
		if (v <= 0x7f) return dist.push_back(static_cast<char>(v));
		if (v <= 0x7ff) {
			dist.push_back(static_cast<char>(0xc0 | (v >> 6) & 0x1f));
			dist.push_back(static_cast<char>(0x80 | (v & 0x3f)));
			return;
		}
		if (v <= 0xffff) {
			dist.push_back(static_cast<char>(0xe0 | ((v >> 12) & 0x0f)));
			dist.push_back(static_cast<char>(0x80 | ((v >> 6) & 0x3f)));
			dist.push_back(static_cast<char>(0x80 | (v & 0x3f)));
			return;
		}
		dist.push_back(static_cast<char>(0xf0 | ((v >> 18) & 0x07)));
		dist.push_back(static_cast<char>(0x80 | ((v >> 12) & 0x3f)));
		dist.push_back(static_cast<char>(0x80 | ((v >> 6) & 0x3f)));
		dist.push_back(static_cast<char>(0x80 | (v & 0x3f)));
	}

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
			escaped = false;
			switch (c) {
				case 'u': {
					unicodestatus++;
					return true;
				}
				case '"':
				case '\\': {
					temp.push_back(c);
					return true;
				}
				default: {
					return false;
				}
			}
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
				if (tempisactive) return false;
				instring = true;
				skipws = false;
				tempisactive = true;
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
				if (unicodestatus > 0) {
					unicodestatus++;
					if (unicodestatus == 5) {
						unicodestatus = 0;
						const std::string& val = temp.substr(temp.size() - 4);

						char* endPtr = nullptr;
						uint64_t v = std::strtoull(val.data(), &endPtr, 16);
						if (errno != 0 || endPtr != val.data() + 4) return false;
						temp.erase(temp.size() - 4, 4);
						Decoder::uint64_to_unicode(temp, v);
					}
				}
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
