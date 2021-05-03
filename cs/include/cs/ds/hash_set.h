#pragma once

#include "../common/index.h"
#include "./hash_map.h"

namespace cs {

namespace {
struct SetV {
};
}  // namespace

template<typename T, typename Hash = std::hash<T>, typename Equal = std::equal_to<T>, size_t InitCap = 12>
class HashSet {
private:
	typedef HashMap <T, SetV, Hash, Equal, InitCap> MapT;
	MapT* map = nullptr;

public:
	class Iterator {
	private:
		typename MapT::ConstIterator mi;

	public:
		explicit Iterator(typename MapT::ConstIterator&& m) : mi(m) {}

		~Iterator() = default;

		Iterator& operator++() {
			++mi;
			return *this;
		}

		const T& operator*() {
			const auto node = *mi;
			return node->key();
		}

		bool operator!=(const Iterator& other) const { return this->mi != other.mi; }
	};

	HashSet() = default;

	virtual ~HashSet() { delete map; }

	size_t size() { return map == nullptr ? 0 : map->size(); }

	bool empty() { return map == nullptr || map->empty(); }

	void add(const T& val) {
		if (map == nullptr) map = new MapT();
		map->set(val, SetV{});
	}

	void del(const T& val) {
		if (map == nullptr) return;
		map->del(val);
	}

	bool has(const T& val) {
		if (map == nullptr) return false;
		return map->get(val).updated;
	}

	Iterator begin() const { return Iterator(const_cast<const MapT*>(map)->begin()); }

	Iterator end() const { return Iterator(const_cast<const MapT*>(map)->end()); }
};

}  // namespace cs