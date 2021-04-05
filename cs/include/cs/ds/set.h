#pragma once

#include "../common/index.h"
#include "./map.h"

namespace cs {

namespace {
struct SetV {};
}  // namespace

template <typename T, typename Hash = std::hash<T>, typename Equal = std::equal_to<T>, size_t InitCap = 12>
class Set {
   private:
	typedef Map<T, SetV, Hash, Equal, InitCap> MapT;
	MapT* map = nullptr;

   public:
	class Iterator {
	   private:
		typename MapT::Iterator mi;
		bool empty = false;

	   public:
		explicit Iterator(typename MapT::Iterator& m, bool e) : mi(m), empty(e) {}

		~Iterator() = default;

		Iterator& operator++() {
			++mi;
			return *this;
		}

		const T& operator*() {
			typename MapT::Node* node = *mi;
			return node->key();
		}

		bool operator!=(const Iterator& other) const {
			if (empty && other.empty) return false;
			return this->mi != other.mi;
		}
	};

	Set() = default;
	virtual ~Set() { delete map; }

	size_t size() { return map == nullptr ? 0 : map->size(); }

	bool empty() { return map == nullptr || map->empty(); }

	void add(const T& val) {
		if (map == nullptr) map = new MapT;
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

	Iterator begin() {
		if (map == nullptr) return Iterator(MapT::Iterator(nullptr, 0), true);
		return Iterator(map->begin(), false);
	}

	Iterator end() {
		if (map == nullptr) return Iterator(MapT::Iterator(nullptr, 0), true);
		return Iterator(map->end(), false);
	}

	Iterator begin() const {
		if (map == nullptr) return Iterator(MapT::Iterator(nullptr, 0), true);
		auto cm = const_cast<MapT*>(map);
		return Iterator(cm->begin(), false);
	}

	Iterator end() const {
		if (map == nullptr) return Iterator(MapT::Iterator(nullptr, 0), true);
		auto cm = const_cast<MapT*>(map);
		return Iterator(cm->end(), false);
	}
};

}  // namespace cs