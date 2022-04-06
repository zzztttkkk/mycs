//
// Created by ztk on 2022/4/4.
//

#include <json/encoder.h>

#include <iostream>

using namespace mycs::json;

int main() {
	Encoder encoder(std::cout);
	MapValue mv;
	mv.insert("a", new NumberValue(12));
	mv.insert("b", new StringValue("hello"));
	auto av = new ArrayValue();
	av->push(new NumberValue(3.8123));
	av->push(False);
	av->push(Null);
	mv.insert("c", av);

	encoder.encode(mv);
	std::cout << std::endl;
	return 0;
}