cmake_minimum_required(VERSION 3.10)
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_PATH}/scripts/buildsystems/vcpkg.cmake")

project(mycs)
set(CMAKE_CXX_STANDARD 20)

if (${MSVC})
	add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
	add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
	add_definitions(-D_WIN32_WINNT=0x0601)
endif ()

set(Libs "")

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	add_definitions(-DCS_DEBUG)
	find_package(GTest CONFIG REQUIRED)
	set(Libs "${Libs};GTest::gmock;GTest::gtest;GTest::gmock_main;GTest::gtest_main")
endif ()

find_package(fmt CONFIG REQUIRED)
set(Libs "${Libs};fmt::fmt")
find_package(asio CONFIG REQUIRED)
set(Libs "${Libs};asio;asio::asio")
add_definitions(-DASIO_NO_DEPRECATED)
find_package(ZLIB REQUIRED)
set(Libs "${Libs};ZLIB")

function(cs_link_libs name)
	target_link_libraries(${name} PUBLIC ${Libs})
endfunction()

add_subdirectory(./tests)
