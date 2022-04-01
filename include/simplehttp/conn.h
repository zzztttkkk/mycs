//
// Created by ztk on 2022/3/29.
//

#pragma once

#include <fmt/core.h>

#include <asio.hpp>
#include <optional>

#include "./message.h"

#ifndef MYCS_SIMPLEHTTP_CONN_READBUF_INIT_SIZE
#define MYCS_SIMPLEHTTP_CONN_READBUF_INIT_SIZE 1024
#endif

#if MYCS_SIMPLEHTTP_CONN_READBUF_INIT_SIZE < 128
#error "MYCS_SIMPLEHTTP_CONN_READBUF_INIT_SIZE must >= 128"
#endif

namespace mycs::simplehttp {

class Server;

void do_remove_conn_from_server(Server* server, Conn* conn);

class Conn {
	using tcp = asio::ip::tcp;

   private:
	Server* server;
	tcp::socket* sock = nullptr;
	std::string linetemp;

   public:
	Conn(Server* server, tcp::socket* sock) : server(server), sock(sock) {}

	virtual ~Conn() {
		do_remove_conn_from_server(this->server, this);
		if (this->sock) {
			this->sock->close();
			delete (this->sock);
		}
	}

	tcp::socket& socket() { return *this->sock; }

	[[nodiscard]] const tcp::socket& socket() const { return *this->sock; }

	bool read_message(asio::streambuf& readbuf, Message* msg, bool is_req = true) {
		readbuf.prepare(MYCS_SIMPLEHTTP_CONN_READBUF_INIT_SIZE);
		char status = 0;
		size_t size = 0;
		asio::error_code err;

		std::string keytemp;
		std::string valtemp;

		// read first line and headers
		while (status != 4) {
			readbuf.consume(size);
			size = 0;
			err.clear();

			switch (status) {
				case 0:	 // read first line 1
				case 1: {
					// read first line 2
					size = asio::read_until(*this->sock, readbuf, ' ', err);
					break;
				}
				case 2:	 // read first line 3
				case 3: {
					// read header lines
					size = asio::read_until(*this->sock, readbuf, '\n', err);
					break;
				}
				default: {
					break;
				}
			}

			if (err || size < 2) return false;

			switch (status) {
				case 0: {
					// `request method` or `response protocol version`
					if (size > 10) return false;
					linetemp.clear();
					linetemp.assign(static_cast<const char*>(readbuf.data().data()), size - 1);

					valtemp.clear();
					uppercopy(valtemp, linetemp);

					if (!msg->fl1(valtemp)) return false;
					status++;
					linetemp.clear();
					break;
				}
				case 1: {
					// `request path` or `response status code`
					linetemp.append(static_cast<const char*>(readbuf.data().data()), size - 1);
					if (!msg->fl2(linetemp)) return false;
					status++;
					break;
				}
				case 2: {
					// `request protocol version` or `response phrase`
					if (size > 50) return false;
					linetemp.clear();
					linetemp.assign(static_cast<const char*>(readbuf.data().data()), size - 2);

					if (is_req) {
						valtemp.clear();
						uppercopy(valtemp, linetemp);
						if (!msg->fl3(valtemp)) return false;
					} else {
						if (!msg->fl3(linetemp)) return false;
					}

					status++;
					linetemp.clear();
					break;
				}
				case 3: {
					linetemp.append(static_cast<const char*>(readbuf.data().data()), size - 2);
					if (linetemp.empty()) {
						status++;
					} else {
						auto idx = linetemp.find(':');
						if (idx == std::string::npos) return false;

						keytemp.clear();
						trimlowercopy(keytemp, linetemp.substr(0, idx));
						valtemp.clear();
						trimcopy(valtemp, linetemp.substr(idx + 1));
						msg->_headers.append(keytemp, valtemp);
						linetemp.clear();
					}
					break;
				}
				default: {
				}
			}
		}

		// read body
		return true;
	}

	void handle() {
		Request req;
		asio::streambuf buf(1024);
		this->read_message(buf, &req);

		asio::write(this->socket(), asio::buffer("HTTP/1.0 200 OK\r\nContent-Length: 12\r\n\r\nHello World!"));
		delete (this);
	}
};

}  // namespace mycs::simplehttp