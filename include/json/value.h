//
// Created by ztk on 2022/4/4.
//

#pragma once

#include <cmath>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace mycs::json {

enum class Type : unsigned char {
	Unknown = 0,
	String,
	Bool,
	Number,
	Array,
	Map,
	None,
};

class StringValue;

class NumberValue;

class ArrayValue;

class MapValue;

namespace {
class NoneValue;
class BoolValue;
}  // namespace

class Decoder;
class Encoder;

class Value {
   protected:
	Type t;

	explicit Value(Type t) : t(t) {}

	static void free_value(Value*);

   public:
	[[nodiscard]] inline Type type() const { return t; }

#define MakeJsonValueTypeCastMethod(T, n)                                       \
	[[nodiscard]] virtual const T& n() const { throw std::runtime_error(""); }; \
	virtual T& n() { throw std::runtime_error(""); }
	MakeJsonValueTypeCastMethod(StringValue, string);
	MakeJsonValueTypeCastMethod(NumberValue, number);
	MakeJsonValueTypeCastMethod(BoolValue, boolean);
	MakeJsonValueTypeCastMethod(ArrayValue, array);
	MakeJsonValueTypeCastMethod(MapValue, map);
#undef MakeJsonValueTypeCastMethod
};

class StringValue : public Value {
   private:
	friend class Decoder;
	friend class Encoder;

	std::string _data;

   public:
	StringValue() : Value(Type::String){};
	explicit StringValue(const std::string& v) : Value(Type::String) { this->_data = v; }
	StringValue(const char* p, size_t s) : Value(Type::String) { this->_data.assign(p, s); }
	[[nodiscard]] const StringValue& string() const override { return *this; }
	StringValue& string() override { return *this; }
};

class NumberValue : public Value {
   private:
	friend class Decoder;
	friend class Encoder;

	double _data = 0;

   public:
	NumberValue() : Value(Type::Number) {}
	explicit NumberValue(double v) : Value(Type::Number) { _data = v; }
	[[nodiscard]] const NumberValue& number() const override { return *this; }
	NumberValue& number() override { return *this; }

	[[nodiscard]] int64_t integer() const { return (int64_t)(std::trunc(_data)); }
};

class ArrayValue : public Value {
   private:
	friend class Decoder;
	friend class Encoder;
	typedef std::vector<Value*> Vec;

	Vec _data;

	template <class VecIter, bool IsConst>
	class IterBase {
	   private:
		VecIter raw;

	   public:
		explicit IterBase(VecIter raw) : raw(std::move(raw)) {}

		IterBase& operator++() {
			this->raw++;
			return *this;
		}

		IterBase& operator--() {
			this->raw--;
			return *this;
		}

		IterBase& operator+(int n) {
			this->raw += n;
			return *this;
		}

		IterBase& operator-(int n) {
			this->raw -= n;
			return *this;
		}

		bool operator==(const VecIter& another) const { return this->raw == another.raw; }

		template <bool WasConst = IsConst, typename = std::enable_if<WasConst>>
		const Value* operator*() {
			return this->raw.operator*();
		}

		template <bool WasConst = IsConst, typename = std::enable_if<!WasConst>>
		Value* operator*() {
			return this->raw.operator*();
		}
	};

   public:
	ArrayValue() : Value(Type::Array) {}
	virtual ~ArrayValue() {
		for (auto item : _data) Value::free_value(item);
	}

	[[nodiscard]] const ArrayValue& array() const override { return *this; }
	ArrayValue& array() override { return *this; }

	void push(Value* v) { _data.push_back(v); }

	Value* at(size_t idx) { return _data.at(idx); }

	[[nodiscard]] const Value* at(size_t idx) const { return _data.at(idx); }

	[[nodiscard]] size_t size() const { return _data.size(); }

	typedef IterBase<Vec::iterator, false> Iter;
	typedef IterBase<Vec::reverse_iterator, false> RIter;
	typedef IterBase<Vec::const_iterator, true> ConstIter;
	typedef IterBase<Vec::const_reverse_iterator, true> ConstRIter;

#define MakeIter(name, T) \
	T name() { return T(_data.name()); }

	MakeIter(begin, Iter);
	MakeIter(end, Iter);
	MakeIter(cbegin, ConstIter);
	MakeIter(cend, ConstIter);
	MakeIter(rbegin, RIter);
	MakeIter(rend, RIter);
	MakeIter(crbegin, ConstRIter);
	MakeIter(crend, ConstRIter);
#undef MakeIter
};

class MapValue : public Value {
   private:
	friend class Decoder;
	friend class Encoder;

	std::unordered_map<std::string, Value*> _data;

   public:
	MapValue() : Value(Type::Map) {}
	virtual ~MapValue() {
		for (const auto& pair : _data) Value::free_value(pair.second);
	}

	[[nodiscard]] const MapValue& map() const override { return *this; }
	MapValue& map() override { return *this; }

	void insert(const std::string& key, Value* val) { _data.insert(std::make_pair(key, val)); }

	void erase(const std::string& key) { _data.erase(key); }

	[[nodiscard]] size_t size() const { return _data.size(); }
};

namespace {

class NoneBoolNew;

class NoneValue : public Value {
   private:
	friend class NoneBoolNew;
	NoneValue() : Value(Type::None) {}

   public:
	inline void operator delete(void*) {}  // NOLINT
};

class BoolValue : public Value {
   private:
	friend class NoneBoolNew;
	bool _data = false;
	BoolValue() : Value(Type::Bool) {}
	explicit BoolValue(bool v) : Value(Type::Bool) { _data = v; }

   public:
	explicit operator bool() const { return _data; }

	inline void operator delete(void*) {}  // NOLINT
};

class NoneBoolNew {
   public:
	static NoneValue* none() { return new NoneValue(); }
	static BoolValue* boolean(bool v) { return new BoolValue(v); }
};

}  // namespace

static NoneValue* const None = NoneBoolNew::none();			  // NOLINT
static BoolValue* const True = NoneBoolNew::boolean(true);	  // NOLINT
static BoolValue* const False = NoneBoolNew::boolean(false);  // NOLINT

}  // namespace mycs::json