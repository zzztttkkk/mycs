//
// Created by ztk on 2022/3/31.
//

#include <simplehttp/conn.h>
#include <simplehttp/server.h>

namespace mycs::simplehttp {

void Conn::close() {
	auto _ = std::lock_guard<std::shared_mutex>(server->mutex);
	server->alive.erase(this);
}

void Conn::handle() {
	auto _ = ConnGuard(this);
	readbuf.prepare(MYCS_SIMPLEHTTP_CONN_READBUF_INIT_SIZE);

	auto opt = &this->server->option;

	Request req(opt->MaxRequestBodySize, opt->MaxResponseBodySize);
	this->read_message(&req);

	asio::write(this->socket(), asio::buffer("HTTP/1.0 200 OK\r\nContent-Length: 12\r\n\r\nHello World!"));
}

}  // namespace mycs::simplehttp