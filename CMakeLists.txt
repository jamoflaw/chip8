###########################################################################
# This file is part of the Evolution Engine.
#
#    Evolution is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    Foobar is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
############################################################################
cmake_minimum_required(VERSION 2.8.10)

# DEFAULTS
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES  ON)

# Set Output Directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/bin)

# Set Additional Module Path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/Modules.cmake/")

# Check for C++11 Support
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fstack-protector-all")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -fstack-protector-all")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()


if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(SEND_ERROR "In-source builds are not allowed.")
endif ()


#set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)

# Set Our Global Log Dir
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/build/lib)

option(EVO_MEMDEBUG "Enable Memory debugging with ASAN" OFF)
if(EVO_MEMDEBUG)
  set (EVO_ASAN_FLAGS "-fsanitize=address -fno-omit-frame-pointer")
else()
  set (EVO_ASAN_FLAGS "")
endif()

option(EVO_DEBUGSYMBOLS "Enable debugging symbols" OFF)
if(EVO_DEBUGSYMBOLS)
  set (EVO_DEBUGSYMBOLS_FLAGS "-g")
else()
  set (EVO_DEBUGSYMBOLS_FLAGS "")
endif()

option(EVO_VERBOSEWARNINGS "Enable Verbose Warnings during compilation" OFF)
if(EVO_VERBOSEWARNINGS)
  set (EVO_VERBOSEWARNINGS_FLAGS "-Wall -Wextra")
else()
  set (EVO_VERBOSEWARNINGS_FLAGS "")
endif()

# Setup the CMAKE C++ Compilation flags
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EVO_VERBOSEWARNINGS_FLAGS} ${EVO_DEBUGSYMBOLS_FLAGS} ${EVO_ASAN_FLAGS}")


# Variables
project (Chip8)

# Set Link Dependencies
#set(linkdepend  boost_thread pthread boost_filesystem physfs enet glfw GL GLEW openal ogg vorbisfile theoradec theora vorbis png freetype pugixml boost_regex)

if(EVO_MEMDEBUG)
  set(linkdepend ${linkdepend} asan)
endif()

# Project Information
message(STATUS "Configuring ${PROJECT_NAME}...")

# Find Files
file(GLOB CPP_FILES *.cpp)
add_executable(${PROJECT_NAME} EXCLUDE_FROM_ALL ${CPP_FILES})

# Include Dir
include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Linker Libs
foreach(lib ${linkdepend})
  target_link_libraries(${PROJECT_NAME} ${lib})
endforeach(lib)




