#include <mycs/json/encoder.h>

#include <algorithm>

namespace mycs::json {

void Encoder::write_string(const char* p, size_t s) {
	write('"');
	for (int i = 0; i < s; ++i) {
		char c = *(p + i);
		switch (c) {
			case '\\':
			case '"': {
				write('\\');
				write(p + i, 1);
				break;
			}
			case '\r': {
				write('\\');
				write('r');
				break;
			}
			case '\t': {
				write('\\');
				write('t');
				break;
			}
			case '\f': {
				write('\\');
				write('f');
				break;
			}
			case '\n': {
				write('\\');
				write('n');
				break;
			}
			case '\b': {
				write('\\');
				write('b');
				break;
			}
			default: {
				write(c);
			}
		}
	}
	write('"');
}

void Encoder::encode_number(const NumberValue& nv) {
	buf.clear();
	if (nv.is_int) {
		buf = std::to_string(static_cast<int64_t>(nv._data));
	} else {
		buf = std::to_string(nv._data);
	}
	ostream->write(buf.c_str(), static_cast<std::streamsize>(buf.size()));
}

bool Encoder::encode_array(const ArrayValue& av) {
	write('[');
	int idx = 0;
	int lastIdx = static_cast<int>(av.size()) - 1;
	for (const Value* v : av._data) {
		if (!encode(*v)) return false;
		if (idx < lastIdx) write(',');
		idx++;
	}
	return write(']');
}

class PairSortObj {
   public:
	inline bool operator()(const typename Encoder::SortPair& a, const typename Encoder::SortPair& b) {
		return a.first->compare(*b.first) < 0;
	}
};

bool Encoder::encode_map(const MapValue& mv) {
	write('{');
	int idx = 0;
	int lastIdx = static_cast<int>(mv.size()) - 1;
	if (sortkey) {
		std::vector<typename Encoder::SortPair> sorttemp;
		sorttemp.reserve(mv._data.size());
		for (const auto& pair : mv._data) sorttemp.emplace_back(&(pair.first), pair.second);
		std::sort(sorttemp.begin(), sorttemp.end(), PairSortObj{});
		for (const auto& pair : sorttemp) {
			write_string(pair.first->c_str(), pair.first->size());
			write(':');
			if (!encode(*pair.second)) return false;

			if (idx < lastIdx) write(',');
			idx++;
		}
	} else {
		for (const auto& pair : mv._data) {
			write_string(pair.first.c_str(), pair.first.size());
			write(':');
			if (!encode(*pair.second)) return false;

			if (idx < lastIdx) write(',');
			idx++;
		}
	}
	return write('}');
}

}  // namespace mycs::json