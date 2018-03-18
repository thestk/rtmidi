[![Build Status](https://travis-ci.org/keinstein/rtmidi.svg?branch=master-ts)](https://travis-ci.org/keinstein/rtmidi)

Extended RtMidi fork for Mutabor, GUI based Software, and saving MIDI connections
=================================================================================

RtMidi - a set of C++ classes that provide a common API for realtime MIDI input/output across Linux (ALSA & JACK), Macintosh OS X (CoreMidi & JACK) and Windows (Multimedia).

By Gary P. Scavone, 2003-2014.
Forked by Tobias Schlemmer, 2014-2018.

This fork has been started because the original RtMidi did not work
for Mutabor and ALSA. Mutabor has a two-step activation scheme. The
MIDI configuration is set up while no connections are active. After
compiling the tuning logic all MIDI interfaces are activated at
once. Each MIDI connection that RtMidi makes is reported as MIDI
device again by RtMidi. This leads to a renumbering of all MIDI
endpoints which have a higher ALSA device ID than the current RtMidi
instance. The result is unexpected behaviour.

As a side effect this library uses a different namespace and is more
C++-ish than the original RtMidi.

Incompatible changes against upstream
-------------------------------------

- The old API has been deprecated as there is no way to rely on
  consecutive port numbers. It is always the responsibility of the 
  end user not to change the MIDI configuration between certain points
  in the execution path of the library. Obviously they usually lack the
  necessary information for that Currently, it is still available, but a 
  compiler warning is generated if applicable
  
- `__MACOSX_CORE__` has been renamed to `__MACOSX_CORE__`


Compilation
-----------

This distribution of RtMidi contains the following:

doc:      RtMidi documentation (see doc/html/index.html)
tests:    example RtMidi programs

On unix systems, type "./configure" in the top level directory, then "make" in the tests/ directory to compile the test programs.  In Windows, open the Visual C++ workspace file located in the tests/ directory.

If you checked out the code from git, please run "autoreconf" before "./configure".

Overview
--------

RtMidi is a set of C++ classes (rtmidi::MidiIn, rtmidi::MidiOut, and API specific classes) that provide a common API (Application Programming Interface) for realtime MIDI input/output across Linux (ALSA, JACK), Macintosh OS X (CoreMIDI, JACK), and Windows (Multimedia Library) operating systems.  RtMidi significantly simplifies the process of interacting with computer MIDI hardware and software.  It was designed with the following goals:

  - object oriented C++ design
  - simple, common API across all supported platforms
  - only one header and one source file for easy inclusion in programming projects
  - MIDI device enumeration

MIDI input and output functionality are separated into two classes, RtMidiIn and RtMidiOut.  Each class instance supports only a single MIDI connection.  RtMidi does not provide timing functionality (i.e., output messages are sent immediately).  Input messages are timestamped with delta times in seconds (via a double floating point type).  MIDI data is passed to the user as raw bytes using an std::vector<unsigned char>.

Further reading
---------------

For complete documentation on the original RtMidi, see the doc directory of the distribution or surf to http://music.mcgill.ca/~gary/rtmidi/.


Legal and ethical
-----------------

The RtMidi license is similar to the the MIT License, with the added
"feature" that modifications be sent to the developer.

    RtMidi: realtime MIDI i/o C++ classes
    Copyright (c) 2003-2014 Gary P. Scavone

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation files
    (the "Software"), to deal in the Software without restriction,
    including without limitation the rights to use, copy, modify, merge,
    publish, distribute, sublicense, and/or sell copies of the Software,
    and to permit persons to whom the Software is furnished to do so,
    subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    Any person wishing to distribute modifications to the Software is
    asked to send the modifications to the original developer so that
    they can be incorporated into the canonical version.  This is,
    however, not a binding provision of this license.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
    ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
    CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
    WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
