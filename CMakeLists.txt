# xxx
# (C) 2018-, Mura, All rights reserved.

cmake_minimum_required (VERSION 3.8)

project ("xxx")

cmake_policy(SET			CMP0076		NEW)	# converts relative paths to absolute

add_library					(xxx)
target_sources				(xxx	PRIVATE
	src/xxx.cxx
)
target_sources				(xxx	PUBLIC
	xxx/xxx.hxx
	xxx/exceptions.hxx
	xxx/finally.hxx
)
target_compile_features		(xxx	PUBLIC		cxx_std_17)
target_compile_options		(xxx	PUBLIC		)
target_include_directories	(xxx	PUBLIC		".")

add_subdirectory			(test)
