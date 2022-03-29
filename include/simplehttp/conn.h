//
// Created by ztk on 2022/3/29.
//

#pragma once

#include <optional>
#include <asio.hpp>

namespace mycs::simplehttp {

class Conn {
	using tcp = asio::ip::tcp;
private:
	std::optional<tcp::socket> sock;

public:
	explicit Conn(asio::io_context& ctx) {
		this->sock = tcp::socket(ctx);
	}

	virtual ~Conn() {
		if (this->sock.has_value()) {
			this->sock->close();
		}
	}

	tcp::socket& socket() { return this->sock.value(); }
};

}