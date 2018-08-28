# 2018 (c) Juan G. Victores, Bartek Łukawski, Stephen Sinclair
# CopyPolicy: RtMidi license.

# Set minimum CMake required version for this project.
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

# Define a C++ project.
project(RtMidi LANGUAGES CXX)

# Check for Jack (any OS)
find_library(JACK_LIB jack)
find_package(PkgConfig)
pkg_check_modules(jack jack)
if(JACK_LIB OR jack_FOUND)
  set(HAVE_JACK TRUE)
endif()

# Necessary for Windows
if(WIN32)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
  set(CMAKE_DEBUG_POSTFIX "d")
endif()

# Build Options
option(BUILD_SHARED_LIBS "Compile library shared lib." TRUE)
option(BUILD_STATIC_LIBS "Compile library static lib." TRUE)
option(BUILD_TESTING "Compile test programs." TRUE)
set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type (Release,Debug)")

# API Options
option(RTMIDI_API_JACK "Compile with JACK support." ${HAVE_JACK})
if(UNIX AND NOT APPLE)
  option(RTMIDI_API_ALSA "Compile with ALSA support." ON)
endif()
option(RTMIDI_API_WINMM "Compile with WINMM support." ${WIN32})
option(RTMIDI_API_CORE "Compile with CoreMIDI support." ${APPLE})

# Add -Wall if possible
if (CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif (CMAKE_COMPILER_IS_GNUCXX)

# Add debug flags
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions(-D__RTMIDI_DEBUG__)
  if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
  endif (CMAKE_COMPILER_IS_GNUCXX)
endif ()

# Read libtool version info from configure.ac
set(R "m4_define\\(\\[lt_([a-z]+)\\], ([0-9]+)\\)")
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/configure.ac" CONFIGAC
  REGEX ${R})
foreach(_S ${CONFIGAC})
  string(REGEX REPLACE ${R} "\\1" k ${_S})
  string(REGEX REPLACE ${R} "\\2" v ${_S})
  set(SO_${k} ${v})
endforeach()
math(EXPR SO_current_minus_age "${SO_current} - ${SO_age}")
set(SO_VER "${SO_current_minus_age}")
set(FULL_VER "${SO_current_minus_age}.${SO_revision}.${SO_age}")

# Read package version info from configure.ac
set(R "AC_INIT\\(RtMidi, ([0-9\\.]+),.*\\)")
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/configure.ac" CONFIGAC
  REGEX ${R})
string(REGEX REPLACE ${R} "\\1" PACKAGE_VERSION ${CONFIGAC})

# Init variables
set(rtmidi_SOURCES RtMidi.cpp RtMidi.h rtmidi_c.cpp rtmidi_c.h)
set(LINKLIBS)
set(INCDIRS)
set(PKGCONFIG_REQUIRES)
set(API_DEFS)
set(API_LIST)

# Tweak API-specific configuration.

# Jack
if(RTMIDI_API_JACK)
  if (NOT HAVE_JACK)
    message(FATAL_ERROR "Jack API requested but no Jack dev libraries found")
  endif()
  set(NEED_PTHREAD ON)
  list(APPEND PKGCONFIG_REQUIRES "jack")
  list(APPEND API_DEFS "-D__UNIX_JACK__")
  list(APPEND API_LIST "jack")
  if(jack_FOUND)
    list(APPEND LINKLIBS ${jack_LIBRARIES})
    list(APPEND INCDIRS ${jack_INCLUDEDIR})
  else()
    list(APPEND LINKLIBS ${JACK_LIB})
  endif()
endif()

# ALSA
if(RTMIDI_API_ALSA)
  set(NEED_PTHREAD ON)
  find_package(ALSA)
  if (NOT ALSA_FOUND)
    message(FATAL_ERROR "ALSA API requested but no ALSA dev libraries found")
  endif()
  list(APPEND INCDIRS ${ALSA_INCLUDE_DIR})
  list(APPEND LINKLIBS ${ALSA_LIBRARY})
  list(APPEND PKGCONFIG_REQUIRES "alsa")
  list(APPEND API_DEFS "-D__LINUX_ALSA__")
  list(APPEND API_LIST "alsa")
endif()

# WinMM
if(RTMIDI_API_WINMM)
  list(APPEND API_DEFS "-D__WINDOWS_MM__")
  list(APPEND API_LIST "winmm")
  list(APPEND LINKLIBS winmm)
endif()

# CoreMIDI
if(RTMIDI_API_CORE)
  list(APPEND API_DEFS "-D__MACOSX_CORE__")
  list(APPEND LINKLIBS "-framework CoreServices")
  list(APPEND LINKLIBS "-framework CoreAudio")
  list(APPEND LINKLIBS "-framework CoreMIDI")
  list(APPEND LINKLIBS "-framework CoreFoundation")
  list(APPEND LINKFLAGS "-Wl,-F/Library/Frameworks")
endif()

# pthread
if (NEED_PTHREAD)
  find_package(Threads REQUIRED
    CMAKE_THREAD_PREFER_PTHREAD
    THREADS_PREFER_PTHREAD_FLAG)
  list(APPEND LINKLIBS Threads::Threads)
endif()

# Create library targets.
cmake_policy(SET CMP0042 OLD)
set(LIB_TARGETS)
if(BUILD_SHARED_LIBS)
  add_library(rtmidi SHARED ${rtmidi_SOURCES})
  list(APPEND LIB_TARGETS rtmidi)

  # Add headers destination for install rule.
  set_target_properties(rtmidi PROPERTIES PUBLIC_HEADER RtMidi.h
    SOVERSION ${SO_VER}
    VERSION ${FULL_VER})

  # Set include paths, populate target interface.
  target_include_directories(rtmidi PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    ${INCDIRS})

  # Set compile-time definitions
  target_compile_definitions(rtmidi PRIVATE ${API_DEFS})

  target_link_libraries(rtmidi ${LINKLIBS})
endif()

if(BUILD_STATIC_LIBS)
  add_library(rtmidi_static STATIC ${rtmidi_SOURCES})
  list(APPEND LIB_TARGETS rtmidi_static)

  # Add headers destination for install rule.
  set_target_properties(rtmidi_static PROPERTIES PUBLIC_HEADER RtMidi.h
    SOVERSION ${SO_VER}
    VERSION ${FULL_VER})

  # Set include paths, populate target interface.
  target_include_directories(rtmidi_static PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    ${INCDIRS})

  # Set compile-time definitions
  target_compile_definitions(rtmidi_static PRIVATE ${API_DEFS})

  target_link_libraries(rtmidi_static ${LINKLIBS})
endif()

# Set standard installation directories.
include(GNUInstallDirs)

# Add tests if requested.
if(BUILD_TESTING)
  add_executable(cmidiin    tests/cmidiin.cpp)
  add_executable(midiclock  tests/midiclock.cpp)
  add_executable(midiout    tests/midiout.cpp)
  add_executable(midiprobe  tests/midiprobe.cpp)
  add_executable(qmidiin    tests/qmidiin.cpp)
  add_executable(sysextest  tests/sysextest.cpp)
  list(GET LIB_TARGETS 0 LIBRTMIDI)
  set_target_properties(cmidiin midiclock midiout midiprobe qmidiin sysextest
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY tests
               INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}
               LINK_LIBRARIES ${LIBRTMIDI})
endif()

# Set standard installation directories.
include(GNUInstallDirs)

# Message
string(REPLACE ";" " " apilist "${API_LIST}")
message(STATUS "Compiling with support for: ${apilist}")

# PkgConfig file
string(REPLACE ";" " " req "${PKGCONFIG_REQUIRES}")
string(REPLACE ";" " " api "${API_DEFS}")
configure_file("rtmidi.pc.in" "rtmidi.pc" @ONLY)

# Add install rule.
install(TARGETS ${LIB_TARGETS}
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
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/RtMidiConfigUninstall.cmake.in"
    "${CMAKE_BINARY_DIR}/RtMidiConfigUninstall.cmake" @ONLY)

# Create uninstall target.
add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/RtMidiConfigUninstall.cmake)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/rtmidi.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
