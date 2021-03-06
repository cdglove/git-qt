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

include("contrib/hunter_gate/cmake/HunterGate.cmake")
HunterGate(
    URL "https://github.com/cdglove/hunter/archive/custom_boost.tar.gz"
    SHA1 "822f6123389ce426cd3f3735606a0a4f08cc8d4e"
    FILEPATH "${CMAKE_SOURCE_DIR}/cmake/hunter_config.cmake"
)

project(git-qt VERSION 0.1 LANGUAGES C CXX)

set(Boost_USE_MULTI_THREADED ON)
set(Boost_USE_STATIC_LIBS ON)

hunter_add_package(Boost)
hunter_add_package(JsonSpirit)

if(NOT MSVC)
	hunter_add_package(Qt)
endif()


add_subdirectory(contrib/daily/future EXCLUDE_FROM_ALL)
add_subdirectory(contrib/cppgit)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find includes in corresponding build directories, needed
# to find the automoc'ed files.
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run uic automatically when needed.
#set(CMAKE_AUTOUIC ON)
find_package(Qt5Widgets REQUIRED)
find_package(Boost REQUIRED timer chrono)

set(CMAKE_CXX_STANDARD 14)
file(GLOB_RECURSE PROJECT_SOURCES src/*.cpp)
file(GLOB_RECURSE PROJECT_HEADERS src/*.hpp)
file(GLOB_RECURSE PROJECT_FORMS forms/*.ui)

list(APPEND ALL_SOURCES ${PROJECT_SOURCES})
list(APPEND ALL_SOURCES ${PROJECT_HEADERS})
list(APPEND ALL_SOURCES ${PROJECT_FORMS})

source_group(TREE "${PROJECT_SOURCE_DIR}" FILES ${ALL_SOURCES})

QT5_WRAP_UI(UIS_HDRS ${PROJECT_FORMS})

add_executable(
	git-qt WIN32 
	${ALL_SOURCES}
)

target_include_directories(
	git-qt PRIVATE src contrib/boost.nowide/include
)

target_link_libraries(
	git-qt
	Qt5::Widgets 
	Qt5::Core
	cppgit
	Boost::timer
	Boost::chrono
)

if(MINGW)
	set_target_properties(git-qt PROPERTIES LINK_FLAGS "-static")
endif()