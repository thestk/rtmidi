module;

#ifdef RTMIDI_USE_NAMESPACE
#define RTMIDI_NAMESPACE_INLINE inline
#else
#define RTMIDI_NAMESPACE_INLINE
#endif

#include "RtMidi.h"

export module rtmidi;

export RTMIDI_NAMESPACE_INLINE namespace rtmidi {
    using rtmidi::RtMidiError;
    using rtmidi::RtMidiErrorCallback;
    using rtmidi::RtMidi;
    using rtmidi::RtMidiIn;
    using rtmidi::RtMidiOut;
    using rtmidi::MidiApi;
    using rtmidi::MidiInApi;
    using rtmidi::MidiOutApi;
}
