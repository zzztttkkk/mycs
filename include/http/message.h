//
// Created by ztk on 2022/3/29.
//

#pragma once

#include <asio.hpp>
#include <cctype>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "./opts.h"

namespace mycs::http {

#ifndef MYCS_SIMPLEHTTP_HEADERS_VECTOR_SIZE
#define MYCS_SIMPLEHTTP_HEADERS_VECTOR_SIZE 12
#endif

void trimlowercopy(std::string& dist, const std::string& src);

void trimcopy(std::string& dist, const std::string& src);

void uppercopy(std::string& dist, const std::string& src);

class Headers {
   private:
	class Pair {
	   private:
		friend class Headers;

		std::string key;
		std::string val;
		bool invalid = true;

	   public:
		Pair(const std::string& k, const std::string& v) { this->load(k, v); }

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
			for (const auto& pair : *this->map) {
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

		for (auto& pair : this->items) {
			if (pair.invalid) {
				pair.load(key, val);
				return;
			}
		}

		this->items.emplace_back(key, val);
		if (this->items.size() > MYCS_SIMPLEHTTP_HEADERS_VECTOR_SIZE) {
			this->map = new (typename Headers::MapType)();
			for (const auto& pair : this->items) {
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

	[[nodiscard]] bool exists(const std::string& key) const {
		if (this->map != nullptr) {
			return this->map->contains(key);
		}
		return std::any_of(this->items.begin(), this->items.end(), [key](const auto& pair) {
			return !pair.invalid && pair.key == key;
		});
	}

	[[nodiscard]] const std::string* one(const std::string& key) const {
		if (this->map != nullptr) {
			auto iter = this->map->find(key);
			if (iter == this->map->end()) return nullptr;
			return &(iter->second->at(0));
		}
		for (const auto& pair : this->items) {
			if (pair.invalid) continue;
			if (pair.key == key) {
				return &(pair.val);
			}
		}
		return nullptr;
	}

	[[nodiscard]] std::unique_ptr<typename Headers::ValVecType> all(const std::string& key) const {
		if (this->map != nullptr) {
			auto iter = this->map->find(key);
			if (iter == this->map->end()) return nullptr;
			auto _ptr = new typename Headers::ValVecType();
			for (const auto& val : *iter->second) {
				_ptr->push_back(&val);
			}
			return std::unique_ptr<typename Headers::ValVecType>(_ptr);
		}

		auto _ptr = new typename Headers::ValVecType();
		for (const auto& pair : this->items) {
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

		for (auto& pair : this->items) {
			if (pair.invalid) continue;
			if (pair.key == key) {
				pair.invalid = true;
			}
		}
	}

	void each(const std::function<bool(const std::string&, std::string&)>& fn) {
		if (this->map != nullptr) {
			for (auto& pair : *this->map) {
				for (auto& val : *pair.second) {
					if (!fn(pair.first, val)) return;
				}
			}
			return;
		}

		for (auto& pair : this->items) {
			if (pair.invalid) continue;
			if (!fn(pair.key, pair.val)) return;
		}
	}

	void each(const std::function<bool(const std::string&, const std::string&)>& fn) const {
		if (this->map != nullptr) {
			for (const auto& pair : *this->map) {
				for (const auto& val : *pair.second) {
					if (!fn(pair.first, val)) return;
				}
			}
			return;
		}

		for (const auto& pair : this->items) {
			if (pair.invalid) continue;
			if (!fn(pair.key, pair.val)) return;
		}
	}
};

class Conn;

class Message {
   protected:
	friend class Conn;

	Headers _headers;
	asio::streambuf* rbuf = nullptr;
	asio::streambuf* wbuf = nullptr;

	Message(size_t rsize, size_t wsize) {
		this->rbuf = new asio::streambuf(rsize);
		this->wbuf = new asio::streambuf(wsize);
	};

	virtual ~Message() {
		delete (this->rbuf);
		delete (this->wbuf);
	}

	virtual bool fl1(const std::string&) = 0;
	virtual bool fl2(const std::string&) = 0;
	virtual bool fl3(const std::string&) = 0;

	virtual void clear() {
		this->_headers.clear();
		this->rbuf->consume(this->rbuf->size());
		this->wbuf->consume(this->wbuf->size());
	}

   public:
	Headers& headers() { return this->_headers; }

	[[nodiscard]] const Headers& headers() const { return this->_headers; }
};

enum class ProtocolVersion : uint16_t {
	Unknown = 0x00,
	IO = 0x10,
	II = 0x11,
	ZO = 0x20,
};

enum class Method : uint16_t { Unknown = 0x00, Get = 0x07, Head, Post, Put, Delete, Connect, Options, Trace, Patch };

const std::string& protocolversion2string(ProtocolVersion version);

ProtocolVersion string2protocolversion(const std::string&);

Method string2method(const std::string&);

const std::string& method2string(Method method);

class Request : public Message {
   private:
	Method _method = Method::Unknown;
	std::string _rawpath;
	ProtocolVersion _protocolversion = ProtocolVersion::Unknown;

	bool fl1(const std::string& v) override {
		this->_method = string2method(v);
		return this->_method != Method::Unknown;
	}
	bool fl2(const std::string& v) override {
		this->_rawpath = v;
		return true;
	}
	bool fl3(const std::string& v) override {
		this->_protocolversion = string2protocolversion(v);
		return this->_protocolversion != ProtocolVersion::Unknown;
	}

   public:
	Request(size_t rsize, size_t wsize) : Message(rsize, wsize) {}

	void clear() override {
		Message::clear();
		this->_method = Method::Unknown;
		this->_rawpath.clear();
		this->_protocolversion = ProtocolVersion::Unknown;
	}
};

}  // namespace mycs::http