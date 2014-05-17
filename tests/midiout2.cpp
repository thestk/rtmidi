//*****************************************//
//  midiout.cpp
//  by Gary Scavone, 2003-2014.
//  and Tobias Schlemmer 2014
//
/*! \example midiout2.cpp
    Simple program to test MIDI output.
*/
// *****************************************//



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

// This function should be embedded in a try/catch block in case of
// an exception.  It offers the user a choice of MIDI ports to open.
// It returns false if there are no ports available.
bool chooseMidiPort( rtmidi::MidiOut &midi );

//! The main program
int main( int /* argc*/, char */*argv*/[] )
{
	std::vector<unsigned char> message;
	std::cout << "\nWould you like to check all output ports? [Y/n] ";

	std::string keyHit;
	std::getline( std::cin, keyHit );
	rtmidi::ApiType type = rtmidi::ALL_API;
	if ( keyHit == "n" ) {
		type = rtmidi::UNSPECIFIED;
	}

	// rtmidi::MidiOut constructor
	try {
		rtmidi::MidiOut midiout(type);

		// Call function to select port.
		try {
			if ( chooseMidiPort( midiout ) == false ) return 1;
		}
		catch ( rtmidi::Error &error ) {
			error.printMessage();
			return 2;
		}


		SLEEP( 5000 );

		// Send out a series of MIDI messages.

		// Program change: 192, 5
		message.push_back( 192 );
		message.push_back( 5 );
		midiout.sendMessage( &message );

		SLEEP( 500 );

		message[0] = 0xF1;
		message[1] = 60;
		midiout.sendMessage( &message );

		// Control Change: 176, 7, 100 (volume)
		message[0] = 176;
		message[1] = 7;
		message.push_back( 100 );
		midiout.sendMessage( &message );

		// Note On: 144, 64, 90
		message[0] = 144;
		message[1] = 64;
		message[2] = 90;
		midiout.sendMessage( &message );

		SLEEP( 500 );

		// Note Off: 128, 64, 40
		message[0] = 128;
		message[1] = 64;
		message[2] = 40;
		midiout.sendMessage( &message );

		SLEEP( 500 );

		// Control Change: 176, 7, 40
		message[0] = 176;
		message[1] = 7;
		message[2] = 40;
		midiout.sendMessage( &message );

		SLEEP( 500 );

		// Sysex: 240, 67, 4, 3, 2, 247
		message[0] = 240;
		message[1] = 67;
		message[2] = 4;
		message.push_back( 3 );
		message.push_back( 2 );
		message.push_back( 247 );
		midiout.sendMessage( &message );


	}
	catch ( rtmidi::Error &error ) {
		error.printMessage();
		exit( EXIT_FAILURE );
	}

	return 0;
}

bool chooseMidiPort( rtmidi::MidiOut &midi )
{
	std::cout << "\nWould you like to open a virtual output port? [y/N] ";

	std::string keyHit;
	std::getline( std::cin, keyHit );
	if ( keyHit == "y" ) {
		midi.openVirtualPort("RtMidi virtual output");
		std::cout << "Press return to start the transmission." << std::endl;
		std::getline( std::cin, keyHit );

		return true;
	}

	std::string portName;
	rtmidi::PortList list = midi.getPortList(rtmidi::PortDescriptor::OUTPUT);
	if ( list.empty() ) {
		std::cout << "No output ports available!" << std::endl;
		return false;
	}

	rtmidi::Pointer<rtmidi::PortDescriptor> selected = list.front();
	if ( list.size() == 1 ) {
		std::cout << "\nOpening " << selected->getName() << std::endl;
	}  else {
		int nr;
		std::vector<rtmidi::Pointer<rtmidi::PortDescriptor> > pointers(list.size());
		// copy the data into a structure that is used by the user interface.
		std::copy(list.begin(),list.end(),pointers.begin());
		for (nr = 0 ; nr < (int)pointers.size(); nr++) {
			portName = pointers[nr]->getName(rtmidi::PortDescriptor::LONG_NAME
							 | rtmidi::PortDescriptor::UNIQUE_NAME
							 | rtmidi::PortDescriptor::INCLUDE_API);
			std::cout << "  Output port #" << nr << ": " << portName << '\n';
		}

		do {
			std::cout << "\nChoose a port number: ";
			std::cin >> nr;
		} while ( nr >= (int)pointers.size() );
		std::getline( std::cin, keyHit );  // used to clear out stdin
		selected = pointers[nr];
	}

	/* The midi setup might have changed meanwhile.
	   Our portlist is under our control. So we enumerate this list. */
	// midi.openPort( i );
	midi.openPort(selected);

	return true;
}
