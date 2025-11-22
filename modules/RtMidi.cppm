module;

#include "RtMidi.h"

export module rt.midi;

export
#ifndef RTMIDI_USE_NAMESPACE
inline namespace rt {
inline namespace midi {
#else
namespace rt::midi {
#endif
    using rt::midi::RtMidiError;
    using rt::midi::RtMidiErrorCallback;
    using rt::midi::RtMidi;
    using rt::midi::RtMidiIn;
    using rt::midi::RtMidiOut;
    using rt::midi::MidiApi;
    using rt::midi::MidiInApi;
    using rt::midi::MidiOutApi;
}
#ifndef RTMIDI_USE_NAMESPACE
}
#endif
