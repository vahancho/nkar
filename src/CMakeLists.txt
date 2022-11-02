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

set(TARGET nkar)

set(HEADERS
    export.h
    color.h
    comparator.h
    image.h
    point.h
    stb_image.h
    stb_image_write.h
)

set(SOURCES
    color.cpp
    comparator.cpp
    image.cpp
    point.cpp
)

add_library(${TARGET} ${HEADERS} ${SOURCES})

# Append a postfix for the debug version of the library
set_target_properties(${TARGET} PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")

# Specify the include directories for the target
target_include_directories(${TARGET}
                           PUBLIC
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
                           "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_compile_definitions(${TARGET} PUBLIC MAKEDLL)

###############################################################################
# The installation and packaging
#

# The installation directories
# Note: exporting of pugixml doesn't seem to be necessary, but CMake requires it
# when build static libs. It's either a bug or intended behavior.
install(TARGETS  ${TARGET}
        EXPORT   ${TARGET}Targets
        LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install header files
install(FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Install help files
install(FILES     ${PROJECT_SOURCE_DIR}/README.md          TYPE DOC)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../docs/html TYPE DOC OPTIONAL)

# Install the export
install(EXPORT ${TARGET}Targets
        FILE ${TARGET}Targets.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${TARGET}
)

# Packaging
include(CMakePackageConfigHelpers)
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}Config.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${TARGET}
)

# Creating a Package Configuration File
install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}ConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${TARGET}
)

# Creating a Package Version File
set_property(TARGET ${TARGET} PROPERTY VERSION ${CMAKE_PROJECT_VERSION})
set_property(TARGET ${TARGET} PROPERTY INTERFACE_${TARGET}_MAJOR_VERSION ${PROJECT_VERSION_MAJOR})
set_property(TARGET ${TARGET} APPEND PROPERTY COMPATIBLE_INTERFACE_STRING ${TARGET}_MAJOR_VERSION)

# Generate the version file for the configuration file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}ConfigVersion.cmake"
    VERSION "${CMAKE_PROJECT_VERSION}"
    COMPATIBILITY AnyNewerVersion
)

# Exporting targets from the build tree if we want our project to also be used
# from a build directory
export(EXPORT ${TARGET}Targets
       FILE "${CMAKE_CURRENT_BINARY_DIR}/../cmake/${TARGET}Targets.cmake"
)

# Set the name of the package
string(TOLOWER
       "${CMAKE_PROJECT_NAME}-${CMAKE_PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}-${BIT}-${CMAKE_BUILD_TYPE}"
       CPACK_PACKAGE_FILE_NAME)

# Enable packaging with CPack
include(CPack)
