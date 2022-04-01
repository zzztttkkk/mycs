//
// Created by ztk on 2022/3/31.
//

#include <iostream>
#include <simplehttp.hpp>

namespace mycs::simplehttp {

void Conn::create_readbuf() {
	asio::streambuf* buf;
	if (server->option.MaxMessageLineSize > 0) {
		buf = new asio::streambuf(server->option.MaxMessageLineSize);
	} else {
		buf = new asio::streambuf();
	}
	this->_readbuf = buf;
}

void Conn::close() {
	auto _ = std::lock_guard<std::shared_mutex>(server->mutex);
	server->alive.erase(this);
}

}  // namespace mycs::simplehttp