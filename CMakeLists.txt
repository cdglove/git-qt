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
    URL "https://github.com/ruslo/hunter/archive/v0.19.14.tar.gz"
    SHA1 "d4216c82be9c273ce2567b35f75356da045b884f"
    FILEPATH "contrib/hunter_config.cmake"
)

project(git-qt)

#set(Boost_USE_STATIC_LIBS ON)

add_subdirectory(contrib/daily/future)
add_subdirectory(contrib/cppgit)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find includes in corresponding build directories, needed
# to find the automoc'ed files.
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run uic automatically when needed.
#set(CMAKE_AUTOUIC ON)
# doesnt work with vs2017 yet.
#hunter_add_package(Qt)
find_package(Qt5Widgets REQUIRED)

hunter_add_package(Boost COMPONENTS timer chrono)
find_package(Boost CONFIG REQUIRED timer chrono)

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

