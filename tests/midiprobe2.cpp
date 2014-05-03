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
  std::map<int, std::string> apiMap;
  apiMap[RtMidi::MACOSX_CORE] = "OS-X CoreMidi";
  apiMap[RtMidi::WINDOWS_MM] = "Windows MultiMedia";
  apiMap[RtMidi::WINDOWS_KS] = "Windows Kernel Straming";
  apiMap[RtMidi::UNIX_JACK] = "Jack Client";
  apiMap[RtMidi::LINUX_ALSA] = "Linux ALSA";
  apiMap[RtMidi::RTMIDI_DUMMY] = "RtMidi Dummy";

  std::vector< RtMidi::Api > apis;
  RtMidi :: getCompiledApi( apis );

  std::cout << "\nCompiled APIs:\n";
  for ( unsigned int i=0; i<apis.size(); i++ )
    std::cout << "  " << apiMap[ apis[i] ] << std::endl;


  try {

    // RtMidiIn constructor ... exception possible
    RtMidiIn midiin;

    std::cout << "\nCurrent input API: " << apiMap[ midiin.getCurrentApi() ] << std::endl;


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
    RtMidiOut midiout;

    std::cout << "\nCurrent output API: " << apiMap[ midiout.getCurrentApi() ] << std::endl;

    list = midiout.getPortList();

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



  } catch ( RtMidiError &error ) {
    error.printMessage();
  }

  return 0;
}
