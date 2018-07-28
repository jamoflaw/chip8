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

option(MEMDEBUG "Enable Memory debugging with ASAN" OFF)
if(MEMDEBUG)
  set (ASAN_FLAGS "-fsanitize=address -fno-omit-frame-pointer")
else()
  set (ASAN_FLAGS "")
endif()

option(DEBUGSYMBOLS "Enable debugging symbols" OFF)
if(DEBUGSYMBOLS)
  set (DEBUGSYMBOLS_FLAGS "-g")
else()
  set (DEBUGSYMBOLS_FLAGS "")
endif()

option(VERBOSEWARNINGS "Enable Verbose Warnings during compilation" OFF)
if(VERBOSEWARNINGS)
  set (VERBOSEWARNINGS_FLAGS "-Wall -Wextra")
else()
  set (VERBOSEWARNINGS_FLAGS "")
endif()

# Setup the CMAKE C++ Compilation flags
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${VERBOSEWARNINGS_FLAGS} ${DEBUGSYMBOLS_FLAGS} ${ASAN_FLAGS}")


# Variables
project (Chip8)

# Set Link Dependencies
set(linkdepend glfw GL GLEW)

if(MEMDEBUG)
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




