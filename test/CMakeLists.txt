# xxx
# (C) 2018-, Mura, All rights reserved.

cmake_minimum_required (VERSION 3.8)

add_executable				(test)
target_sources				(test	PRIVATE
	main.cxx
)
target_compile_features		(test	PRIVATE		cxx_std_17)
target_compile_options		(test	PRIVATE		)
target_include_directories	(test	PRIVATE		".")
target_link_libraries		(test	PRIVATE		xxx)
