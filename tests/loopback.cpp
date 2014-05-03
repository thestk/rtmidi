//*****************************************//
//  loopback
//  by Tobis Schlemmer, 2014.
//  inspired by virtual-loopback-test-automated.js from the node-midi project.
//  donated to RtMidi.
//
//  Simple program to test MIDI input and
//  output using a user callback function.
//
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


/* Here, we store the expected output. Timing is not tested */
std::vector<unsigned char> virtualinstring;
const char * virtualinstringgoal =
	"\xc0\x5\xf1\x3c\xb0\x7\x64\x90\x40\x5a\x80\x40\x28\xb0\x7\x28\xf0\x43\x4\x3\x2\xf7";
std::vector<unsigned char> instring;
const  char * instringgoal =
	"\xc0\x6\xf1\x3d\xb0\x8\x64\x90\x41\x5a\x80\x41\x28\xb0\x8\x28\xf0\x43\x4\x3\x3\xf7";

inline size_t getlength(const char * messages) {
	size_t retval = 0;
	const unsigned char * c = reinterpret_cast<const unsigned char *>(messages);
	while (*(c++) != 0xf7) retval++;
	return ++retval;
}

void mycallback1( double deltatime, std::vector< unsigned char > *message, void * /* userData */ )
{
	unsigned int nBytes = message->size();
	for ( unsigned int i=0; i<nBytes; i++ ) {
		instring.push_back(message->at(i));
		//		std::cout << "\\x" << std::hex << (int)message->at(i) << std::flush;
	}
	/*	if ( nBytes > 0 )
		std::cout << "stamp = " << deltatime << std::endl;
	*/
}

void mycallback2( double deltatime, std::vector< unsigned char > *message, void * /* userData */ )
{
	unsigned int nBytes = message->size();
	for ( unsigned int i=0; i<nBytes; i++ ) {
		virtualinstring.push_back(message->at(i));
		// std::cout << "\\x" << std::hex << (int)message->at(i);
	}
	/*
	  if ( nBytes > 0 )
	  std::cout << "stamp = " << deltatime << std::endl;
	*/
}



int main( int argc, char */*argv*/[] )
{

	std::vector<unsigned char> message;

	try {

		// RtMidiIn constructor
		RtMidiIn virtualin;
		// RtMidiIn constructor
		RtMidiOut virtualout;

		virtualin.openVirtualPort("RtMidi Test Virtual In");
		virtualout.openVirtualPort("RtMidi Test Virtual Out");

		rtmidi::Pointer<rtmidi::PortDescriptor> indescriptor 
			= virtualin.getDescriptor(true);

		rtmidi::Pointer<rtmidi::PortDescriptor> outdescriptor 
			= virtualout.getDescriptor(true);

		{ // avoid problems with wrong destruction order
			/* use smart pointers to handle deletion */
			rtmidi::Pointer<rtmidi::MidiInApi> midiin = outdescriptor->getInputApi();
			if (!midiin) abort();
			rtmidi::Pointer<rtmidi::MidiOutApi> midiout = indescriptor->getOutputApi();
			if (!midiout) abort();



			midiin->openPort(outdescriptor);
			midiout->openPort(indescriptor);
    

			// Set our callback function.  This should be done immediately after
			// opening the port to avoid having incoming messages written to the
			// queue instead of sent to the callback function.
			midiin->setCallback( &mycallback1 );
			virtualin.setCallback( &mycallback2 );

			// Don't ignore sysex, timing, or active sensing messages.
			// Don't ignore sysex, timing, or active sensing messages.
			midiin->ignoreTypes( false, false, false );
			virtualin.ignoreTypes( false, false, false );

			SLEEP( 5000 );

			// Send out a series of MIDI messages.

			// Program change: 192, 5
			message.push_back( 192 );
			message.push_back( 5 );
			midiout->sendMessage( &message );
			message[1] = 6;
			virtualout.sendMessage(&message);

			SLEEP( 500 );

			message[0] = 0xF1;
			message[1] = 60;
			midiout->sendMessage( &message );
			message[1] = 61;
			virtualout.sendMessage(&message);

			// Control Change: 176, 7, 100 (volume)
			message[0] = 176;
			message[1] = 7;
			message.push_back( 100 );
			midiout->sendMessage( &message );
			message[1] = 8;
			virtualout.sendMessage ( &message );

			// Note On: 144, 64, 90
			message[0] = 144;
			message[1] = 64;
			message[2] = 90;
			midiout->sendMessage( &message );
			message[1] = 65;
			virtualout.sendMessage( &message );

			SLEEP( 500 );

			// Note Off: 128, 64, 40
			message[0] = 128;
			message[1] = 64;
			message[2] = 40;
			midiout->sendMessage( &message );
			message[1] = 65;
			virtualout.sendMessage( &message );

			SLEEP( 500 );

			// Control Change: 176, 7, 40
			message[0] = 176;
			message[1] = 7;
			message[2] = 40;
			midiout->sendMessage( &message );
			message[1] = 8;
			virtualout.sendMessage( &message );

			SLEEP( 500 );

			// Sysex: 240, 67, 4, 3, 2, 247
			message[0] = 240;
			message[1] = 67;
			message[2] = 4;
			message.push_back( 3 );
			message.push_back( 2 );
			message.push_back( 247 );
			midiout->sendMessage( &message );
			message[4] = 3;
			virtualout.sendMessage( &message );

			SLEEP( 500 );
		}
		const unsigned char * goal = reinterpret_cast<const unsigned char *>(instringgoal);
		size_t i;
		std::cout << "Virtual output -> input:" << std::endl;
		if (instring.size() != getlength(instringgoal)) abort();
		for (i = 0 ; i < instring.size() && goal[i] ; i++){
			std::cout << " " << std::hex << (int) instring[i];
			std::cout << "/" << std::hex << (int) goal[i] << std::flush;
			if (instring[i] != goal[i]) abort();
		}
		std::cout << std::endl;
		if (i != instring.size()) abort();
		goal = reinterpret_cast<const unsigned char *>(virtualinstringgoal);
		std::cout << "Output -> virtual input:" << std::endl;
		if (instring.size() != getlength(virtualinstringgoal)) abort();
		for (i = 0 ; i < virtualinstring.size() && goal[i] ; i++) {
			std::cout << " " << std::hex << (int) virtualinstring[i];
			std::cout << "/" << std::hex << (int) goal[i] << std::flush;
			if (virtualinstring[i] != goal[i]) abort();
		}
		std::cout << std::endl;
		if (i != virtualinstring.size()) abort();
		
	} catch ( RtMidiError &error ) {
		error.printMessage();
	}
}
