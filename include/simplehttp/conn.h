//
// Created by ztk on 2022/3/29.
//

#pragma once

#include <fmt/core.h>

#include <asio.hpp>
#include <optional>

#include "./message.h"

namespace mycs::simplehttp {

class Conn {
	using tcp = asio::ip::tcp;

   private:
	tcp::socket* sock = nullptr;
	asio::streambuf buf;
	std::string line_buf;

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

	void read_message(Message* msg) {
		this->buf.prepare(1024);
		char status = 0;
		size_t size = 0;
		asio::error_code err;

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

			if (err) {
				fmt::print("{} {}\r\n", err.value(), err.message());
				break;
			}

			switch (status) {
				case 0: {
					msg->fl1.append(static_cast<const char*>(this->buf.data().data()), size - 1);
					status++;
					break;
				}
				case 1: {
					msg->fl2.append(static_cast<const char*>(this->buf.data().data()), size - 1);
					status++;

					break;
				}
				case 2: {
					msg->fl3.append(static_cast<const char*>(this->buf.data().data()), size - 2);
					status++;
					break;
				}
				case 3: {
					line_buf.clear();
					line_buf.append(static_cast<const char*>(this->buf.data().data()), size - 2);
					if (line_buf.empty()) {
						status++;
					} else {
						auto idx = line_buf.find(':');
						if (idx == std::string::npos) return;
						const std::string& key = line_buf.substr(0, idx);
						msg->_headers.append(key, line_buf.substr(idx + 1));
					}
					break;
				}
			}
		}
	}

	void handle() {
		Request req;
		this->read_message(&req);

		asio::write(this->socket(), asio::buffer("HTTP/1.0 200 OK\r\nContent-Length: 12\r\n\r\nHello World!"));
		delete (this);
	}
};

}  // namespace mycs::simplehttp