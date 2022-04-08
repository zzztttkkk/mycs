//
// Created by ztk on 2022/3/29.
//

#pragma once

#include <fmt/core.h>

#include <asio.hpp>
#include <optional>

#include "./buf.h"
#include "./gzip.h"
#include "./message.h"

#ifndef MYCS_SIMPLEHTTP_CONN_READBUF_INIT_SIZE
#define MYCS_SIMPLEHTTP_CONN_READBUF_INIT_SIZE 1024
#endif

#if MYCS_SIMPLEHTTP_CONN_READBUF_INIT_SIZE < 128
#error "MYCS_SIMPLEHTTP_CONN_READBUF_INIT_SIZE must >= 128"
#endif

namespace mycs::http {

class Server;

class Conn {
	using tcp = asio::ip::tcp;

   private:
	friend class ServConnCtrlObj;

	Server* server;
	tcp::socket* sock = nullptr;
	asio::streambuf readbuf;
	std::string linetemp;
	char pstatus = 0;
	size_t rsize = 0;
	asio::error_code err;
	std::string keytemp;
	std::string valtemp;

	class ConnGuard {
	   private:
		Conn* ptr;

	   public:
		explicit ConnGuard(Conn* c) : ptr(c) {}

		~ConnGuard() { delete (this->ptr); }
	};

	void close();

	bool read_message(Message* msg, bool is_req = true) {
		// read first line and headers
		while (pstatus != 4) {
			readbuf.consume(rsize);
			rsize = 0;
			err.clear();

			switch (pstatus) {
				case 0:	 // read first line 1
				case 1: {
					// read first line 2
					rsize = asio::read_until(*this->sock, readbuf, ' ', err);
					break;
				}
				case 2:	 // read first line 3
				case 3: {
					// read header lines
					rsize = asio::read_until(*this->sock, readbuf, '\n', err);
					break;
				}
				default: {
					break;
				}
			}

			if (err || rsize < 2) return false;

			switch (pstatus) {
				case 0: {
					// `request method` or `response protocol version`
					if (rsize > 10) return false;
					linetemp.clear();
					linetemp.assign(static_cast<const char*>(readbuf.data().data()), rsize - 1);

					valtemp.clear();
					uppercopy(valtemp, linetemp);

					if (!msg->fl1(valtemp)) return false;
					pstatus++;
					linetemp.clear();
					break;
				}
				case 1: {
					// `request path` or `response pstatus code`
					linetemp.append(static_cast<const char*>(readbuf.data().data()), rsize - 1);
					if (!msg->fl2(linetemp)) return false;
					pstatus++;
					break;
				}
				case 2: {
					// `request protocol version` or `response phrase`
					if (rsize > 50) return false;
					linetemp.clear();
					linetemp.assign(static_cast<const char*>(readbuf.data().data()), rsize - 2);

					if (is_req) {
						valtemp.clear();
						uppercopy(valtemp, linetemp);
						if (!msg->fl3(valtemp)) return false;
					} else {
						if (!msg->fl3(linetemp)) return false;
					}

					pstatus++;
					linetemp.clear();
					break;
				}
				case 3: {
					linetemp.append(static_cast<const char*>(readbuf.data().data()), rsize - 2);
					if (linetemp.empty()) {
						pstatus++;
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

   public:
	Conn(Server* server, tcp::socket* sock) : server(server), sock(sock) {}

	virtual ~Conn() {
		this->close();
		if (this->sock) {
			this->sock->close();
			delete (this->sock);
		}
	}

	tcp::socket& socket() { return *this->sock; }

	[[nodiscard]] const tcp::socket& socket() const { return *this->sock; }

	void handle();
};

}  // namespace mycs::http