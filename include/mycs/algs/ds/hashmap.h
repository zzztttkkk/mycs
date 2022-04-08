//
// Created by ztk on 2022/4/8.
//

#pragma once
#include <functional>
#include <optional>

#include "./drray.h"

namespace mycs::algs {

template <typename K, typename V, typename Hash = std::hash<K>>
class Hashmap {
   private:
   public:
	size_t size() const;

	void insert(const K& key, const V& val);

	std::optional<V*> find(const K& key);

	std::optional<const V*> find(const K& key) const;
};

}  // namespace mycs::algs