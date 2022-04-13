//
// Created by ztk on 2022/4/6.
//

#include <mycs/json/decoder.h>

namespace mycs::json {

// ] } ,
bool Decoder::AfterContainerChars[255] = {
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false, false, true,  false, false, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false, true,  false, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false, true,  false, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};

bool Decoder::on_map_begin() {
	auto mv = new MapValue();
	stack.push(mv);
	return true;
}

bool Decoder::on_map_end() {
	if (stack.empty()) return false;
	Value* ele = stack.top();
	if (ele->type() != Type::Map) return false;
	if (!on_value_sep(ValueSepCase::BeforeContainerEnd)) return false;
	stack.pop();
	lastpopedisacontainer = true;
	return on_value_done(ele);
}

bool Decoder::on_array_begin() {
	auto av = new ArrayValue();
	stack.push(av);
	return true;
}

bool Decoder::on_array_end() {
	if (stack.empty()) return false;
	auto ele = stack.top();
	if (ele->type() != Type::Array) return false;
	if (!on_value_sep(ValueSepCase::BeforeContainerEnd)) return false;
	stack.pop();
	lastpopedisacontainer = true;
	return on_value_done(ele);
}

bool Decoder::on_value_sep(ValueSepCase vsc) {
	switch (vsc) {
		case ValueSepCase::DecodeEnd: {
			if (!tempisactive) return !requirenext;
			break;
		}
		case ValueSepCase::ByComma: {
			if (stack.empty()) return false;
			auto ele = stack.top();
			switch (ele->type()) {
				case Type::Map:
				case Type::Array: {
					break;
				}
				default: {
					return false;
				}
			}
			if (!tempisactive) return false;
			break;
		}
		case ValueSepCase::BeforeContainerEnd: {
			if (stack.empty()) return false;
			auto ele = stack.top();
			switch (ele->type()) {
				case Type::Map:
				case Type::Array: {
					break;
				}
				default: {
					return false;
				}
			}
			if (!tempisactive) return true;
			break;
		}
	}

	if (isstring) {
		if (unicodestatus != 0) return false;
		isstring = false;
		return on_value_done(new StringValue(temp));
	}

	const static std::string null = "null";
	const static std::string t = "true";
	const static std::string f = "false";

	switch (temp.length()) {
		case 0: {
			return false;
		}
		case 4: {
			if (temp == null) return on_value_done(Null);
			if (temp == t) return on_value_done(True);
			break;
		}
		case 5: {
			if (temp == f) return on_value_done(False);
			break;
		}
	}

	char* endPtr = nullptr;
	double v = std::strtod(temp.data(), &endPtr);
	if (errno != 0 || endPtr != temp.data() + temp.size()) return false;
	bool is_int = temp.find('.') == std::string::npos;
	return on_value_done(new NumberValue(v, is_int));
}

bool Decoder::on_kv_sep() {
	if (!isstring) return false;
	if (stack.empty()) return false;
	auto ele = stack.top();
	if (ele->type() != Type::Map) return false;

	auto& mv = ele->map();
	mv.keytemp = temp;
	mv.keytempisactive = true;
	isstring = false;
	clear_temp();
	return true;
}

bool Decoder::on_value_done(Value* val) {
	clear_temp();
	if (requirenext) requirenext = false;

	if (stack.empty()) {
		_result = val;
		return true;
	}

	Value* ele = stack.top();
	switch (ele->type()) {
		case Type::Map: {
			auto& mv = ele->map();
			if (!mv.keytempisactive) return false;
			mv.insert(mv.keytemp, val);
			mv.keytempisactive = false;
			mv.keytemp.clear();
			return true;
		}
		case Type::Array: {
			auto& av = ele->array();
			av.push(val);
			return true;
		}
		default: {
			return false;
		}
	}
}

}  // namespace mycs::json