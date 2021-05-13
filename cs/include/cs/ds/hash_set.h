#pragma once

#include "../common/index.h"
#include "./hash_table.h"

namespace cs {

namespace {
struct SetV {
};
}  // namespace

template<typename T, typename Hash = std::hash<T>, typename Equal = std::equal_to<T>, size_t InitCap = 12>
class HashSet {
private:
	typedef HashTable <T, SetV, Hash, Equal, InitCap> MapT;
	MapT* map = nullptr;

public:
	class ConstIterator {
	private:
		typename MapT::ConstIterator mi;

	public:
		explicit ConstIterator(typename MapT::ConstIterator&& m) : mi(m) {}

		~ConstIterator() = default;

		ConstIterator& operator++() {
			++mi;
			return *this;
		}

		const T& operator*() {
			const auto node = *mi;
			return node->key();
		}

		bool operator!=(const ConstIterator& other) const { return this->mi != other.mi; }
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

	ConstIterator begin() const { return ConstIterator(const_cast<const MapT*>(map)->begin()); }

	ConstIterator end() const { return ConstIterator(const_cast<const MapT*>(map)->end()); }
};

}  // namespace cs