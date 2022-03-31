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

class Conn {
	using tcp = asio::ip::tcp;

   private:
	tcp::socket* sock = nullptr;
	asio::streambuf buf;
	std::string temp;

   public:
	explicit Conn(tcp::socket* s) { this->sock = s; }

	virtual ~Conn() {
		if (this->sock) {
			this->sock->close();
			delete (this->sock);
		}
	}

	tcp::socket& socket() { return *this->sock; }

	[[nodiscard]] const tcp::socket& socket() const { return *this->sock; }

	bool read_message(Message* msg) {
		this->buf.prepare(MYCS_SIMPLEHTTP_CONN_READBUF_INIT_SIZE);
		char status = 0;
		size_t size = 0;
		asio::error_code err;

		// read first line and headers
		while (status != 4) {
			this->buf.consume(size);
			size = 0;
			err.clear();

			switch (status) {
				case 0:	 // read first line 1
				case 1: {
					// read first line 2
					size = asio::read_until(*this->sock, this->buf, ' ', err);
					break;
				}
				case 2:	 // read first line 3
				case 3: {
					// read header lines
					size = asio::read_until(*this->sock, this->buf, '\n', err);
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
					temp.clear();
					temp.assign(static_cast<const char*>(this->buf.data().data()), size - 1);
					if (!msg->fl1(temp)) return false;
					status++;
					temp.clear();
					break;
				}
				case 1: {
					// `request path` or `response status code`
					// todo very long path
					temp.append(static_cast<const char*>(this->buf.data().data()), size - 1);
					if (!msg->fl2(temp)) return false;
					status++;
					break;
				}
				case 2: {
					// `request protocol version` or `response phrase`
					if (size > 50) return false;
					temp.clear();
					temp.assign(static_cast<const char*>(this->buf.data().data()), size - 1);
					if (!msg->fl3(temp)) return false;
					status++;
					break;
				}
				case 3: {
					temp.clear();
					temp.append(static_cast<const char*>(this->buf.data().data()), size - 2);
					if (temp.empty()) {
						status++;
					} else {
						auto idx = temp.find(':');
						if (idx == std::string::npos) return false;
						const std::string& key = temp.substr(0, idx);
						msg->_headers.append(key, temp.substr(idx + 1));
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
		this->read_message(&req);

		asio::write(this->socket(), asio::buffer("HTTP/1.0 200 OK\r\nContent-Length: 12\r\n\r\nHello World!"));
		delete (this);
	}
};

}  // namespace mycs::simplehttp