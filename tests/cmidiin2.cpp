//*****************************************//
//  cmidiin.cpp
//  by Gary Scavone, 2003-2004.
//
//  Simple program to test MIDI input and
//  use of a user callback function.
//
//*****************************************//

#include <iostream>
#include <cstdlib>
#include "RtMidi.h"

void usage( void ) {
  // Error function in case of incorrect command-line
  // argument specifications.
  std::cout << "\nuseage: cmidiin2 <port>\n";
  std::cout << "    where port = the device to use (default = 0).\n\n";
  exit( 0 );
}

void mycallback( double deltatime, std::vector< unsigned char > *message, void * /* userData */ )
{
  unsigned int nBytes = message->size();
  for ( unsigned int i=0; i<nBytes; i++ )
    std::cout << "Byte " << i << " = " << (int)message->at(i) << ", ";
  if ( nBytes > 0 )
    std::cout << "stamp = " << deltatime << std::endl;
}

// This function should be embedded in a try/catch block in case of
// an exception.  It offers the user a choice of MIDI ports to open.
// It returns false if there are no ports available.
bool chooseMidiPort( RtMidiIn &rtmidi );

int main( int argc, char */*argv*/[] )
{

  // Minimal command-line check.
  if ( argc > 2 ) usage();

  try {

    // RtMidiIn constructor
    RtMidiIn midiin;

    // Call function to select port.
    if ( chooseMidiPort( midiin ) == false ) return 0;

    // Set our callback function.  This should be done immediately after
    // opening the port to avoid having incoming messages written to the
    // queue instead of sent to the callback function.
    midiin.setCallback( &mycallback );

    // Don't ignore sysex, timing, or active sensing messages.
    midiin.ignoreTypes( false, false, false );

    std::cout << "\nReading MIDI input ... press <enter> to quit.\n";
    char input;
    std::cin.get(input);

  } catch ( RtMidiError &error ) {
    error.printMessage();
  }
}

bool chooseMidiPort( RtMidiIn &midi )
{
  std::cout << "\nWould you like to open a virtual input port? [y/N] ";

  std::string keyHit;
  std::getline( std::cin, keyHit );
  if ( keyHit == "y" ) {
    midi.openVirtualPort();
    return true;
  }

  std::string portName;
  rtmidi::PortList list = midi.getPortList(rtmidi::PortDescriptor::INPUT);
  if ( list.empty() ) {
    std::cout << "No input ports available!" << std::endl;
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
		  std::cout << "  Input port #" << nr << ": " << portName << '\n';
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
