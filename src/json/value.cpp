//
// Created by ztk on 2022/4/5.
//

#include <json/value.h>

namespace mycs::json {

void Value::free_value(Value* val) {
	switch (val->type()) {
		case Type::None: {
		}
		case Type::Bool: {
			return;
		}
		default: {
			delete (val);
		}
	}
}

}  // namespace mycs::json