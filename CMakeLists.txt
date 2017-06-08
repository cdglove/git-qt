# *****************************************************************************
# 
# CMakeLists.txt
# 
# Copyright Chris Glover 2017
# 
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt
# 
# *****************************************************************************

cmake_minimum_required(VERSION 3.8)

project(git-qt)

set(Boost_USE_STATIC_LIBS ON)

add_subdirectory(contrib/daily/future)
add_subdirectory(contrib/cppgit)
add_subdirectory(contrib/json_spirit)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find includes in corresponding build directories, needed
# to find the automoc'ed files.
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run uic automatically when needed.
#set(CMAKE_AUTOUIC ON)

find_package(Qt5Widgets REQUIRED)

find_package(Boost REQUIRED
	timer chrono)

set(CMAKE_CXX_STANDARD 14)
file(GLOB_RECURSE PROJECT_SOURCES src/*.cpp)
file(GLOB_RECURSE PROJECT_HEADERS src/*.hpp)
file(GLOB_RECURSE PROJECT_FORMS "forms/*.ui")

QT5_WRAP_UI(UIS_HDRS ${PROJECT_FORMS})

add_executable(
	git-qt WIN32 
	${PROJECT_SOURCES}
	${PROJECT_HEADERS} 
	${PROJECT_FORMS} 
)

target_include_directories(
	git-qt PRIVATE src 
)

target_link_libraries(
	git-qt
	Qt5::Widgets 
	Qt5::Core
	cppgit
	${Boost_LIBRARIES}
)

