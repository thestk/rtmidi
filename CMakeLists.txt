# 2018 (c) Juan G. Victores
# CopyPolicy: RtMidi license.

# Set minimum CMake required version for this project.
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

# Define a project.
project(RtMidi)

# Create library target.
add_library(rtmidi RtMidi.h
                   RtMidi.cpp)

# Add headers destination for install rule.
set_target_properties(rtmidi PROPERTIES PUBLIC_HEADER RtMidi.h)

# Standard installation directories.
include(GNUInstallDirs)

# Set include paths, populate target interface.
target_include_directories(rtmidi PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                                         $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

# Tweak platform-specific configuration.
if(WIN32)
    target_compile_definitions(rtmidi PUBLIC __WINDOWS_MM__)
    target_link_libraries(rtmidi PUBLIC winmm.lib)
endif()

# Add install rule.
install(TARGETS rtmidi
        EXPORT RtMidiTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Store the package in the user registry.
export(PACKAGE RtMidi)

# Set installation path for CMake files.
if(WIN32)
    set(RTMIDI_CMAKE_DESTINATION cmake)
else()
    set(RTMIDI_CMAKE_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/RtMidi)
endif()

# Create CMake configuration export file.
file(WRITE ${CMAKE_BINARY_DIR}/RtMidiConfig.cmake "include(\${CMAKE_CURRENT_LIST_DIR}/RtMidiTargets.cmake)")

# Install CMake configuration file.
install(FILES ${CMAKE_BINARY_DIR}/RtMidiConfig.cmake
        DESTINATION ${RTMIDI_CMAKE_DESTINATION})

# Export library target (build-tree).
export(EXPORT RtMidiTargets
       NAMESPACE RtMidi::)

# Export library target (install-tree).
install(EXPORT RtMidiTargets
        DESTINATION ${RTMIDI_CMAKE_DESTINATION}
        NAMESPACE RtMidi::)

# Configure uninstall target.
configure_file(
    "${CMAKE_SOURCE_DIR}/cmake/templates/RTMIDIConfigUninstall.cmake.in"
    "${CMAKE_BINARY_DIR}/RTMIDIConfigUninstall.cmake" @ONLY)

# Create uninstall target.
add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/RTMIDIConfigUninstall.cmake)
