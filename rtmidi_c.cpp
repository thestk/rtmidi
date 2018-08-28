#include <string.h>
#include <stdlib.h>
#include "rtmidi_c.h"
#include "RtMidi.h"

#define RTMIDI_CLASSNAME "C interface"

/* Compile-time assertions that will break if the enums are changed in
 * the future without synchronizing them properly.  If you get (g++)
 * "error: ‘StaticAssert<b>::StaticAssert() [with bool b = false]’ is
 * private within this context", it means enums are not aligned. */
template<bool b> class StaticAssert { private: StaticAssert() {} };
template<> class StaticAssert<true>{ public: StaticAssert() {} };
#define ENUM_EQUAL(x,y) StaticAssert<(int)x==(int)y>()
class StaticAssertions { StaticAssertions() {
    ENUM_EQUAL( RT_MIDI_API_UNSPECIFIED,     RtMidi::UNSPECIFIED );
    ENUM_EQUAL( RT_MIDI_API_MACOSX_CORE,     RtMidi::MACOSX_CORE );
    ENUM_EQUAL( RT_MIDI_API_LINUX_ALSA,      RtMidi::LINUX_ALSA );
    ENUM_EQUAL( RT_MIDI_API_UNIX_JACK,       RtMidi::UNIX_JACK );
    ENUM_EQUAL( RT_MIDI_API_WINDOWS_MM,      RtMidi::WINDOWS_MM );
    ENUM_EQUAL( RT_MIDI_API_RTMIDI_DUMMY,    RtMidi::RTMIDI_DUMMY );

    ENUM_EQUAL( RT_ERROR_WARNING,            RtMidiError::WARNING );
    ENUM_EQUAL( RT_ERROR_DEBUG_WARNING,      RtMidiError::DEBUG_WARNING );
    ENUM_EQUAL( RT_ERROR_UNSPECIFIED,        RtMidiError::UNSPECIFIED );
    ENUM_EQUAL( RT_ERROR_NO_DEVICES_FOUND,   RtMidiError::NO_DEVICES_FOUND );
    ENUM_EQUAL( RT_ERROR_INVALID_DEVICE,     RtMidiError::INVALID_DEVICE );
    ENUM_EQUAL( RT_ERROR_MEMORY_ERROR,       RtMidiError::MEMORY_ERROR );
    ENUM_EQUAL( RT_ERROR_INVALID_PARAMETER,  RtMidiError::INVALID_PARAMETER );
    ENUM_EQUAL( RT_ERROR_INVALID_USE,        RtMidiError::INVALID_USE );
    ENUM_EQUAL( RT_ERROR_DRIVER_ERROR,       RtMidiError::DRIVER_ERROR );
    ENUM_EQUAL( RT_ERROR_SYSTEM_ERROR,       RtMidiError::SYSTEM_ERROR );
    ENUM_EQUAL( RT_ERROR_THREAD_ERROR,       RtMidiError::THREAD_ERROR );
}};

class CallbackProxyUserData
{
  public:
	CallbackProxyUserData (RtMidiCCallback cCallback, void *userData)
		: c_callback (cCallback), user_data (userData)
	{
	}
	RtMidiCCallback c_callback;
	void *user_data;
};

/* RtMidi API */
int rtmidi_get_compiled_api (enum RtMidiApi *apis, unsigned int apis_size)
{
    std::vector<rtmidi::ApiType> v;
    try {
        rtmidi::Midi::getCompiledApi(v);
    } catch (...) {
        return -1;
    }
    if (apis) {
        unsigned int i;
        for (i = 0; i < v.size() && i < apis_size; i++)
            apis[i] = (RtMidiApi) v[i];
        return (int)i;
    }
    // return length for NULL argument.
    return v.size();
}

void rtmidi_error (rtmidi::MidiApi *api, enum RtMidiErrorType type, const char* errorString)
{
	// std::string msg = errorString;
	api->error (RTMIDI_ERROR(errorString,(rtmidi::Error::Type)type)); /* (RtMidiError::Type) type, msg); */
}

void rtmidi_open_port (RtMidiPtr device, unsigned int portNumber, const char *portName)
{
    std::string name = portName;
    try {
	((rtmidi::Midi*)device->ptr)->openPort (portNumber, name);
    
    } catch (const RtMidiError & err) {
        device->ok  = false;
        device->msg = err.what ();
    }
}

void rtmidi_open_virtual_port (RtMidiPtr device, const char *portName)
{
    std::string name = portName;
    try {
	((rtmidi::Midi*)device->ptr)->openVirtualPort (name);
    
    } catch (const RtMidiError & err) {
        device->ok  = false;
        device->msg = err.what ();
    }

}

void rtmidi_close_port (RtMidiPtr device)
{
    try { 
	((rtmidi::Midi*)device->ptr)->closePort ();

    } catch (const RtMidiError & err) {
        device->ok  = false;
        device->msg = err.what ();
    }
}

unsigned int rtmidi_get_port_count (RtMidiPtr device)
{
    try {
	return ((rtmidi::Midi *)device->ptr)->getPortCount ();

    } catch (const RtMidiError & err) {
        device->ok  = false;
        device->msg = err.what ();
        return -1;
    }
}

const char* rtmidi_get_port_name (RtMidiPtr device, unsigned int portNumber)
{
    try {
	std::string name = ((rtmidi::Midi*)device->ptr)->getPortName (portNumber);
        return strdup (name.c_str ());
    
    } catch (const RtMidiError & err) {
        device->ok  = false;
        device->msg = err.what ();
        return "";
    }
}

/* MidiIn API */
RtMidiInPtr rtmidi_in_create_default ()
{
    RtMidiInWrapper* wrp = new RtMidiInWrapper;
    
    try {
	rtmidi::MidiIn* rIn = new rtmidi::MidiIn ();
        
        wrp->ptr = (void*) rIn;
        wrp->data = 0;
        wrp->ok  = true;
        wrp->msg = "";
    
    } catch (const RtMidiError & err) {
        wrp->ptr = 0;
        wrp->data = 0;
        wrp->ok  = false;
        wrp->msg = err.what ();
    }

    return wrp;
}

RtMidiInPtr rtmidi_in_create (enum RtMidiApi api, const char *clientName, unsigned int queueSizeLimit)
{
    std::string name = clientName;
    RtMidiInWrapper* wrp = new RtMidiInWrapper;
    
    try {
        rtmidi::MidiIn* rIn = new rtmidi::MidiIn ((rtmidi::ApiType) api, name, queueSizeLimit);
        
        wrp->ptr = (void*) rIn;
        wrp->data = 0;
        wrp->ok  = true;
        wrp->msg = "";

    } catch (const RtMidiError & err) {
        wrp->ptr = 0;
        wrp->data = 0;
        wrp->ok  = false;
        wrp->msg = err.what ();
    }

    return wrp;
}

void rtmidi_in_free (RtMidiInPtr device)
{
    if (device->data)
      delete (CallbackProxyUserData*) device->data;
    delete (rtmidi::MidiIn*) device->ptr;
    delete device;
}

enum RtMidiApi rtmidi_in_get_current_api (RtMidiInPtr device)
{
    try {
	return (RtMidiApi) ((rtmidi::MidiIn*)device->ptr)->getCurrentApi ();
    
    } catch (const RtMidiError & err) {
        device->ok  = false;
        device->msg = err.what ();

        return RT_MIDI_API_UNSPECIFIED;
    }
}

static
void callback_proxy (double timeStamp, std::vector<unsigned char> *message, void *userData)
{
	CallbackProxyUserData* data = reinterpret_cast<CallbackProxyUserData*> (userData);
	data->c_callback (timeStamp, message->data (), message->size (), data->user_data);
}

void rtmidi_in_set_callback (RtMidiInPtr device, RtMidiCCallback callback, void *userData)
{
    device->data = (void*) new CallbackProxyUserData (callback, userData);
    try {
	((rtmidi::MidiIn*)device->ptr)->setCallback (callback_proxy, device->data);
    } catch (const RtMidiError & err) {
        device->ok  = false;
        device->msg = err.what ();
        delete (CallbackProxyUserData*) device->data;
        device->data = 0;
    }
}

void rtmidi_in_cancel_callback (RtMidiInPtr device)
{
    try {
	((rtmidi::MidiIn*)device->ptr)->cancelCallback ();
        delete (CallbackProxyUserData*) device->data;
        device->data = 0;
    } catch (const RtMidiError & err) {
        device->ok  = false;
        device->msg = err.what ();
    }
}

void rtmidi_in_ignore_types (RtMidiInPtr device, bool midiSysex, bool midiTime, bool midiSense)
{
	((rtmidi::MidiIn*)device->ptr)->ignoreTypes (midiSysex, midiTime, midiSense);
}

double rtmidi_in_get_message (RtMidiInPtr device, 
                              unsigned char *message,
                              size_t *size)
{
    try {
        // FIXME: use allocator to achieve efficient buffering
        std::vector<unsigned char> v;
	double ret = ((rtmidi::MidiIn*)device->ptr)->getMessage (v);

        if (v.size () > 0 && v.size() <= *size) {
            memcpy (message, v.data (), (int) v.size ());
        }

        *size = v.size();
        return ret;
    } 
    catch (const RtMidiError & err) {
        device->ok  = false;
        device->msg = err.what ();
        return -1;
    }
    catch (...) {
        device->ok  = false;
        device->msg = "Unknown error";
        return -1;
    }
}

/* RtMidiOut API */
RtMidiOutPtr rtmidi_out_create_default ()
{
    RtMidiOutWrapper* wrp = new RtMidiOutWrapper;

    try {
	rtmidi::MidiOut* rOut = new rtmidi::MidiOut ();
        
        wrp->ptr = (void*) rOut;
        wrp->data = 0;
        wrp->ok  = true;
        wrp->msg = "";
    
    } catch (const RtMidiError & err) {
        wrp->ptr = 0;
        wrp->data = 0;
        wrp->ok  = false;
        wrp->msg = err.what ();
    }

    return wrp;
}

RtMidiOutPtr rtmidi_out_create (enum RtMidiApi api, const char *clientName)
{
    RtMidiOutWrapper* wrp = new RtMidiOutWrapper;
    std::string name = clientName;

    try {
        rtmidi::MidiOut* rOut = new rtmidi::MidiOut ((rtmidi::ApiType) api, name);
        
        wrp->ptr = (void*) rOut;
        wrp->data = 0;
        wrp->ok  = true;
        wrp->msg = "";
    
    } catch (const RtMidiError & err) {
        wrp->ptr = 0;
        wrp->data = 0;
        wrp->ok  = false;
        wrp->msg = err.what ();
    }


    return wrp;
}

void rtmidi_out_free (RtMidiOutPtr device)
{
    delete ((rtmidi::MidiOut*)device->ptr);
    delete device;
}

enum RtMidiApi rtmidi_out_get_current_api (RtMidiPtr device)
{
    try {
        return (RtMidiApi) ((rtmidi::MidiOut*)device->ptr)->getCurrentApi ();

    } catch (const RtMidiError & err) {
        device->ok  = false;
        device->msg = err.what ();

        return RT_MIDI_API_UNSPECIFIED;
    }
}

int rtmidi_out_send_message (RtMidiOutPtr device, const unsigned char *message, int length)
{
    try {
        ((RtMidiOut*) device->ptr)->sendMessage (message, length);
        return 0;
    }
    catch (const RtMidiError & err) {
        device->ok  = false;
        device->msg = err.what ();
        return -1;
    }
    catch (...) {
        device->ok  = false;
        device->msg = "Unknown error";
        return -1;
    }
}
