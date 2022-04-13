//
// Created by ztk on 2022/4/8.
//

#pragma once

#include <fmt/core.h>

#include <functional>
#include <iostream>
#include <utility>

#define Fmtp(...) fmt::print(__VA_ARGS__)

#ifdef _WINDOWS
#define InitConsole() SetConsoleOutputCP(CP_UTF8)
#else
#define InitConsole()
#endif

namespace mycs::_utils {

class Defer {
   private:
	std::function<void()> fn;

   public:
	explicit Defer(std::function<void()> fn) : fn(std::move(fn)) {}

	virtual ~Defer() { fn(); }
};

}  // namespace mycs::_utils
