//
// Created by ztk on 2022/4/4.
//

#include <mycs.hpp>

using namespace mycs::json;

int main() {
	Encoder encoder(std::cout, true);
	MapValue mv;
	mv.insert("xa", new NumberValue(12));
	auto av = new ArrayValue();
	av->push(new NumberValue(3.8123));
	av->push(False);
	av->push(Null);
	mv.insert("c", av);
	mv.insert("b", new StringValue("hello"));

	encoder.encode(mv);
	std::cout << std::endl;

	auto obj = peek(&mv, "c", 1);
	Fmtp("{}\r\n", obj == False);
	return 0;
}