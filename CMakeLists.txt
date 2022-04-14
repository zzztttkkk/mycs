cmake_minimum_required(VERSION 3.10)

project(mycs)
set(CMAKE_CXX_STANDARD 20)

set(Libs "")

if (${MSVC})
	add_compile_options(/utf-8 /wd4005)
	add_definitions(-D_WIN32_WINNT=0x0601)
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
	add_compile_options(-fcoroutines)
	set(Libs "${Libs};pthread")
endif ()

find_package(fmt CONFIG REQUIRED)
set(Libs "${Libs};fmt::fmt")
find_package(asio CONFIG REQUIRED)
set(Libs "${Libs};asio;asio::asio")
add_definitions(-DASIO_NO_DEPRECATED)
add_definitions(-DZLIB_COMPAT)
find_package(ZLIB REQUIRED)
set(Libs "${Libs};ZLIB::ZLIB")

function(cs_link_libs name)
	target_link_libraries(${name} PUBLIC ${Libs})
endfunction()

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	add_definitions(-DMYCS_DEBUG)
	find_package(GTest CONFIG REQUIRED)
	set(Libs "${Libs};GTest::gmock;GTest::gtest;GTest::gmock_main;GTest::gtest_main")

	add_subdirectory(./tests)
endif ()