# 2018 (c) Juan G. Victores
# CopyPolicy: RtMidi license.

cmake_minimum_required(VERSION 3.0)

project(rtmidi)

add_library(rtmidi RtMidi.h
                   RtMidi.cpp)

set_target_properties(rtmidi PROPERTIES PUBLIC_HEADER RtMidi.h)

target_include_directories(rtmidi PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

if(_WINDOWS)
    set_target_properties(rtmidi APPEND PROPERTY COMPILE_DEFINITIONS __WINDOWS_MM__)
    target_link_libraries(rtmidi winmm.lib)
endif()

# Standard installation directories.
include(GNUInstallDirs)

install(TARGETS rtmidi
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

set_property(GLOBAL APPEND PROPERTY RTMIDI_TARGETS rtmidi)
