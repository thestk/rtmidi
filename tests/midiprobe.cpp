// midiprobe.cpp
//
// Simple program to check MIDI inputs and outputs.
//
// by Gary Scavone, 2003-2004.

#include <iostream>
#include <cstdlib>
#include "RtMidi.h"

int main()
{
  RtMidiIn  *midiin = 0;
  RtMidiOut *midiout = 0;

  try {

    // RtMidiIn constructor ... exception possible
    midiin = new RtMidiIn();

    // Check inputs.
    unsigned int nPorts = midiin->getPortCount();
    std::cout << "\nThere are " << nPorts << " MIDI input sources available.\n";

    for ( unsigned i=0; i<nPorts; i++ ) {
      std::string portName = midiin->getPortName(i);
      std::cout << "  Input Port #" << i+1 << ": " << portName << '\n';
    }

    // RtMidiOut constructor ... exception possible
    midiout = new RtMidiOut();

    // Check outputs.
    nPorts = midiout->getPortCount();
    std::cout << "\nThere are " << nPorts << " MIDI output ports available.\n";

    for ( unsigned i=0; i<nPorts; i++ ) {
      std::string portName = midiout->getPortName(i);
      std::cout << "  Output Port #" << i+1 << ": " << portName << std::endl;
    }
    std::cout << std::endl;

  } catch ( RtError &error ) {
    error.printMessage();
  }

  delete midiin;
  delete midiout;

  return 0;
}
