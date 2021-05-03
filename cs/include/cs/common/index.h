#pragma once

#ifdef CS_DEBUG

#define TestMain                              \
	int main(int argc, char** argv) {         \
		testing::InitGoogleTest(&argc, argv); \
		return RUN_ALL_TESTS();               \
	}

#endif	// CS_DEBUG

#include <fmt/core.h>

#include <chrono>
#include <cstddef>
#include <cstring>
#include <exception>
#include <functional>
#include <iostream>
#include <optional>
#include <random>
#include <stdexcept>
#include <thread>

namespace cs {

#ifndef FMT
#ifdef _MSC_VER
#define FMT(f, ...) fmt::print(f, __VA_ARGS__)
#else
#define FMT(f, args...) fmt::print(f, ##args)
#endif
#endif	// FMT

int rand();

int rand(int min, int max);

// detect mem-leak directly
template <typename Callable, typename Clock = std::chrono::system_clock>
void run(const Callable& callable, std::chrono::duration<double> duration = std::chrono::minutes(5)) {
	auto begin = Clock::now();
	auto count = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
	while (true) {
		auto now = Clock::now();
		if ((std::chrono::duration_cast<std::chrono::milliseconds>(now - begin)).count() >= count) {
			return;
		}
		callable();
	}
}

template <typename T>
struct Comparator {
	constexpr int operator()(const T& a, const T& b) {
		if (std::equal_to<T>{}(a, b)) {
			return 0;
		}
		if (std::less<T>{}(a, b)) {
			return -1;
		}
		return 1;
	}
};

}  // namespace cs
