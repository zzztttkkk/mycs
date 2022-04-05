//
// Created by ztk on 2022/4/4.
//

#pragma once

#include <cmath>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
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

   public:
	[[nodiscard]] inline Type type() const { return t; }
	[[nodiscard]] bool is_none() const;

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

	std::vector<std::shared_ptr<Value>> _data;

   public:
	ArrayValue() : Value(Type::Array) {}
	[[nodiscard]] const ArrayValue& array() const override { return *this; }
	ArrayValue& array() override { return *this; }

	void push(Value* v) { _data.push_back(std::shared_ptr<Value>(v)); }

	void push(const std::shared_ptr<Value>& v) { _data.push_back(v); }

	std::shared_ptr<Value> at(size_t idx) { return std::move(_data.at(idx)); }

	[[nodiscard]] std::shared_ptr<const Value> at(size_t idx) const { return _data.at(idx); }

	[[nodiscard]] size_t size() const { return _data.size(); }
};

class MapValue : public Value {
   private:
	friend class Decoder;
	friend class Encoder;

	std::unordered_map<std::string, std::shared_ptr<Value>> _data;

   public:
	MapValue() : Value(Type::Map) {}
	[[nodiscard]] const MapValue& map() const override { return *this; }
	MapValue& map() override { return *this; }

	void insert(const std::string& key, Value* val) { _data.insert(std::make_pair(key, std::shared_ptr<Value>(val))); }

	void insert(const std::string& key, const std::shared_ptr<Value>& val) { _data.insert(std::make_pair(key, val)); }

	void erase(const std::string& key) { _data.erase(key); }

	[[nodiscard]] size_t size() const { return _data.size(); }
};

namespace {
class NoneValue : public Value {
   public:
	NoneValue() : Value(Type::None) {}
};

class BoolValue : public Value {
   private:
	bool _data = false;

   public:
	BoolValue() : Value(Type::Bool) {}

	explicit BoolValue(bool v) : Value(Type::Bool) { _data = v; }

	explicit operator bool() const { return false; }
};

}  // namespace

const static Value None = NumberValue();		   // NOLINT
const static Value True = BoolValue(true);		   // NOLINT
const static Value* False = new BoolValue(false);  // NOLINT

}  // namespace mycs::json