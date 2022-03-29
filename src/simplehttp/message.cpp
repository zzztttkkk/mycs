//
// Created by ztk on 2022/3/29.
//

#include <simplehttp/message.h>

namespace mycs::simplehttp {

const std::string h10 = "http/1.0";
const std::string h11 = "http/1.1";
const std::string unknown = "";

const std::string& httpversion2string(HttpVersion version) {
	switch (version) {
		case HttpVersion::H10: {
			return h10;
		}
		case HttpVersion::H11: {
			return h11;
		}
		default: {
			return unknown;
		}
	}
}

std::unordered_map<std::string, HttpVersion> map = {
	{"http/1.1", HttpVersion::H11},
	{"HTTP/1.1", HttpVersion::H11},
	{"http/1.0", HttpVersion::H10},
	{"HTTP/1.0", HttpVersion::H10}
};

HttpVersion string2httpversion(const std::string& vstr) {
	auto iter = map.find(vstr);
	if (iter == map.end()) {
		return HttpVersion::Unknown;
	}
	return iter->second;
}

}  // namespace mycs::simplehttp