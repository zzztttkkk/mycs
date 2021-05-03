#pragma once

#include "../common/index.h"
#include "./drray.h"
#include "./linkedlist.h"
#include <ctime>

namespace cs {

template<typename K, typename V, typename Hash = std::hash<K>, typename Equal = std::equal_to<K>, size_t InitCap = 12>
class HashMap {
public:
	class Node;

	typedef LinkedList<Node*> List;
	typedef Drray<List*> Data;

	class Node {
	private:
		friend class HashMap<K, V, Hash, Equal, InitCap>;

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

private:
	typedef HashMap<K, V, Hash, Equal, InitCap> ThisType;
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

	Node* _find(const K& key, size_t hv) const {
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

	Node* set(const K& key, const V& val, V* old_val_ptr, bool* is_update) {
		size_t hash_val = calc_hash(key);
		Node* node = _find(key, hash_val);
		if (node != nullptr) {
			*old_val_ptr = node->_val;
			*is_update = true;
			node->_val = val;
			return node;
		}

		*is_update = false;
		node = new Node(key, val, hash_val);
		append_node(node);
		_size++;
		return node;
	}

	void init() {
		static std::random_device srd;
		static std::uniform_int_distribution<> distribution(INT32_MIN, INT32_MAX);
		static std::mt19937 mt19937(static_cast<unsigned int>(distribution(srd)));
		// the same keys are kept in a different order in different maps
		this->seed = distribution(mt19937);
	}

	template<bool IsConst>
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

		template<bool WasConst = IsConst, typename = typename std::enable_if<WasConst, void>::type>
		const Node* operator*() { return *(li); }

		template<bool WasConst = IsConst, typename = typename std::enable_if<WasConst, void>::type>
		const Node* operator->() { return *(li); }

		template<bool WasConst = IsConst, typename = typename std::enable_if<!WasConst, void>::type>
		Node* operator*() { return *(li); }

		template<bool WasConst = IsConst, typename = typename std::enable_if<!WasConst, void>::type>
		Node* operator->() { return *(li); }
	};

public:
	typedef BaseIterator<true> ConstIterator;
	typedef BaseIterator<false> Iterator;

	HashMap() { init(); };

	explicit HashMap(int load_factor) {
		if (load_factor < 20) load_factor = 20;
		this->factor = load_factor;
		init();
	}

	virtual ~HashMap() {
		if (data == nullptr) return;
		for (auto list : *data) {
			if (list == nullptr) continue;
			for (auto node : *list) delete node;
			delete list;
		}
		delete data;
	}

	size_t size() { return _size; }

	bool empty() { return _size == 0; }

	struct SetResult {
	public:
		bool updated;
		V old_val;
	};

	SetResult set(const K& key, const V& val) {
		SetResult result;
		set(key, val, &result.old_val, &result.updated);
		return result;
	}

	SetResult update_only(const K& key, const V& val) {
		SetResult result;
		size_t hV = calc_hash(key);
		Node* node = find(key, hV);
		if (node == nullptr) {
			result.updated = false;
			return result;
		}
		result.updated = true;
		result.old_val = node->_val;
		node->_val = val;
		return result;
	}

	struct FindResult {
	public:
		bool ok = false;
		V* ptr = nullptr;
	};

	FindResult find(const K& key) {
		FindResult result;
		auto hV = calc_hash(key);
		Node* node = _find(key, hV);
		if (node == nullptr) {
			result.ok = false;
			return result;
		}
		result.ok = true;
		result.ptr = &node->_val;
		return result;
	}

	struct ConstFindResult {
	public:
		bool ok = false;
		V* ptr = nullptr;
	};

	ConstFindResult find(const K& key) const {
		ConstFindResult result;
		auto hV = calc_hash(key);
		Node* node = find(key, hV);
		if (node == nullptr) {
			result.ok = false;
			return result;
		}
		result.ok = true;
		result.ptr = &node->_val;
		return result;
	}

	V& at(const K& key) {
		FindResult result = find(key);
		if (result.ok) return *result.ptr;
		throw std::runtime_error(fmt::format("cs.HashMap: missing key `{}`", key));
	}

	const V& at(const K& key) const {
		ConstFindResult result = find(key);
		if (result.ok) return *result.ptr;
		throw std::runtime_error(fmt::format("cs.HashMap: missing key `{}`", key));
	}

	[[nodiscard]] bool empty() const { return _size == 0; }

	bool contains(const K& key) { return find(key, calc_hash(key)) != nullptr; }

	SetResult del(const K& key) {
		SetResult result;
		result.updated = false;

		if (empty()) return result;

		auto hV = calc_hash(key);
		List* list = data->at(hV % data->cap());
		if (list == nullptr) return result;
		if (list->remove_if(
				[&key, &result](auto node) -> bool {
					bool e = Equal{}(node->_key, key);
					if (e) {
						result.updated = true;
						result.old_val = node->_val;
						delete node;
					}
					return e;
				},
				1) == 1) {
			_size--;
		}

		return result;
	}

	typedef std::function<bool(const K& key, const V& val)> ItemVisitorFunc;

	void items(const ItemVisitorFunc& func) {
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

	template<typename SeqContainer>
	void keys_to(SeqContainer& seq) const {
		keys([&seq](const K& key) -> bool {
			seq.push_back(key);
			return true;
		});
	}

	typedef std::function<bool(const V& val)> ValVisitorFunc;

	void values(const ValVisitorFunc& func) const {
		if (empty()) return;
		for (auto list : *data) {
			if (list == nullptr) continue;
			for (Node* node : *list) {
				if (!func(node->_val)) return;
			}
		}
	}

	template<typename SeqContainer>
	void values_to(SeqContainer& seq) const {
		values([&seq](const V& val) -> bool {
			seq.push_back(val);
			return true;
		});
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

	V& operator[](const K& key) {
		FindResult findResult = find(key);
		if (findResult.ok) return *findResult.ptr;
		SetResult setResult;
		return set(key, V{}, &setResult.old_val, &setResult.updated)->_val;
	}

	const V& operator[](const K& key) const {
		ConstFindResult result = find(key);
		if (result.ok) return *result.ptr;
		throw std::runtime_error(fmt::format("cs.HashMap: missing key `{}`", key));
	}
};

}  // namespace cs