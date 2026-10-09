//*****************************************//
//  sysexchunked.cpp
//  by Eric Bateman, 2026.
//
//  Sends a SysEx of any size in pieces and checks it arrives whole (#214).
//  Exits 0 when the message comes back identical.
//
//  Measured limits on one sendMessage(), which sending in pieces avoids:
//    ALSA seq   16355 bytes (alsa-lib's output buffer; snd_seq_set_output_buffer_size() raises it)
//    JACK       32720 bytes each way; a 66 kB dump from a device never arrived
//    CoreMIDI   about 117 MB on macOS, 19 MB on iOS, dropped silently past that;
//               in pieces, about 650 to 760 pieces per message
//    WinMM      none found (256 MB), but sendMessage() blocks until the data is sent
//  Measured with alsa-lib 1.2.14, JACK 1.9.21, macOS 26.4.1, iPadOS 26.7, Windows 11 25H2.
//
//  Receiving needs ignoreTypes( false, ... ) and setBufferSize() larger than the message.
//  On iOS, virtual ports need UIBackgroundModes "audio" in the app's Info.plist.
//*****************************************//

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include "RtMidi.h"

#if defined(_WIN32)
  #define NOMINMAX  // windows.h's min() macro breaks std::min
  #include <windows.h>
  #define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds )
#else // Unix variants
  #include <unistd.h>
  #define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif

// Any size works: only the first piece carries F0 and only the last F7.
static const size_t kSysExSpan = 512;

void sendLargeSysEx( RtMidiOut &midiout, const std::vector<unsigned char> &message )
{
  for ( size_t offset = 0; offset < message.size(); offset += kSysExSpan ) {
    size_t count = std::min( kSysExSpan, message.size() - offset );
    midiout.sendMessage( &message[offset], count );
    SLEEP( 2 );  // JACK takes about 32 kB per period, so spread the pieces out
  }
}

void usage( void )
{
  std::cout << "\nusage: sysexchunked N [out in]\n";
  std::cout << "       sysexchunked -l\n";
  std::cout << "       sysexchunked --listen [seconds [in]]\n\n";
  std::cout << "    N         length of the SysEx message to send.  Try a size a\n";
  std::cout << "              single sendMessage() cannot carry, such as 66312,\n";
  std::cout << "              the size of a real firmware dump.\n";
  std::cout << "    --listen  receive only, and report what arrives.  Useful for\n";
  std::cout << "              checking what a device actually sends, and whether\n";
  std::cout << "              a large message survives the transport.\n";
  std::cout << "    out in    port numbers, on APIs without virtual ports (Windows, Android).\n";
  std::cout << "              The default is port 0, which is rarely a loopback.\n";
  std::cout << "    -l        list port numbers; they can change when devices are replugged.\n\n";
  exit( 0 );
}

void listPorts( void )
{
  RtMidiOut midiout;
  RtMidiIn midiin;
  for ( unsigned int i = 0; i < midiout.getPortCount(); i++ )
    std::cout << "out " << i << ": " << midiout.getPortName( i ) << "\n";
  for ( unsigned int i = 0; i < midiin.getPortCount(); i++ )
    std::cout << "in  " << i << ": " << midiin.getPortName( i ) << "\n";
}

static std::vector<unsigned char> received;
static bool complete = false;
static unsigned int callbacks = 0;

void mycallback( double /*deltatime*/, std::vector<unsigned char> *message, void * /*userData*/ )
{
  received.insert( received.end(), message->begin(), message->end() );
  callbacks++;
  if ( !received.empty() && received.back() == 0xF7 ) complete = true;
}

int main( int argc, char *argv[] )
{
  if ( argc < 2 ) usage();

  if ( std::string( argv[1] ) == "-l" ) {
    try { listPorts(); }
    catch ( RtMidiError &error ) { error.printMessage(); return 1; }
    return 0;
  }

  bool listen = ( std::string( argv[1] ) == "--listen" );
  size_t nBytes = 0;
  int seconds = 30;
  int result = 1;
  unsigned int outPort = 0, inPort = 0;

  if ( listen ) {
    if ( argc > 2 ) seconds = atoi( argv[2] );
    if ( argc > 3 ) inPort = (unsigned int) atoi( argv[3] );
  } else {
    if ( argc != 2 && argc != 4 ) usage();
    nBytes = (size_t) atoi( argv[1] );
    if ( nBytes < 3 ) usage();
    if ( argc == 4 ) {
      outPort = (unsigned int) atoi( argv[2] );
      inPort = (unsigned int) atoi( argv[3] );
    }
  }

  RtMidiOut *midiout = 0;
  RtMidiIn *midiin = 0;

  try {
    midiout = new RtMidiOut();
    midiin = new RtMidiIn();

    // Windows and Android have no virtual ports, so use real ports there.
    if ( midiin->getCurrentApi() == RtMidi::WINDOWS_MM ||
         midiin->getCurrentApi() == RtMidi::WINDOWS_UWP ||
         midiin->getCurrentApi() == RtMidi::ANDROID_AMIDI ) {
      if ( inPort >= midiin->getPortCount() || outPort >= midiout->getPortCount() ) {
        std::cout << "No such MIDI port.\n";
        goto cleanup;
      }
      if ( listen )
        std::cout << "Listening on \"" << midiin->getPortName( inPort ) << "\".\n";
      else
        std::cout << "Opening \"" << midiout->getPortName( outPort ) << "\" for output and\n"
                  << "        \"" << midiin->getPortName( inPort ) << "\" for input.\n"
                  << "Connect them externally for the round trip to complete.\n";
      midiout->openPort( outPort );
      midiin->openPort( inPort );
    }
    else {
      midiout->openVirtualPort( "sysexchunked out" );
      midiin->openVirtualPort( "sysexchunked in" );
      if ( listen )
        std::cout << "Opened virtual port \"sysexchunked in\".\n"
                  << "Connect a device or another application to it.\n";
      else
        std::cout << "Opened virtual ports \"sysexchunked out\" and\n"
                  << "\"sysexchunked in\". Connect them now";
    }

    midiin->ignoreTypes( false, true, true );   // do not ignore SysEx

    // The default 1 kB input buffer can't hold a large SysEx.
    midiin->setBufferSize( listen ? 1048576 : (unsigned int) nBytes + 1024, 4 );

    midiin->setCallback( &mycallback );

    if ( listen ) {
      std::cout << "Listening for " << seconds << " seconds...\n";
      for ( int i = 0; i < seconds * 10 && !complete; i++ ) SLEEP( 100 );

      if ( received.empty() ) {
        std::cout << "Nothing received.\n";
      } else {
        // More than one callback means the message arrived in pieces.
        std::cout << "Received " << received.size() << " bytes in "
                  << callbacks << " callback(s).\n";
        std::cout << "First bytes:";
        for ( size_t i = 0; i < received.size() && i < 8; i++ )
          std::cout << " " << std::hex << (int) received[i] << std::dec;
        std::cout << ( complete ? "  (ends with F7)\n" : "  (no F7 seen)\n" );
        result = complete ? 0 : 1;
      }
    }
    else {
      // Give the user a moment to patch the two ports together before sending.
      for ( int i = 0; i < 10; i++ ) { std::cout << "." << std::flush; SLEEP( 500 ); }
      std::cout << "\n";

      // F0 7D <data...> F7.  0x7D is the non-commercial manufacturer id.
      std::vector<unsigned char> message;
      message.push_back( 0xF0 );
      message.push_back( 0x7D );
      while ( message.size() < nBytes - 1 )
        message.push_back( (unsigned char) ( message.size() & 0x7F ) );
      message.push_back( 0xF7 );

      std::cout << "Sending " << message.size() << " bytes in "
                << kSysExSpan << "-byte pieces...\n";
      sendLargeSysEx( *midiout, message );

      for ( int i = 0; i < 2000 && !complete; i++ ) SLEEP( 5 );

      if ( received.empty() )
        std::cout << "Nothing received. Are the two ports connected?\n";
      else
        std::cout << "Received " << received.size() << " bytes: "
                  << ( received == message ? "identical." : "MISMATCH." ) << "\n";
      result = ( received == message ) ? 0 : 1;
    }
  }
  catch ( RtMidiError &error ) {
    error.printMessage();
  }

 cleanup:
  delete midiout;
  delete midiin;
  return result;
}
