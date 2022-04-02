//
// Created by ztk on 2022/4/2.
//

// ref: https://github.com/mapbox/gzip-hpp

#pragma once

#include <zlib.h>

namespace mycs::simplehttp::gzip {

class Compression {
   private:
	z_stream zstream;
	asio::streambuf* buf = nullptr;
	int _level;
	size_t p = 0;

   public:
	explicit Compression(int level = Z_DEFAULT_COMPRESSION) : _level(level){};	// NOLINT

	int init(asio::streambuf* b) {
		zstream.zalloc = Z_NULL;
		zstream.zfree = Z_NULL;
		zstream.opaque = Z_NULL;
		zstream.avail_in = 0;
		zstream.next_in = Z_NULL;
		buf = b;
		return deflateInit2(&zstream, _level, Z_DEFLATED, 31, 8, Z_DEFAULT_STRATEGY);
	}

	void write(const char* data, size_t size) {
		zstream.next_in = reinterpret_cast<z_const Bytef*>((void*)data);
		zstream.avail_in = static_cast<unsigned int>(size);

		size_t size_compressed = 0;
		do {
			size_t increase = size / 2 + 1024;
			if (buf->capacity() < (size_compressed + increase)) {
				buf->prepare(size_compressed + increase);
			}
			zstream.next_out = reinterpret_cast<Bytef*>(((char*)(buf->data().data()) + size_compressed));
			zstream.avail_out = static_cast<unsigned int>(increase);
			deflate(&zstream, Z_NO_FLUSH);
			size_compressed += (increase - zstream.avail_out);
		} while (zstream.avail_out == 0);

		p += size_compressed;
	}

	void finish() {
		deflateEnd(&zstream);
	}

	inline void write(const std::string& val) { return write(val.c_str(), val.size()); }
};

}  // namespace mycs::simplehttp::gzip