# 2018 (c) Juan G. Victores, Bartek Łukawski, Stephen Sinclair
# CopyPolicy: RtMidi license.

# Set minimum CMake required version for this project.
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

# Check for Jack (any OS)
find_package(PkgConfig)
pkg_check_modules(jack jack)

# Build Options
option(BUILD_SHARED_LIBS "Compile library as shared lib." TRUE)
option(BUILD_TESTS "Compile test programs." TRUE)

# API Options
option(RTMIDI_API_JACK "Compile with JACK support." ${jack_FOUND})
option(RTMIDI_API_ALSA "Compile with ALSA support." ${UNIX})
option(RTMIDI_API_WINMM "Compile with WINMM support." ${WIN32})
option(RTMIDI_API_CORE "Compile with CoreMIDI support." ${APPLE})

# Define a C++ project.
project(RtMidi LANGUAGES CXX)

# Create library target.
add_library(rtmidi SHARED RtMidi.h RtMidi.cpp)

# Read version info from configure.ac
set(R "m4_define\\(\\[lt_([a-z]+)\\], ([0-9]+)\\)")
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/configure.ac" CONFIGAC
  REGEX ${R})
foreach(_S ${CONFIGAC})
  string(REGEX REPLACE ${R} "\\1" k ${_S})
  string(REGEX REPLACE ${R} "\\2" v ${_S})
  set(SO_${k} ${v})
endforeach()
math(EXPR SO_current_minus_age "${SO_current} - ${SO_age}")
set(SO_VER ${SO_current_minus_age})
set(FULL_VER "${SO_current_minus_age}.${SO_revision}.${SO_age}")

# Add headers destination for install rule.
set_target_properties(rtmidi PROPERTIES PUBLIC_HEADER RtMidi.h
  SOVERSION ${SO_VER}
  VERSION ${FULL_VER})

# Set standard installation directories.
include(GNUInstallDirs)

# Set include paths, populate target interface.
include_directories(rtmidi PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                                  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

# Tweak API-specific configuration.
if(RTMIDI_API_JACK AND jack_FOUND)
    target_compile_definitions(rtmidi PRIVATE __UNIX_JACK__)
    target_include_directories(rtmidi PRIVATE ${jack_INCLUDEDIR})
    target_link_libraries(rtmidi PRIVATE ${jack_LIBRARIES})
endif()
if(RTMIDI_API_ALSA)
  find_package(ALSA)
  if (ALSA_LIBRARY)
    target_compile_definitions(rtmidi PRIVATE __LINUX_ALSA__)
    target_include_directories(rtmidi PRIVATE ${ALSA_INCLUDE_DIR})
    target_link_libraries(rtmidi PRIVATE ${ALSA_LIBRARY})
  endif()
endif()
if(RTMIDI_API_WINMM)
    target_compile_definitions(rtmidi PRIVATE __WINDOWS_MM__)
    target_link_libraries(rtmidi PRIVATE winmm.lib)
endif()
if(RTMIDI_API_CORE)
    target_compile_definitions(rtmidi PRIVATE __MACOSX_CORE__)
    target_link_libraries(rtmidi PRIVATE "-framework CoreServices")
    target_link_libraries(rtmidi PRIVATE "-framework CoreAudio")
    target_link_libraries(rtmidi PRIVATE "-framework CoreMIDI")
    target_link_libraries(rtmidi PRIVATE "-framework CoreFoundation")
    set_target_properties(rtmidi PROPERTIES LINK_FLAGS "-Wl,-F/Library/Frameworks")
endif()

# Add tests if requested.
if(BUILD_TESTS)
  add_executable(cmidiin    tests/cmidiin.cpp)
  add_executable(midiclock  tests/midiclock.cpp)
  add_executable(midiout    tests/midiout.cpp)
  add_executable(midiprobe  tests/midiprobe.cpp)
  add_executable(qmidiin    tests/qmidiin.cpp)
  add_executable(sysextest  tests/sysextest.cpp)
  set_target_properties(cmidiin midiclock midiout midiprobe qmidiin sysextest
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY tests
               LINK_LIBRARIES rtmidi)
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

# Install CMake configuration export file.
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
