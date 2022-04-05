//
// Created by ztk on 2022/4/5.
//

#include <json/value.h>

namespace mycs::json {

bool Value::is_none() const { return this == &None; }

}  // namespace mycs::json