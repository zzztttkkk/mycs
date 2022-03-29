//
// Created by ztk on 2022/3/29.
//

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <asio.hpp>

namespace mycs::simplehttp {

#ifndef MYCS_SIMPLEHTTP_HEADERS_VECTOR_SIZE
#define MYCS_SIMPLEHTTP_HEADERS_VECTOR_SIZE 12
#endif

class Headers {
private:
	class Pair {
	private:
		friend class Headers;

		std::string key;
		std::string val;
		bool invalid = true;

	public:
		Pair(const std::string& k, const std::string& v) {
			this->load(k, v);
		}

		void load(const std::string& k, const std::string& v) {
			this->key = k;
			this->val = v;
			this->invalid = false;
		}
	};

	std::vector<Pair> items;
	typedef std::vector<std::string> VecType;
	typedef std::vector<const std::string*> ValVecType;
	typedef std::unordered_map<std::string, typename Headers::VecType*> MapType;
	typename Headers::MapType* map = nullptr;

	void append_to_map(const std::string& key, const std::string& val) {
		auto result = this->map->find(key);
		std::vector<std::string>* vec;
		if (result == this->map->end()) {
			vec = new (typename Headers::VecType)();
			this->map->insert(std::make_pair(key, vec));
		} else {
			vec = result->second;
		}
		vec->push_back(val);
	}

public:
	Headers() = default;

	virtual ~Headers() {
		if (this->map != nullptr) {
			for (const auto& pair: *this->map) {
				delete (pair.second);
			}
			delete (this->map);
		}
	}

	void clear() {
		this->items.clear();
		if (this->map != nullptr) this->map->clear();
	}

	void append(const std::string& key, const std::string& val) {
		if (this->map != nullptr) {
			this->append_to_map(key, val);
			return;
		}

		for (auto& pair: this->items) {
			if (pair.invalid) {
				pair.load(key, val);
				return;
			}
		}

		this->items.emplace_back(key, val);
		if (this->items.size() > MYCS_SIMPLEHTTP_HEADERS_VECTOR_SIZE) {
			this->map = new (typename Headers::MapType)();
			for (const auto& pair: this->items) {
				if (pair.invalid) continue;
				this->append_to_map(pair.key, pair.val);
			}
			this->items.clear();
		}
	}

	void reset(const std::string& key, const std::string& val) {
		this->remove(key);
		this->append(key, val);
	}

	[[nodiscard]]
	bool exists(const std::string& key) const {
		if (this->map != nullptr) {
			return this->map->contains(key);
		}
		for (const auto& pair: this->items) {
			if (pair.invalid) continue;
			if (pair.key == key) {
				return true;
			}
		}
		return false;
	}

	[[nodiscard]]
	const std::string* one(const std::string& key) const {
		if (this->map != nullptr) {
			auto iter = this->map->find(key);
			if (iter == this->map->end()) return nullptr;
			return &(iter->second->at(0));
		}
		for (const auto& pair: this->items) {
			if (pair.invalid) continue;
			if (pair.key == key) {
				return &(pair.val);
			}
		}
		return nullptr;
	}

	[[nodiscard]]
	std::unique_ptr<typename Headers::ValVecType> all(const std::string& key) const {
		if (this->map != nullptr) {
			auto iter = this->map->find(key);
			if (iter == this->map->end()) return nullptr;
			auto _ptr = new typename Headers::ValVecType();
			for (const auto& val: *iter->second) {
				_ptr->push_back(&val);
			}
			return std::unique_ptr<typename Headers::ValVecType>(_ptr);
		}

		auto _ptr = new typename Headers::ValVecType();
		for (const auto& pair: this->items) {
			if (pair.invalid) continue;
			if (pair.key == key) {
				_ptr->push_back(&(pair.val));
			}
		}
		return std::unique_ptr<typename Headers::ValVecType>(_ptr);
	}

	void remove(const std::string& key) {
		if (this->map != nullptr) {
			auto iter = this->map->find(key);
			if (iter == this->map->end()) return;
			delete (iter->second);
			this->map->erase(iter);
			return;
		}

		for (auto& pair: this->items) {
			if (pair.invalid) continue;
			if (pair.key == key) {
				pair.invalid = true;
			}
		}
	}

	void each(const std::function<bool(const std::string&, std::string&)>& fn) {
		if (this->map != nullptr) {
			for (auto& pair: *this->map) {
				for (auto& val: *pair.second) {
					if (!fn(pair.first, val)) return;
				}
			}
			return;
		}

		for (auto& pair: this->items) {
			if (pair.invalid) continue;
			if (!fn(pair.key, pair.val)) return;
		}
	}

	void each(const std::function<bool(const std::string&, const std::string&)>& fn) const {
		if (this->map != nullptr) {
			for (const auto& pair: *this->map) {
				for (const auto& val: *pair.second) {
					if (!fn(pair.first, val)) return;
				}
			}
			return;
		}

		for (const auto& pair: this->items) {
			if (pair.invalid) continue;
			if (!fn(pair.key, pair.val)) return;
		}
	}
};

class Message {
protected:
	char status = 0;
	std::string fl1;
	std::string fl2;
	std::string fl3;
	Headers _headers;
	asio::mutable_buffer rbuf;
	asio::mutable_buffer wbuf;

	Message() = default;

	virtual ~Message() = default;

public:
	Headers& headers() { return this->_headers; }

	[[nodiscard]]
	const Headers& headers() const { return this->_headers; }

	// fl1 1 fl2 2 fl3 3
	// headers 4
	// body 5
	int feed(char c) {

	}
};

enum class HttpVersion : uint16_t {
	Unknown = 0x00,
	H10 = 0x10,
	H11 = 0x11,
};

const std::string& httpversion2string(HttpVersion version);

HttpVersion string2httpversion(const std::string&);

class Request : public Message {
public:
	Request() : Message() {}

	[[nodiscard]]
	const std::string& method() const { return this->fl1; }

	Request* method(const std::string& method) {
		this->fl1 = method;
		return this;
	}

	[[nodiscard]]
	const std::string& rawpath() const { return this->fl2; }

	Request* rawpath(const std::string& rawpath) {
		this->fl2 = rawpath;
		return this;
	}

	[[nodiscard]]
	HttpVersion version() const { return string2httpversion(this->fl1); }

	Request* version(HttpVersion version) {
		this->fl3 = httpversion2string(version);
		return this;
	}
};


class Response : public Message {
private:
	uint16_t _status = 0;

public:
	Response() : Message() {
	}

	[[nodiscard]]
	HttpVersion version() const { return string2httpversion(this->fl1); }

	Response* version(HttpVersion version) {
		this->fl1 = httpversion2string(version);
		return this;
	}

	[[nodiscard]]
	uint16_t status() const {
		if (this->_status != 0) return this->_status;
		if (this->fl2.empty()) return 0;
		return static_cast<uint16_t>(std::strtol(this->fl2.c_str(), nullptr, 10));
	}

	[[nodiscard]]
	uint16_t status() {
		if (this->_status != 0) return this->_status;
		this->_status = const_cast<const Response*>(this)->status();
		return this->_status;
	}

	[[nodiscard]]
	const std::string& phrase() const { return this->fl3; }
};

}