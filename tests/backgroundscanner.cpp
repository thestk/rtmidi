#include <iostream>
#include <chrono>
#include <thread>
#include <ctime>

#include "RtMidi.h"


int main(int argc, char * argv[]){

  const unsigned int interval = 1000;
  unsigned int round = 0;

  RtMidiIn * baseThreadPort = new RtMidiIn();

  baseThreadPort->openVirtualPort("baseThreadPort");

  std::thread thr = std::thread([interval, &round]() {

    for(round = 1;; round++){

      RtMidiIn * midi = NULL;

      try {
        midi = new RtMidiIn();

        unsigned int nPorts = midi->getPortCount();

        std::cout << "Scan-Round " << std::to_string(round) << ", found " << nPorts << " ports" << std::endl;

        for ( unsigned int i=0; i<nPorts; i++ ) {

          std::string name = midi->getPortName(i);

          std::cout << "\t #" << i << " " << name << std::endl;
        }

      } catch ( RtMidiError &error ) {
        error.printMessage();
      }

      delete midi;

      std::this_thread::sleep_for(std::chrono::milliseconds(interval));
    }
  });

  // loop until CTRL-C
  std::cout << ">>>>>>>>> PRESS CTRL-C to QUIT <<<<<<<<<<" << std::endl;
  while(1){

    #ifdef __APPLE__
    RtMidi_multithreadRunLoop();
    #endif

  }

  return EXIT_SUCCESS;
}
