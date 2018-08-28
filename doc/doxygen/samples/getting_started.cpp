#include "RtMidi.h"

int main() {
  try {
    rtmidi::MidiIn midiin;
  } catch (const rtmidi::Error &error) {
    // Handle the exception here
    error.printMessage();
    // opitionally rethrow:
    // throw;
  }
  return 0;
}
