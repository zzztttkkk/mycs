//
// Created by ztk on 2022/4/6.
//

#include <json/decoder.h>

namespace mycs::json {

bool Decoder::on_map_begin() {
	temp.clear();
	auto mv = new MapValue();
	stack.push(mv);
	return true;
}

bool Decoder::on_map_end() {
	if (stack.empty()) return false;
	auto ele = stack.top();
	if (ele->type() != Type::Map) return false;
	stack.pop();
	return on_value_done(ele);
}

bool Decoder::on_array_begin() {
	temp.clear();
	auto av = new ArrayValue();
	stack.push(av);
	return true;
}

bool Decoder::on_array_end() {
	if (stack.empty()) return false;
	auto ele = stack.top();
	if (ele->type() != Type::Array) return false;
	stack.pop();
	return on_value_done(ele);
}

bool Decoder::on_value_sep() {
	if (isstring) {
		auto sv = new StringValue(temp);
		return on_value_done(sv);
	}

	if (temp.length() < 1) {
	} else {
		switch (temp.length()) {
			case 4: {
			}
			case 5: {
			}
		}
	}
	return true;
}

bool Decoder::on_kv_sep() {
	if (!isstring) return false;

	return false;
}

}  // namespace mycs::json