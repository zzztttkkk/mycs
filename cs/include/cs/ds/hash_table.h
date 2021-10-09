#pragma once

#include "../common/index.h"
#include "./drray.h"
#include "./linkedlist.h"

namespace cs {

template <typename K, typename V, typename Hash = std::hash<K>, typename Equal = std::equal_to<K>, size_t InitCap = 12>
class HashTable {
   public:
	class Node;

	typedef LinkedList<Node*> List;
	typedef Drray<List*> Data;

	class Node {
	   private:
		friend class HashTable<K, V, Hash, Equal, InitCap>;

		K _key;
		V _val;
		size_t hash;

	   public:
		Node(const K& k, const V& v, size_t h) : _key(k), _val(v), hash(h) {}

		virtual ~Node() = default;

		const K& key() const { return _key; }

		V& val() { return _val; }

		const V& val() const { return _val; }
	};

	class MissingKey : public std::exception {
	   private:
		std::string v;

	   public:
		explicit MissingKey(const K& key) { v = fmt::format("Missing Key: {}", key); }

		[[nodiscard]] char const* what() const override { return v.c_str(); }
	};

   private:
	typedef HashTable<K, V, Hash, Equal, InitCap> ThisType;
	typedef typename List::Iterator ListIter;

	Data* data = nullptr;
	int factor = 80;
	size_t seed = 0;
	Hash hash;
	size_t _size = 0;

	size_t calc_hash(const K& key) const { return hash(key) + seed; }

	void allocate() {
		auto oldData = data;
		data = new Drray<List*>;
		data->reserve(oldData->cap() << 1);
		data->resize(data->cap());
		data->fill_zero();

		for (List* list : *oldData) {
			if (list == nullptr) continue;
			for (Node* node : *list) append_node(node);
			delete list;
		}
		delete oldData;
	}

	Node* do_find(const K& key, size_t hv) const {
		if (empty()) return nullptr;

		Equal equal;
		size_t ind = hv % data->cap();
		List* list = data->at(ind);
		if (list == nullptr) return nullptr;
		for (Node* node : *list) {
			if (equal(node->_key, key)) return node;
		}
		return nullptr;
	}

	void append_node(Node* node) {
		if (data == nullptr) {
			data = new Drray<List*>;
			data->reserve(InitCap);
			data->resize(InitCap);
			data->fill_zero();
		}
		if (int(((_size + 1) * 100) / data->cap()) > factor) allocate();

		size_t ind = node->hash % data->cap();
		List* list = data->at(ind);
		if (list == nullptr) {
			list = new List;
			data->at(ind) = list;
		}
		list->push_back(node);
	}

	std::optional<std::reference_wrapper<V>> _find(const K& key) {
		auto hV = calc_hash(key);
		Node* node = do_find(key, hV);
		if (node == nullptr) {
			return {};
		}
		return node->_val;
	}

	void init() {
		static std::random_device srd;
		static std::uniform_int_distribution<> distribution(INT32_MIN, INT32_MAX);
		static std::mt19937 mt19937(static_cast<unsigned int>(distribution(srd)));
		// the same keys are kept in a different order in different maps
		this->seed = distribution(mt19937);
	}

	template <bool IsConst>
	class BaseIterator {
	   protected:
		ThisType* map = nullptr;
		size_t ind = 0;
		ListIter li;
		bool ok = false;

		void next() {
			if (ind >= map->data->size()) {
				return;
			}
			List* list = map->data->at(ind);
			if (ok) {
				++li;
				if (li != list->end()) return;
				ok = false;
				ind++;
				next();
				return;
			}

			while (list == nullptr) {
				ind++;
				if (ind >= map->data->size()) return;
				list = map->data->at(ind);
			}
			li = list->begin();
			ok = true;
		}

	   public:
		BaseIterator(ThisType* m, size_t i) : map(m), ind(i) {}

		~BaseIterator() = default;

		BaseIterator& operator++() {
			next();
			return *this;
		}

		bool operator!=(const BaseIterator& other) const { return this->ind != other.ind; }

		template <bool WasConst = IsConst, typename = typename std::enable_if<WasConst, void>::type>
		const Node* operator*() {
			return *(li);
		}

		template <bool WasConst = IsConst, typename = typename std::enable_if<WasConst, void>::type>
		const Node* operator->() {
			return *(li);
		}

		template <bool WasConst = IsConst, typename = typename std::enable_if<!WasConst, void>::type>
		Node* operator*() {
			return *(li);
		}

		template <bool WasConst = IsConst, typename = typename std::enable_if<!WasConst, void>::type>
		Node* operator->() {
			return *(li);
		}
	};

   public:
	typedef BaseIterator<true> ConstIterator;
	typedef BaseIterator<false> Iterator;
	typedef std::optional<const std::reference_wrapper<V>> ConstValRefOptional;
	typedef std::optional<std::reference_wrapper<V>> ValRefOptional;

	HashTable() { init(); };

	explicit HashTable(int load_factor) {
		if (load_factor < 20) load_factor = 20;
		this->factor = load_factor;
		init();
	}

	virtual ~HashTable() {
		if (data == nullptr) return;
		for (auto list : *data) {
			if (list == nullptr) continue;
			for (auto node : *list) delete node;
			delete list;
		}
		delete data;
	}

	[[nodiscard]] size_t size() const { return _size; }

	[[nodiscard]] bool empty() const { return _size == 0; }

	std::optional<V> set(const K& key, const V& val) {
		size_t hash_val = calc_hash(key);
		Node* node = do_find(key, hash_val);
		if (node != nullptr) {
			V old_val = node->_val;
			node->_val = val;
			return {old_val};
		}
		node = new Node(key, val, hash_val);
		append_node(node);
		_size++;
		return {};
	}

	std::optional<V> replace(const K& key, const V& val) {
		size_t hV = calc_hash(key);
		Node* node = do_find(key, hV);
		if (node == nullptr) {
			return {};
		}
		V old_val = node->_val;
		node->_val = val;
		return old_val;
	}

	inline ValRefOptional find(const K& key) { return _find(key); }

	ConstValRefOptional find(const K& key) const {
		ValRefOptional&& opt = const_cast<ThisType*>(this)->_find(key);
		if (opt.has_value()) {
			return opt.value();
		}
		return {};
	}

	bool contains(const K& key) { return find(key, calc_hash(key)).has_value(); }

	V& at(const K& key) {
		auto opt = find(key);
		if (opt.has_value()) {
			return opt.value();
		}
		throw MissingKey(key);
	}

	const V& at(const K& key) const {
		auto opt = find(key);
		if (opt.has_value()) {
			return opt.value();
		}
		throw MissingKey(key);
	}

	std::optional<V> del(const K& key) {
		if (empty()) return {};

		auto hV = calc_hash(key);
		List* list = data->at(hV % data->cap());
		if (list == nullptr) return {};

		V val;
		if (list->remove_if(
				[&key, &val](auto node) -> bool {
					bool e = Equal{}(node->_key, key);
					if (e) {
						val = node->_val;
						delete node;
					}
					return e;
				},
				1) == 1) {
			_size--;
		}
		return val;
	}

	typedef std::function<bool(const K& key, V& val)> ItemVisitorFunc;

	void items(const ItemVisitorFunc& func) {
		if (empty()) return;
		for (auto list : *data) {
			if (list == nullptr) continue;
			for (Node* node : *list) {
				if (!func(node->_key, node->_val)) return;
			}
		}
	}

	typedef std::function<bool(const K& key, const V& val)> ConstItemVisitorFunc;

	void items(const ConstItemVisitorFunc& func) const {
		if (empty()) return;
		for (auto list : *data) {
			if (list == nullptr) continue;
			for (Node* node : *list) {
				if (!func(node->_key, node->_val)) return;
			}
		}
	}

	typedef std::function<bool(const K& key)> KeyVisitorFunc;

	void keys(const KeyVisitorFunc& func) const {
		if (empty()) return;
		for (auto list : *data) {
			if (list == nullptr) continue;
			for (Node* node : *list) {
				if (!func(node->_key)) return;
			}
		}
	}

	typedef std::function<bool(const V& val)> ConstValVisitorFunc;

	void values(const ConstValVisitorFunc& func) const {
		if (empty()) return;
		for (auto list : *data) {
			if (list == nullptr) continue;
			for (Node* node : *list) {
				if (!func(node->_val)) return;
			}
		}
	}

	typedef std::function<bool(V& val)> ValVisitorFunc;

	void values(const ValVisitorFunc& func) {
		if (empty()) return;
		for (auto list : *data) {
			if (list == nullptr) continue;
			for (Node* node : *list) {
				if (!func(node->_val)) return;
			}
		}
	}

	Iterator begin() {
		if (empty()) return end();
		return ++Iterator(this, 0);
	}

	Iterator end() {
		if (this->data == nullptr) return Iterator(nullptr, 0);
		return Iterator(this, this->data->size());
	}

	ConstIterator begin() const {
		if (empty()) return end();
		return ++ConstIterator(const_cast<ThisType*>(this), 0);
	}

	ConstIterator end() const {
		if (this->data == nullptr) return ConstIterator(nullptr, 0);
		return ConstIterator(const_cast<ThisType*>(this), this->data->size());
	}

	inline V& operator[](const K& key) { return at(key); }

	inline const V& operator[](const K& key) const { return at(key); }
};

}  // namespace cs