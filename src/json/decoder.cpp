//
// Created by ztk on 2022/4/6.
//

#include <json/decoder.h>

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
	auto ele = stack.top();
	if (ele->type() != Type::Map) return false;
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
	stack.pop();
	return on_value_done(ele);
}

bool Decoder::on_value_sep() {
	skipws = true;
	if (isstring) {
		auto sv = new StringValue(temp);
		return on_value_done(sv);
	}

	const static std::string null = "null";
	const static std::string t = "true";
	const static std::string f = "false";

	if (temp.length() < 1) {

	} else {
		switch (temp.length()) {
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
	}
	return true;
}

bool Decoder::on_kv_sep() {
	if (!isstring) return false;
	keytemp = temp;
	isstring = false;
	return true;
}

}  // namespace mycs::json