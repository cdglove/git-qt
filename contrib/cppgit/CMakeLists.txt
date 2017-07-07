# *****************************************************************************
# 
# cppgit/CMakeLists.txt
# 
# Copyright Chris Glover 2017
# 
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt
# 
# *****************************************************************************
cmake_minimum_required(VERSION 3.8)

project(cppgit VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

hunter_add_package(JsonSpirit)
find_package(JsonSpirit)

hunter_add_package(Boost COMPONENTS system filesystem)
find_package(Boost REQUIRED system filesystem)

file(GLOB_RECURSE PROJECT_SOURCES src/*.cpp src/*.hpp)
file(GLOB_RECURSE PROJECT_HEADERS include/*.hpp)

list(APPEND ALL_SOURCES ${PROJECT_SOURCES})
list(APPEND ALL_SOURCES ${PROJECT_HEADERS})

source_group(TREE "${PROJECT_SOURCE_DIR}" FILES ${ALL_SOURCES})

add_library(
	cppgit
	${ALL_SOURCES}
)

target_include_directories(
	cppgit PRIVATE src 
)

target_include_directories(
	cppgit PUBLIC include 
)

target_link_libraries(
	cppgit PUBLIC
	daily_future
	Boost::system
	Boost::filesystem
	json
)

if (WIN32)
	target_compile_definitions(
		cppgit PUBLIC
		_WIN32_WINNT=0x0601
		WINVER=0x0601
	)

	if(MSVC)
		target_compile_definitions(
			cppgit PUBLIC
			_SCL_SECURE_NO_WARNINGS=1
		)
	endif()
endif(WIN32)

target_compile_definitions(
	cppgit PUBLIC
	BOOST_THREAD_PROVIDES_FUTURE
)

#include( CTest )
option(CPPGIT_BUILD_TESTS "Build the entity project tests." ON)
if(CPPGIT_BUILD_TESTS)
	enable_testing()
	add_subdirectory(test)
endif()