# Default Auxiliary CMakeLists definition

add_library(aux_files
	auxcode/auxcode.c
)
target_link_libraries(aux_files 
	pico_stdlib
)
