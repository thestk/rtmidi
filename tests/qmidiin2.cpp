//*****************************************//
//  qmidiin.cpp
//  by Gary Scavone, 2003-2004.
//
//  Simple program to test MIDI input and
//  retrieval from the queue.
//
//*****************************************//

#include <iostream>
#include <cstdlib>
#include <signal.h>
#include "RtMidi.h"

// Platform-dependent sleep routines.
#if defined(__WINDOWS_MM__)
#include <windows.h>
#define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds ) 
#else // Unix variants
#include <unistd.h>
#define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif

bool done;
static void finish( int /*ignore*/ ){ done = true; }

void usage( rtmidi::PortList list ) {
	// Error function in case of incorrect command-line
	// argument specifications.
	std::cout << "\nusage: qmidiin <port>\n";
	std::cout << "    where port = the device to use (default = first available port).\n\n";

	std::cout << "Available ports:" << std::endl;
	for (rtmidi::PortList::iterator i = list.begin();
	     i != list.end(); i++) {
		std::cout << (*i)->getName(rtmidi::PortDescriptor::SESSION_PATH |
					   rtmidi::PortDescriptor::UNIQUE_NAME |
					   rtmidi::PortDescriptor::INCLUDE_API);
		std::cout << "\t";
		std::cout << (*i)->getName() << std::endl;
	}
	exit( 0 );
}

int main( int argc, char *argv[] )
{
	std::vector<unsigned char> message;
	int nBytes, i;
	double stamp;


	// rtmidi::MidiIn constructor
	try {
		rtmidi::MidiIn midiin;


		rtmidi::PortList list = midiin.getPortList();

		// Minimal command-line check.
		if ( argc > 2 ) usage(list);

		rtmidi::Pointer<rtmidi::PortDescriptor> port = 0;
		// Check available ports vs. specified.
		if ( argc == 2 ) {
			for (rtmidi::PortList::iterator i = list.begin();
			     i != list.end(); i++) {
				if (argv[1] == (*i)->getName(rtmidi::PortDescriptor::SESSION_PATH |
							     rtmidi::PortDescriptor::UNIQUE_NAME |
							     rtmidi::PortDescriptor::INCLUDE_API)) {
					port = *i;
					break;
				}
			}
		} else {
			port = list.front();
		}
		if ( !port ) {
			std::cout << "Invalid port specifier!\n";
			usage(list);
		}

		try {
			midiin.openPort( port );
		}
		catch ( rtmidi::Error &error ) {
			error.printMessage();
			return 1;
		}

		// Don't ignore sysex, timing, or active sensing messages.
		midiin.ignoreTypes( false, false, false );

		// Install an interrupt handler function.
		done = false;
		(void) signal(SIGINT, finish);

		// Periodically check input queue.
		std::cout << "Reading MIDI from port ... quit with Ctrl-C.\n";
		while ( !done ) {
			stamp = midiin.getMessage( &message );
			nBytes = message.size();
			for ( i=0; i<nBytes; i++ )
				std::cout << "Byte " << i << " = " << (int)message[i] << ", ";
			if ( nBytes > 0 )
				std::cout << "stamp = " << stamp << std::endl;

			// Sleep for 10 milliseconds.
			SLEEP( 10 );
		}
	}
	catch ( rtmidi::Error &error ) {
		error.printMessage();
		exit( EXIT_FAILURE );
	}

	return 0;
}
