//
// Created by ztk on 2022/4/6.
//

#pragma once

#include <iostream>

#include "./value.h"

#ifndef MYCS_JSON_ENCODER_WBUF_CAP
#define MYCS_JSON_ENCODER_WBUF_CAP 2048
#endif

namespace mycs::json {

class Encoder {
   public:
	typedef std::pair<const std::string*, const Value*> SortPair;

   private:
	std::ostream* ostream = nullptr;
	std::string buf;
	std::string wbuf;
	bool sortkey = false;

	void write_string(const char* p, size_t s);

	void encode_string(const StringValue& sv) { write_string(sv._data.c_str(), sv._data.size()); }

	void encode_number(const NumberValue& nv);

	inline void encode_null() {
		const static char* none = "null";
		write(none, 4);
	}

	inline void encode_bool(bool v) {
		const static char* t = "true";
		const static char* f = "false";
		if (v) {
			write(t, 4);
		} else {
			write(f, 5);
		}
	}

	bool encode_array(const ArrayValue& av);

	bool encode_map(const MapValue& mv);

	inline bool write(const char* d, size_t s) {
		if (wbuf.size() + s > MYCS_JSON_ENCODER_WBUF_CAP) {
			flush();
			if (s > (MYCS_JSON_ENCODER_WBUF_CAP >> 2)) {
				ostream->write(d, static_cast<std::streamsize>(s));
				return ostream->exceptions() == 0;
			}
		}
		wbuf.append(d, s);
		return flush();
	}

	inline bool write(char c) {
		wbuf.push_back(c);
		return true;
	}

	inline bool flush() {
		if (wbuf.size() < MYCS_JSON_ENCODER_WBUF_CAP) return true;
		ostream->write(wbuf.c_str(), static_cast<std::streamsize>(wbuf.size()));
		wbuf.clear();
		return ostream->exceptions() == 0;
	}

   public:
	explicit Encoder(std::ostream& s, bool sortkey = false) {
		ostream = &s;
		this->sortkey = sortkey;
	}

	bool encode(const Value& val) {
		buf.clear();
		switch (val.type()) {
			case Type::String: {
				encode_string(val.string());
				break;
			}
			case Type::Number: {
				encode_number(val.number());
				break;
			}
			case Type::Bool: {
				encode_bool(val.boolean()._data);
				break;
			}
			case Type::Null: {
				encode_null();
				break;
			}
			case Type::Array: {
				if (!encode_array(val.array())) return false;
				break;
			}
			case Type::Map: {
				if (!encode_map(val.map())) return false;
				break;
			}
			default: {
				return false;
			}
		}
		if (!wbuf.empty()) {
			ostream->write(wbuf.c_str(), static_cast<std::streamsize>(wbuf.size()));
			wbuf.clear();
		}
		return ostream->exceptions() == 0;
	}

	bool encode(const Value* val) { return encode(*val); }

	void reset(std::ostream& s) {
		buf.clear();
		wbuf.clear();
		ostream = &s;
	}
};

}  // namespace mycs::json