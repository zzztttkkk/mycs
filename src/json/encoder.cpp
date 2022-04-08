#include <mycs/json/encoder.h>

namespace mycs::json {

void write_string(std::ostream& o, const char* p, size_t s) {
	const static char* sep = "\"";
	const static char* escape = "\\";
	const auto temp = std::string(p, s);

	if (temp.find('"') == std::string::npos) {
		o.write(sep, 1);
		o.write(p, static_cast<std::streamsize>(s));
		o.write(sep, 1);
		return;
	}

	o.write(sep, 1);
	for (int i = 0; i < s; ++i) {
		char c = *(p + i);
		if (c == '"') {
			o.write(escape, 1);
			o.write(p + i, 1);
		} else {
			o.write(p + i, 1);
		}
	}
	o.write(sep, 1);
}

void Encoder::encode_string(const StringValue& sv) { write_string(*ostream, sv._data.c_str(), sv._data.size()); }

void Encoder::encode_number(const NumberValue& nv) {
	buf.clear();
	if (nv.is_int) {
		buf = std::to_string(static_cast<int64_t>(nv._data));
	} else {
		buf = std::to_string(nv._data);
	}
	ostream->write(buf.c_str(), static_cast<std::streamsize>(buf.size()));
}

void Encoder::encode_null() {
	const static char* none = "null";
	ostream->write(none, 4);
}

void Encoder::encode_bool(bool v) {
	const static char* t = "true";
	const static char* f = "false";
	if (v) {
		ostream->write(t, 4);
	} else {
		ostream->write(f, 5);
	}
}

bool Encoder::encode_array(const ArrayValue& av) {
	const static char* begin = "[";
	const static char* end = "]";
	const static char* sep = ",";
	ostream->write(begin, 1);
	int idx = 0;
	int lastIdx = static_cast<int>(av.size()) - 1;
	for (const Value* v : av._data) {
		encode(*v);
		if (idx < lastIdx) ostream->write(sep, 1);
		if (ostream->exceptions() != 0) return false;
		idx++;
	}
	ostream->write(end, 1);
	return ostream->exceptions() == 0;
}

bool Encoder::encode_map(const MapValue& mv) {
	const static char* begin = "{";
	const static char* end = "}";
	const static char* kvsep = ":";
	const static char* sep = ",";
	ostream->write(begin, 1);
	int idx = 0;
	int lastIdx = static_cast<int>(mv.size()) - 1;
	for (const auto& pair : mv._data) {
		write_string(*ostream, pair.first.c_str(), pair.first.size());
		ostream->write(kvsep, 1);
		encode(*pair.second);

		if (idx < lastIdx) ostream->write(sep, 1);
		idx++;
	}
	ostream->write(end, 1);
	return ostream->exceptions() == 0;
}

}  // namespace mycs::json