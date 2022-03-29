file(GLOB_RECURSE all_cs_source ../src/**/*.cpp;../src/*.cpp)
file(GLOB_RECURSE all_test *.cpp;**/*.cpp)

foreach (item ${all_test})
	get_filename_component(name ${item} NAME_WE)
	get_filename_component(dir ${item} DIRECTORY)
	get_filename_component(dir ${dir} NAME)

	set(name "${dir}_${name}")
	add_executable(${name} "${item};${all_cs_source}")
	target_include_directories(${name} PUBLIC "../include")
	cs_link_libs(${name})
endforeach ()