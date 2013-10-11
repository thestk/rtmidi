//*****************************************//
//  midiout.cpp
//  by Gary Scavone, 2003-2004.
//
//  Simple program to test MIDI output.
//
//*****************************************//

#include <iostream>
#include "RtMidi.h"

// Platform-dependent sleep routines.
#if defined(__WINDOWS_MM__)
  #include <windows.h>
  #define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds ) 
#else // Unix variants
  #include <unistd.h>
  #define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif

void usage(void) {
  // Error function in case of incorrect command-line
  // argument specifications.
  std::cout << "\nuseage: midiout <port>\n";
  std::cout << "    where port = the device to use (default = 0).\n\n";
  exit(0);
}

int main(int argc, char *argv[])
{
  RtMidiOut *midiout = 0;
  std::vector<unsigned char> message;

  // Minimal command-line check.
  if ( argc > 2 ) usage();

  // RtMidiOut constructor
  try {
    midiout = new RtMidiOut();
  }
  catch (RtError &error) {
    error.printMessage();
    exit(EXIT_FAILURE);
  }

  // Check available ports vs. specified.
  unsigned int port = 0;
  unsigned int nPorts = midiout->getPortCount();
  if ( argc == 2 ) port = (unsigned int) atoi( argv[1] );
  if ( port >= nPorts ) {
    delete midiout;
    std::cout << "Invalid port specifier!\n";
    usage();
  }

  try {
    midiout->openPort( port );
    //midiout->openVirtualPort();
  }
  catch (RtError &error) {
    error.printMessage();
    goto cleanup;
  }

  // Send out a series of MIDI messages.

  // Program change: 192, 5
  message.push_back( 192 );
  message.push_back( 5 );
  midiout->sendMessage( &message );

  // Control Change: 176, 7, 100 (volume)
  message[0] = 176;
  message[1] = 7;
  message.push_back( 100 );
  midiout->sendMessage( &message );

  // Note On: 144, 64, 90
  message[0] = 144;
  message[1] = 64;
  message[2] = 90;
  midiout->sendMessage( &message );

  SLEEP( 500 );

  // Note Off: 128, 64, 40
  message[0] = 128;
  message[1] = 64;
  message[2] = 40;
  midiout->sendMessage( &message );

  // Sysex: 240, 67, 16, 4, 3, 2, 247
  message[0] = 240;
  message[1] = 67;
  message[2] = 4;
  message.push_back( 3 );
  message.push_back( 2 );
  message.push_back( 247 );
  midiout->sendMessage( &message );

  // Clean up
 cleanup:
  delete midiout;

  return 0;
}
