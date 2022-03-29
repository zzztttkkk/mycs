//
// Created by ztk on 2022/3/29.
//

#pragma once

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
};

class Message {
	std::vector<BYTE> fl1;
	std::vector<BYTE> fl2;
	std::vector<BYTE> fl3;
};

}