//
// Created by ztk on 2022/4/6.
//

#include <json/decoder.h>

namespace mycs::json {

bool Decoder::feed(char c) {
	if (skipws) {
		if (std::isspace(c)) return true;
		skipws = false;
	}

	return true;
}

}  // namespace mycs::json