module;

#include "RtMidi.h"

export module rtmidi;

export inline namespace rtmidi {
    using rtmidi::RtMidiError;
    using rtmidi::RtMidiErrorCallback;
    using rtmidi::RtMidi;
    using rtmidi::RtMidiIn;
    using rtmidi::RtMidiOut;
    using rtmidi::MidiApi;
    using rtmidi::MidiInApi;
    using rtmidi::MidiOutApi;
}
