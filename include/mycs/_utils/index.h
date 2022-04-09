//
// Created by ztk on 2022/4/8.
//

#pragma once

#include <fmt/core.h>

#include <iostream>

#define Fmtp(...) fmt::print(__VA_ARGS__)

#ifdef _WINDOWS
#define InitConsole() SetConsoleOutputCP(CP_UTF8)
#else
#define InitConsole()
#endif