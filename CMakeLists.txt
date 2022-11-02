#**********************************************************************************
#  MIT License                                                                    *
#                                                                                 *
#  Copyright (c) 2022 Vahan Aghajanyan <vahancho@gmail.com>                       *
#                                                                                 *
#  Permission is hereby granted, free of charge, to any person obtaining a copy   *
#  of this software and associated documentation files (the "Software"), to deal  *
#  in the Software without restriction, including without limitation the rights   *
#  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      *
#  copies of the Software, and to permit persons to whom the Software is          *
#  furnished to do so, subject to the following conditions:                       *
#                                                                                 *
#  The above copyright notice and this permission notice shall be included in all *
#  copies or substantial portions of the Software.                                *
#                                                                                 *
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     *
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       *
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    *
#  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         *
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  *
#  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  *
#  SOFTWARE.                                                                      *
#**********************************************************************************

cmake_minimum_required (VERSION 3.16)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# The project definition
project(nkar VERSION 0.1 DESCRIPTION "The image comparison library" LANGUAGES CXX)

# Make cache variables for install destinations
include(GNUInstallDirs)

# General options
option(BUILD_SHARED_LIBS   "Build using shared libraries" ON)
option(BUILD_DOCUMENTATION "Build documentation"          ON)
option(ENABLE_TESTING      "Enable unit test build"       OFF)
option(ENABLE_EXAMPLES     "Enable examples build"        OFF)

if (MSVC)
    add_compile_options(/W3)
else()
    # Specify the target architecture (Linux). For Windows based generator use rather
    # '-A Win32' or '-A x64 options'
    set(TARGET_ARCH x86 CACHE STRING "the target architecture")
    set_property(CACHE TARGET_ARCH PROPERTY STRINGS x86 x64)

    if (TARGET_ARCH STREQUAL "x86")
        set(CMAKE_C_FLAGS   -m32)
        set(CMAKE_CXX_FLAGS -m32)
    elseif(TARGET_ARCH STREQUAL "x64")
        set(CMAKE_C_FLAGS   -m64)
        set(CMAKE_CXX_FLAGS -m64)
    else()
        message(FATAL_ERROR "Incorrect target architecture specified. "
                            "It should be either x86 or x64")
    endif()

    # Get the architecture: x86 vs x64
    if(TARGET_ARCH STREQUAL "x64")
        set(BIT "x86_64")
    else()
        set(BIT "x86")
    endif()

    add_compile_options(-Wall -Wextra -pedantic)
endif()

# Append a postfix for the debug libraries
if (NOT CMAKE_DEBUG_POSTFIX)
    set(CMAKE_DEBUG_POSTFIX d)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

###############################################################################
# Generate documentation if needed
#

# Check if Doxygen is installed
find_package(Doxygen)

if (DOXYGEN_FOUND AND BUILD_DOCUMENTATION)
    set(DOXYGEN_GENERATE_LATEX          NO)
    set(DOXYGEN_CREATE_SUBDIRS          YES)
    set(DOXYGEN_OUTPUT_DIRECTORY        ${CMAKE_BINARY_DIR}/docs/html)
    set(DOXYGEN_PROJECT_NAME            ${PROJECT_DESCRIPTION})
    set(DOXYGEN_EXTRACT_STATIC          YES)
    set(DOXYGEN_EXTRACT_ALL             YES)
    set(DOXYGEN_EXTRACT_LOCAL_METHODS   YES)
    set(DOXYGEN_WARN_NO_PARAMDOC        YES)
    set(DOXYGEN_RECURSIVE               YES)
    set(DOXYGEN_EXCLUDE                 ${PROJECT_SOURCE_DIR}/test
                                        ${PROJECT_SOURCE_DIR}/examples)
    set(DOXYGEN_HTML_OUTPUT             .)
    set(DOXYGEN_GENERATE_TREEVIEW       YES)
    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE  README.md)

    # Add the documentation target. This target doesn't build by default if doxygen is found.
    # To build it explicitly, use '--target documentation' or 'make documentation' option.
    doxygen_add_docs(documentation
                     WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                     COMMENT "Building the project documentation")
else()
    message("Failed to generate documentation: Doxygen is not found")
endif()

###############################################################################
# Testing
#
if (ENABLE_TESTING)
    # Coverage support.
    option(CODE_COVERAGE "Enable coverage reporting" ON)
    if (CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        # Add required flags (GCC & LLVM/Clang)
	    add_compile_options(-O0        # no optimization
                            -g         # generate debug info
			                --coverage # sets all required flags
	    )
	    if (CMAKE_VERSION VERSION_GREATER_EQUAL 3.13)
	        add_link_options(--coverage)
	    else()
	        link_libraries(--coverage)
	    endif()
    endif()
endif()

add_subdirectory(src)

if (ENABLE_EXAMPLES)
    add_subdirectory(examples)
endif()

if (ENABLE_TESTING)
    enable_testing()
    include(Dart)

    add_subdirectory(test)
endif()
