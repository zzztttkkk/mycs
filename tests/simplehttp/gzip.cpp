//
// Created by ztk on 2022/4/2.
//

#include <simplehttp.hpp>

namespace http = mycs::simplehttp;

int main() {
	http::gzip::Compression compression;
	asio::streambuf buf;

	std::cout << compression.init(&buf) << std::endl;
	compression.write("spring");
	compression.write("spring");
	compression.finish();
	return 0;
}