//
// Created by ztk on 2022/4/6.
//

#pragma once

#include <iostream>

#include "./value.h"

namespace mycs::json {

class Encoder {
   public:
	typedef std::pair<const std::string*, const Value*> SortPairT;

   private:
	std::ostream* ostream = nullptr;
	std::string buf;
	bool sortkey = false;

	void encode_string(const StringValue& sv);

	void encode_number(const NumberValue& nv);

	void encode_null();

	void encode_bool(bool v);

	bool encode_array(const ArrayValue& av);

	bool encode_map(const MapValue& mv);

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
		return true;
	}

	bool encode(const Value* val) { return encode(*val); }

	void reset(std::ostream& s) {
		buf.clear();
		ostream = &s;
	}
};

}  // namespace mycs::json