//
// Created by ztk on 2022/4/6.
//

#pragma once

#include <fmt/core.h>

#include <clocale>
#include <cwctype>
#include <iostream>
#include <stack>

#include "../_utils/index.h"
#include "./value.h"

namespace mycs::json {

class DecodeError : public std::exception {
   private:
	friend class Decoder;

	std::string msg;

	DecodeError(int r, int c) : std::exception() { msg = fmt::format("DecodeError: @ {}:{}", r + 1, c); }

   public:
	const char* what() { return msg.c_str(); }
};

enum ValueSepCase { ByComma, BeforeContainerEnd, DecodeEnd };

class Decoder {
   private:
	std::stack<Value*> stack;

	std::string temp;			// string value; key; value
	bool tempisactive = false;	// a flag for empty string value
	bool tempislocked = false;	// lock temp when string end

	// if the parsing succeeds, the result is held by the caller.(it will never be deleted by `Decoder`)
	Value* _result = nullptr;
	bool instring = false;	 // temp is a string, and string is parsing
	bool escaped = false;	 // prev char is `\`
	bool isstring = false;	 // temp is a string, and it is finished
	char unicodestatus = 0;	 // unicode escape char counter
	// when `,`, if temp is not active, and last poped is a container, it also is valid.
	bool lastpopedisacontainer = false;
	// remember error location
	int row = 0;
	int col = 0;

	bool on_map_begin();

	bool on_map_end();

	bool on_array_begin();

	bool on_array_end();

	bool on_value_sep(ValueSepCase vsc);

	bool on_kv_sep();

	bool on_value_done(Value* val);

	inline void clear_temp() {
		temp.clear();
		tempisactive = false;
		tempislocked = false;
	}

	inline void on_error() {
		if (_result) {
			delete (_result);
			_result = nullptr;
		}
		free_stack();
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

	void free_stack() {
		while (!stack.empty()) {
			auto ele = stack.top();
			Value::free_value(ele);
			stack.pop();
		}
	}

   public:
	Decoder() = default;

	virtual ~Decoder() { clear(); }

	void clear() {
		free_stack();
		temp.clear();
		tempisactive = false;
		tempislocked = false;
		_result = nullptr;
		instring = false;
		isstring = false;
		unicodestatus = 0;
		lastpopedisacontainer = false;
		row = 0;
		col = 0;
	}

	Value* result() { return _result; }	 // peek result when feeding

	bool feed(char c) {
		if (c == '\n') {
			row++;
			col = 0;
		} else {
			col++;
		}

		if (std::iswspace(c)) {
			if (instring) {
				switch (c) {
					case '\f':
					case '\r':
					case '\t':
					case '\b':
					case '\n': {
						return false;
					}
					default: {
						temp.push_back(c);
						return true;
					}
				}
			}
			if (!tempislocked && !temp.empty()) temp.push_back(c);
			return true;
		}

		if (_result != nullptr) return false;

		if (instring) {
			if (escaped) {
				escaped = false;
				switch (c) {
					case 'b': {
						temp.push_back('\b');
						return true;
					}
					case 'n': {
						temp.push_back('\n');
						return true;
					}
					case 'f': {
						temp.push_back('\f');
						return true;
					}
					case 'r': {
						temp.push_back('\r');
						return true;
					}
					case 't': {
						temp.push_back('\t');
						return true;
					}
					case 'u': {
						unicodestatus++;
						return true;
					}
					case '/':
					case '"':
					case '\\': {
						temp.push_back(c);
						tempisactive = true;
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

			if (c == '"') {
				instring = false;
				isstring = true;
				tempislocked = true;
				return true;
			}
			goto totemp;
		}

		switch (c) {
			case '"': {
				if (tempisactive) return false;
				instring = true;
				tempisactive = true;
				return true;
			}
			case '{': {
				if (tempisactive) return false;
				return on_map_begin();
			}
			case '}': {
				return on_map_end();
			}
			case '[': {
				if (tempisactive) return false;
				return on_array_begin();
			}
			case ']': {
				return on_array_end();
			}
			case ',': {
				if (stack.empty()) return false;
				auto ele = stack.top();
				if (!ele->is_container()) return false;

				auto ok = on_value_sep(ValueSepCase::ByComma);
				if (lastpopedisacontainer) lastpopedisacontainer = false;

				if (ele->type() == Type::Array) {
					auto& av = ele->array();
					av.requirenext = true;
				} else {
					auto& mv = ele->map();
					mv.requirenext = true;
				}
				return ok;
			}
			case ':': {
				return on_kv_sep();
			}
			default: {
				goto totemp;
			}
		}

	totemp:
		if (tempislocked) return false;

		tempisactive = true;
		temp.push_back(c);
		if (instring && unicodestatus > 0) {
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

	bool feed(const char* p, size_t s) {
		for (int i = 0; i < s; ++i) {
			if (!feed(*(p + i))) return false;
		}
		return true;
	}

	bool feed(const std::string& s) { return feed(s.c_str(), s.size()); }

#define NodiscardMsg "the result will not deleted by `Decode`"

	// finish the feeding
	[[nodiscard(NodiscardMsg)]] inline Value* finish() {
		if (_result) return _result;
		if (!on_value_sep(ValueSepCase::DecodeEnd)) {
			on_error();
			return nullptr;
		}
		return _result;
	}

	[[nodiscard(NodiscardMsg)]] Value* decode(const std::string& txt) {
		if (!feed(txt)) {
			on_error();
			return nullptr;
		}
		return finish();
	}

	[[nodiscard(NodiscardMsg)]] Value* decode(std::istream& input, char* buf, std::streamsize bufsize) {
		while (true) {
			input.read(buf, bufsize);
			auto size = input.gcount();
			if (size < 1) {
				if (input.eof()) break;
				if (input.exceptions() != 0) {
					on_error();
					return nullptr;
				}
				continue;
			}
			if (!feed(buf, size)) {
				on_error();
				return nullptr;
			}
		}
		return finish();
	}

	[[nodiscard(NodiscardMsg)]] Value* decode(std::istream& input) {
		char buf[512];
		return decode(input, buf, 512);
	}

#undef NodiscardMsg

	std::optional<DecodeError> error() {
		if (_result != nullptr) return std::nullopt;
		return std::move(DecodeError(this->row, this->col));
	}
};

}  // namespace mycs::json
