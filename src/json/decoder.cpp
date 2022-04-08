//
// Created by ztk on 2022/4/6.
//

#include <mycs/json/decoder.h>

#include <cerrno>

namespace mycs::json {

bool Decoder::on_map_begin() {
	temp.clear();
	skipws = true;
	auto mv = new MapValue();
	stack.push(mv);
	return true;
}

bool Decoder::on_map_end() {
	if (stack.empty()) return false;
	skipws = true;
	Value* ele = stack.top();
	if (ele->type() != Type::Map) return false;
	if (!on_value_sep()) return false;
	stack.pop();
	return on_value_done(ele);
}

bool Decoder::on_array_begin() {
	temp.clear();
	skipws = true;
	auto av = new ArrayValue();
	stack.push(av);
	return true;
}

bool Decoder::on_array_end() {
	if (stack.empty()) return false;
	skipws = true;
	auto ele = stack.top();
	if (ele->type() != Type::Array) return false;
	if (!on_value_sep()) return false;
	stack.pop();
	return on_value_done(ele);
}

bool Decoder::on_value_sep() {
	skipws = true;
	if (isstring) {
		auto sv = new StringValue(temp);
		// todo unicode
		return on_value_done(sv);
	}

	const static std::string null = "null";
	const static std::string t = "true";
	const static std::string f = "false";

	switch (temp.length()) {
		case 0:
			return false;
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

	int begin = 0;
	for (char c : temp) {
		begin++;
		if (std::isspace(c)) continue;
		begin--;
		break;
	}
	int end = static_cast<int>(temp.size() - 1);
	for (; end > begin; end--) {
		char c = temp[end];
		if (std::isspace(c)) continue;
		end++;
		break;
	}

	if (begin >= end) return false;

	char* endPtr = nullptr;
	double v = std::strtod(temp.data() + begin, &endPtr);
	if (errno != 0 || endPtr != temp.data() + end) return false;
	bool is_int = temp.find('.') == std::string::npos;
	temp.clear();
	return on_value_done(new NumberValue(v, is_int));
}

bool Decoder::on_kv_sep() {
	if (!isstring) return false;
	keytemp = temp;
	keytempisactive = true;
	isstring = false;
	skipws = true;
	temp.clear();
	return true;
}

bool Decoder::on_value_done(Value* val) {
	if (stack.empty()) {
		_done = true;
		_result = val;
		return true;
	}

	Value* ele = stack.top();
	switch (ele->type()) {
		case Type::Map: {
			if (!keytempisactive) return false;
			auto& mv = ele->map();
			mv.insert(keytemp, val);
			keytempisactive = false;
			keytemp.clear();
			return true;
		}
		case Type::Array: {
			if (keytempisactive) return false;
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