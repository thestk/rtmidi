//*****************************************//
//  loopback
//  by Tobis Schlemmer, 2014.
//  inspired by virtual-loopback-test-automated.js from the node-midi project.
//  donated to RtMidi.
//
/*! \example loopback.cpp
  Simple program to test MIDI input and
  output in an internal loop using a user callback function.
*/
//*****************************************//

#include <iostream>
#include <cstdlib>
#include "RtMidi.h"

// Platform-dependent sleep routines.
#if defined(__WINDOWS_MM__)
#include <windows.h>
#define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds ) 
#else // Unix variants
#include <unistd.h>
#define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif

using namespace rtmidi;

bool ok = false;


int main( int /* argc */, char * /*argv*/[] )
{
	
	std::vector<ApiType> types = Midi::getCompiledApi();
	if (types.size() > 1) {
		for (size_t i = 0 ; i < types.size() ; i++) {
			try {
				MidiIn in (types[i]);
				PortList ports = in.getPortList(PortDescriptor::INPUT);
				if (ports.size()>1) {
					MidiIn in2 (types[i?0:1]);
					try {
						in2.openPort(ports.front());
					} catch (Error e) {
						std::string msg = e.getMessage();
						e.printMessage();
						if (e.getType() != Error::INVALID_DEVICE)
							abort();
					}
				}
			} catch (Error e) {
				std::string msg = e.getMessage();
				e.printMessage();
				if (msg != rtmidi_gettext("")) {
					if (e.getType() != Error::WARNING &&
					    e.getType() != Error::NO_DEVICES_FOUND)
						abort();
				}
			}
		}
	}
	return 0;
}
