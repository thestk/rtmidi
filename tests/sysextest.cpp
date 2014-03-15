//*****************************************//
//  sysextest.cpp
//  by Gary Scavone, 2003-2005.
//
//  Simple program to test MIDI sysex sending and receiving.
//
//*****************************************//

#include <iostream>
#include <cstdlib>
#include <typeinfo>
#include "RtMidi.h"

void usage( void ) {
  std::cout << "\nuseage: sysextest N\n";
  std::cout << "    where N = length of sysex message to send / receive.\n\n";
  exit( 0 );
}

// Platform-dependent sleep routines.
#if defined(__WINDOWS_MM__)
  #include <windows.h>
  #define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds ) 
#else // Unix variants
  #include <unistd.h>
  #define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif

void mycallback( double deltatime, std::vector< unsigned char > *message, void *userData )
{
  unsigned int nBytes = message->size();
  for ( unsigned int i=0; i<nBytes; i++ )
    std::cout << "Byte " << i << " = " << (int)message->at(i) << ", ";
  if ( nBytes > 0 )
    std::cout << "stamp = " << deltatime << std::endl;
}

int main( int argc, char *argv[] )
{
  RtMidiOut *midiout = 0;
  RtMidiIn *midiin = 0;
  std::vector<unsigned char> message;
  unsigned int i, nBytes;

  // Minimal command-line check.
  if ( argc != 2 ) usage();
  nBytes = (unsigned int) atoi( argv[1] );

  // RtMidiOut and RtMidiIn constructors
  try {
    midiout = new RtMidiOut();
    midiin = new RtMidiIn();
  }
  catch ( RtMidiError &error ) {
    error.printMessage();
    goto cleanup;
  }

  // Don't ignore sysex, timing, or active sensing messages.
  midiin->ignoreTypes( false, true, true );

  try {
  midiin->openVirtualPort( "MyVirtualInputPort" );
  midiout->openPort( 0 );
  }
  catch ( RtMidiError &error ) {
    error.printMessage();
    goto cleanup;
  }

  midiin->setCallback( &mycallback );

  message.push_back( 0xF6 );
  midiout->sendMessage( &message );
  SLEEP( 500 ); // pause a little

  // Create a long sysex message of numbered bytes and send it out ... twice.
  for ( int n=0; n<2; n++ ) {
    message.clear();
    message.push_back( 240 );
    for ( i=0; i<nBytes; i++ )
      message.push_back( i % 128 );
    message.push_back( 247 );
    midiout->sendMessage( &message );

    SLEEP( 500 ); // pause a little
  }

  // Clean up
 cleanup:
  delete midiout;
  delete midiin;

  return 0;
}
