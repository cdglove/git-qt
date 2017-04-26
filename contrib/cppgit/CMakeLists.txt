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

set(CMAKE_CXX_STANDARD 14)

find_package(Boost REQUIRED)
file(GLOB_RECURSE PROJECT_SOURCES src/*.cpp)
file(GLOB_RECURSE PROJECT_HEADERS src/*.hpp include/*.hpp)

add_library(
	cppgit
	${PROJECT_SOURCES}
	${PROJECT_HEADERS} 
)

target_include_directories(
	cppgit PRIVATE src 
)

target_include_directories(
	cppgit PUBLIC include 
)

target_include_directories(
	cppgit PUBLIC
	${Boost_INCLUDE_DIRS}
)

if (WIN32)
	target_compile_definitions(
		cppgit PRIVATE
		_WIN32_WINNT=0x0601
		WINVER=0x0601
	)
endif(WIN32)