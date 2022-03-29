//
// Created by ztk on 2022/3/29.
//

#pragma once

#include <optional>
#include <asio.hpp>
#include <shared_mutex>
#include "./conn.h"
#include "./handler.h"
#include "./message.h"

namespace mycs::simplehttp {

class Server {
	using tcp = asio::ip::tcp;

private:
	std::shared_mutex mutex;
	asio::io_context ctx;
	std::optional<tcp::acceptor> acceptor;

public:
	Server() {
		this->acceptor = std::nullopt;
	}

	virtual ~Server() {
		if (this->acceptor.has_value()) {
			this->acceptor.value().close();
		}
	}

	void listen(uint16_t port) {
		std::lock_guard<std::shared_mutex> _(this->mutex);
		if (this->acceptor.has_value()) throw std::runtime_error("server is listening");

		this->acceptor = tcp::acceptor(this->ctx, tcp::endpoint(tcp::v6(), port));
	}

	void listen(const std::string& host, uint16_t port) {
		std::lock_guard<std::shared_mutex> _(this->mutex);
		if (this->acceptor.has_value()) throw std::runtime_error("server is listening");

		asio::ip::address address;
		address.from_string(host);
		tcp::endpoint endpoint;
		endpoint.address(address);
		endpoint.port(port);
		this->acceptor = tcp::acceptor(this->ctx, endpoint);
	}

	void run() {
		this->mutex.lock();
		if (!this->acceptor.has_value()) {
			this->mutex.unlock();
			throw std::runtime_error("server is not listening");
		}
		this->mutex.unlock();

		auto acceptor_ptr = &this->acceptor.value();
		asio::ip::tcp::socket sock(this->ctx);

		while (true) {
			acceptor_ptr->accept(sock);
			asio::write(sock, asio::buffer("HTTP/1.0 200 OK\r\nContent-Length: 11\r\n\r\nHello World"));
			sock.close();
		}
	}
};

}