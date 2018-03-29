# 2018 (c) Juan G. Victores
# CopyPolicy: RtMidi license.

cmake_minimum_required(VERSION 3.0)

project(rtmidi)

add_library(rtmidi RtMidi.h
                   RtMidi.cpp)

set_target_properties(rtmidi PROPERTIES PUBLIC_HEADER RtMidi.h)

# Standard installation directories.
include(GNUInstallDirs)

target_include_directories(rtmidi PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

set(RTMIDI_INCLUDE_DIRS ${CMAKE_INSTALL_INCLUDEDIR})

if(WIN32)
    set_property(TARGET rtmidi APPEND PROPERTY COMPILE_DEFINITIONS __WINDOWS_MM__)
endif()

if(WIN32)
    target_link_libraries(rtmidi winmm.lib)
endif()

# set installation path for CMake modules
if(WIN32)
    set(RTMIDI_CMAKE_DESTINATION CMake)
else()
    set(RTMIDI_CMAKE_DESTINATION ${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}/cmake)
endif()

install(TARGETS rtmidi
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

set(RTMIDI_LIBRARIES "$<TARGET_LINKER_NAME:rtmidi>")

include(CMakePackageConfigHelpers)

# configure PkgConfig.cmake (install-tree)
configure_package_config_file("${CMAKE_SOURCE_DIR}/cmake/templates/RTMIDIConfig.cmake.in"
                              "${CMAKE_BINARY_DIR}/RTMIDIConfigForInstall.cmake"
                              INSTALL_DESTINATION ${RTMIDI_CMAKE_DESTINATION}
                              PATH_VARS RTMIDI_INCLUDE_DIRS RTMIDI_LIBRARIES
                              NO_CHECK_REQUIRED_COMPONENTS_MACRO)

# install PkgConfig.cmake
install(FILES "${CMAKE_BINARY_DIR}/RTMIDIConfigForInstall.cmake"
        RENAME RTMIDIConfig.cmake
        DESTINATION ${RTMIDI_CMAKE_DESTINATION})

# uninstall target configuration
configure_file(
    "${CMAKE_SOURCE_DIR}/cmake/templates/RTMIDIConfigUninstall.cmake.in"
    "${CMAKE_BINARY_DIR}/RTMIDIConfigUninstall.cmake" @ONLY)

# uninstall target creation
add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/RTMIDIConfigUninstall.cmake)
