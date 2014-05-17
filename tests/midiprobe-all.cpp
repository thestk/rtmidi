// midiprobe.cpp
//
// Simple program to check MIDI inputs and outputs.
//
// by Gary Scavone, 2003-2012.

#include <iostream>
#include <cstdlib>
#include <map>
#include "RtMidi.h"

int main()
{
  // Create an api map.

  std::vector< RtMidi::Api > apis;
  rtmidi::Midi :: getCompiledApi( apis );

  std::cout << "\nCompiled APIs:\n";
  for ( unsigned int i=0; i<apis.size(); i++ )
	  std::cout << "  " << rtmidi::getApiName(apis[i]) << std::endl;

  try {

    // RtMidiIn constructor ... exception possible
    rtmidi::MidiIn midiin (rtmidi::ALL_API);

    std::cout << "\nCurrent input API: " << rtmidi::getApiName(midiin.getCurrentApi()) << std::endl;

    rtmidi::PortList list = midiin.getPortList();
    // Check inputs.
    std::cout << "\nThere are " << list.size() << " MIDI input sources available.\n";
    for (rtmidi::PortList::iterator i = list.begin();
	 i != list.end();
	 i++) {
	    std::cout << " Input Port: " << (*i)->getName() << std::endl;
	    for (int j = 0 ; j < 4 ; j++ ) {
		    std::cout << j << ":f:f: " << (*i)->getName (j) << std::endl;
		    std::cout << j << ":t:f: " << (*i)->getName (j | rtmidi::PortDescriptor::UNIQUE_NAME) << std::endl;
		    std::cout << j << ":f:t: " << (*i)->getName (j | rtmidi::PortDescriptor::INCLUDE_API) << std::endl;
		    std::cout << j << ":t:t: " << (*i)->getName (j | rtmidi::PortDescriptor::UNIQUE_NAME
								 | rtmidi::PortDescriptor::INCLUDE_API) << std::endl;
		    std::cout << std::endl;
	    }
	    std::cout << std::endl;
    }

    std::cout << "**********************************************************************" << std::endl;

    // RtMidiOut constructor ... exception possible
    rtmidi::MidiOut midiout (rtmidi::ALL_API);
    list = midiout.getPortList();

    std::cout << "\nCurrent output API: " << rtmidi::getApiName(midiout.getCurrentApi()) << std::endl;

    // Check inputs.
    std::cout << "\nThere are " << list.size() << " MIDI output sinks available.\n";

    for (rtmidi::PortList::iterator i = list.begin();
	 i != list.end();
	 i++) {
	    std::cout << " Output Port: " << (*i)->getName() << std::endl;
	    for (int j = 0 ; j < 4 ; j++ ) {
		    std::cout << j << ":f:f: " << (*i)->getName (j) << std::endl;
		    std::cout << j << ":t:f: " << (*i)->getName (j | rtmidi::PortDescriptor::UNIQUE_NAME) << std::endl;
		    std::cout << j << ":f:t: " << (*i)->getName (j | rtmidi::PortDescriptor::INCLUDE_API) << std::endl;
		    std::cout << j << ":t:t: " << (*i)->getName (j | rtmidi::PortDescriptor::UNIQUE_NAME
							   | rtmidi::PortDescriptor::INCLUDE_API) << std::endl;
		    std::cout << std::endl;
	    }
	    std::cout << std::endl;
    }

    std::cout << "**********************************************************************" << std::endl;
    std::cout << "*                      entering unlimited mode                       *" << std::endl;
    std::cout << "**********************************************************************" << std::endl;

    list = midiin.getPortList(rtmidi::PortDescriptor::UNLIMITED);
    // Check inputs.
    std::cout << "\nThere are " << list.size() << " MIDI input sources available in unlimited mode.\n";
    for (rtmidi::PortList::iterator i = list.begin();
	 i != list.end();
	 i++) {
	    std::cout << " Input Port: " << (*i)->getName() << std::endl;
	    for (int j = 0 ; j < 4 ; j++ ) {
		    std::cout << j << ":f:f: " << (*i)->getName (j) << std::endl;
		    std::cout << j << ":t:f: " << (*i)->getName (j | rtmidi::PortDescriptor::UNIQUE_NAME) << std::endl;
		    std::cout << j << ":f:t: " << (*i)->getName (j | rtmidi::PortDescriptor::INCLUDE_API) << std::endl;
		    std::cout << j << ":t:t: " << (*i)->getName (j | rtmidi::PortDescriptor::UNIQUE_NAME
								 | rtmidi::PortDescriptor::INCLUDE_API) << std::endl;
		    std::cout << std::endl;
	    }
	    std::cout << std::endl;
    }

    std::cout << "**********************************************************************" << std::endl;
    list = midiout.getPortList(rtmidi::PortDescriptor::UNLIMITED);

    // Check inputs.
    std::cout << "\nThere are " << list.size() << " MIDI output sinks available in unlimited mode.\n";

    for (rtmidi::PortList::iterator i = list.begin();
	 i != list.end();
	 i++) {
	    std::cout << " Output Port: " << (*i)->getName() << std::endl;
	    for (int j = 0 ; j < 4 ; j++ ) {
		    std::cout << j << ":f:f: " << (*i)->getName (j) << std::endl;
		    std::cout << j << ":t:f: " << (*i)->getName (j | rtmidi::PortDescriptor::UNIQUE_NAME) << std::endl;
		    std::cout << j << ":f:t: " << (*i)->getName (j | rtmidi::PortDescriptor::INCLUDE_API) << std::endl;
		    std::cout << j << ":t:t: " << (*i)->getName (j | rtmidi::PortDescriptor::UNIQUE_NAME
							   | rtmidi::PortDescriptor::INCLUDE_API) << std::endl;
		    std::cout << std::endl;
	    }
	    std::cout << std::endl;
    }

  } catch ( rtmidi::Error &error ) {
    error.printMessage();
  }

  return 0;
}
