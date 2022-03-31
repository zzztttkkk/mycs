//
// Created by ztk on 2022/3/29.
//

#include <simplehttp/message.h>

namespace mycs::simplehttp {

const std::string& protocolversion2string(ProtocolVersion version) {
	const static std::string io = "HTTP/1.0";
	const static std::string ii = "HTTP/1.1";
	const static std::string zo = "HTTP/2.0";
	const static std::string unknown;

	switch (version) {
		case ProtocolVersion::IO: {
			return io;
		}
		case ProtocolVersion::II: {
			return ii;
		}
		case ProtocolVersion::ZO: {
			return zo;
		}
		default: {
			return unknown;
		}
	}
}

ProtocolVersion string2protocolversion(const std::string& vstr) {
	if (vstr.size() != 8) return ProtocolVersion::Unknown;
	if (!vstr.compare(0, 5, "HTTP/")) return ProtocolVersion::Unknown;
	if (vstr[5] == '1') {
		if (!vstr.compare(6, 2, ".1")) return ProtocolVersion::II;
		if (!vstr.compare(6, 2, ".0")) return ProtocolVersion::IO;
		return ProtocolVersion::Unknown;
	}
	if (!vstr.compare(5, 3, "2.0")) return ProtocolVersion::ZO;
	return ProtocolVersion::Unknown;
}

const std::string& method2string(Method method) {
	const static std::string get = "GET";
	const static std::string head = "HEAD";
	const static std::string post = "POST";
	const static std::string put = "PUT";
	const static std::string _delete = "DELETE";
	const static std::string connect = "CONNECT";
	const static std::string options = "OPTIONS";
	const static std::string trace = "TRACE";
	const static std::string patch = "PATCH";
	const static std::string unknown;

	switch (method) {
		case Method::Get: {
			return get;
		}
		case Method::Head: {
			return head;
		}
		case Method::Post: {
			return post;
		}
		case Method::Put: {
			return put;
		}
		case Method::Delete: {
			return _delete;
		}
		case Method::Connect: {
			return connect;
		}
		case Method::Options: {
			return options;
		}
		case Method::Trace: {
			return trace;
		}
		case Method::Patch: {
			return patch;
		}
		case Method::Unknown: {
			return unknown;
		}
	}
}

Method string2method(const std::string& m) {
	if (m.empty() || m.size() > 7) return Method::Unknown;
	switch (m[0]) {
		case 'G': {
			if (!m.compare(1, 2, "ET")) return Method::Get;
			break;
		}
		case 'P': {
			switch (m.size()) {
				case 3: {
					if (!m.compare(1, 2, "UT")) return Method::Put;
					break;
				}
				case 4: {
					if (!m.compare(1, 3, "OST")) return Method::Post;
					break;
				}
				case 5: {
					if (!m.compare(1, 4, "ATCH")) return Method::Patch;
					break;
				}
				default: {
					break;
				}
			}
			break;
		}
		case 'H': {
			if (!m.compare(1, 3, "EAD")) return Method::Head;
			break;
		}
		case 'D': {
			if (!m.compare(1, 5, "ELETE")) return Method::Delete;
			break;
		}
		case 'C': {
			if (!m.compare(1, 6, "ONNECT")) return Method::Connect;
			break;
		}
		case 'O': {
			if (!m.compare(1, 6, "PTIONS")) return Method::Options;
			break;
		}
		case 'T': {
			if (!m.compare(1, 4, "RACE")) return Method::Trace;
			break;
		}
		default: {
			return Method::Unknown;
		}
	}
	return Method::Unknown;
}

}  // namespace mycs::simplehttp