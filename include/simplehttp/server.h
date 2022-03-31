//
// Created by ztk on 2022/3/29.
//

#pragma once

#include <fmt/core.h>

#include <asio.hpp>
#include <iostream>
#include <optional>
#include <shared_mutex>
#include <thread>
#include <unordered_set>

#include "./conn.h"
#include "./handler.h"
#include "./message.h"

namespace mycs::simplehttp {

namespace {
class ServConnCtrlObj;
}

class Server {
	using tcp = asio::ip::tcp;

   private:
	friend class ServConnCtrlObj;

	std::shared_mutex mutex;
	asio::io_context ctx;
	std::optional<tcp::acceptor> acceptor;
	std::unordered_set<Conn*> alive;
	bool running = false;
	bool closed = false;

   public:
	Server() { this->acceptor = std::nullopt; }

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

		tcp::resolver resolver(this->ctx);
		auto result = resolver.resolve(host, std::to_string(port));
		if (result.size() != 1) throw std::runtime_error(fmt::format("bad host: {}\r\n", host));
		this->acceptor = tcp::acceptor(this->ctx, result.begin()->endpoint());
	}

	void close() {
		this->mutex.lock_shared();
		if (!this->running) {
			this->mutex.unlock_shared();
			return;
		}
		this->mutex.unlock_shared();

		this->mutex.lock();
		this->running = false;
		this->acceptor.value().close();
		this->mutex.unlock();

		while (true) {
			this->mutex.lock_shared();
			if (this->alive.empty()) {
				this->mutex.unlock_shared();
				break;
			}
			this->mutex.unlock_shared();
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}

		this->mutex.lock();
		this->closed = true;
		this->mutex.unlock();
	}

	int run() {
		this->mutex.lock();
		if (!this->acceptor.has_value()) {
			this->mutex.unlock();
			throw std::runtime_error("server is not listening");
		}
		this->running = true;
		this->mutex.unlock();

		auto aptr = &this->acceptor.value();
		aptr->listen(256);

		asio::error_code err;

		asio::signal_set signals(this->ctx, SIGINT);
		signals.async_wait([this](auto e, int s) { this->close(); });
		std::thread([this]() { this->ctx.run(); }).detach();

		while (true) {
			err.clear();
			auto conn = new Conn(this, new tcp::socket(this->ctx));
			aptr->accept(conn->socket(), err);
			if (err) {
				if (err.value() == asio::error::interrupted) break;
				continue;
			}
			this->mutex.lock();
			this->alive.insert(conn);
			this->mutex.unlock();
			std::thread([conn]() { conn->handle(); }).detach();
		}

		while (true) {
			this->mutex.lock_shared();
			if (this->closed) {
				this->mutex.unlock_shared();
				break;
			}
			this->mutex.unlock_shared();
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
		return 0;
	}
};

namespace {

class ServConnCtrlObj {
   public:
	static void remove_conn(Server* server, Conn* conn) {
		auto _ = std::lock_guard<std::shared_mutex>(server->mutex);
		server->alive.erase(conn);
	}
};

}  // namespace

}  // namespace mycs::simplehttp