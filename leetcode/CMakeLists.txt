aux_source_directory(. problems)

foreach (item ${problems})
	get_filename_component(name ${item} NAME_WE)
	set(name "leetcode_${name}")
	add_executable(${name} "${item};${commons}")
endforeach ()
