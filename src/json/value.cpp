//
// Created by ztk on 2022/4/5.
//

#include <mycs/json/value.h>

namespace mycs::json {

const Value* Value::peek(const std::vector<typename Value::PathItem>& path) const {
	const Value* cursor = this;
	for (const auto& v : path) {
		if (cursor == nullptr) break;

		const char* key = nullptr;
		switch (v.index()) {
			case 0: {
				if (cursor->t != Type::Array) return nullptr;
				auto& av = cursor->array();
				cursor = av.at(std::get<int>(v));
				continue;
			}
			case 1: {
				key = std::get<std::string>(v).c_str();
				break;
			}
			case 2: {
				key = std::get<const char*>(v);
				break;
			}
		}
		if (cursor->t != Type::Map) return nullptr;
		auto& mv = cursor->map();
		cursor = mv.get(key);
	}
	return cursor;
}

}  // namespace mycs::json