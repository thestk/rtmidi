#include <string.h>
#include <stdlib.h>
#include "rtmidi_c.h"
#include "RtMidi.h"

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
int rtmidi_get_compiled_api (enum RtMidiApi *apis) // return length for NULL argument.
{
	if (!apis) {
		std::vector<RtMidi::Api> v;
#if defined(__NO_EXCEPTIONS__)
		RtMidi::getCompiledApi (v);
		int size = v.size ();
		return size;
#else
		try {
			RtMidi::getCompiledApi (v);
			int size = v.size ();
			return size;
		} catch (...) {
			return -1;
		}
#endif
	} else {
#if defined(__NO_EXCEPTIONS__)
		std::vector<RtMidi::Api> v;
		RtMidi::getCompiledApi (v);
		for (unsigned int i = 0; i < v.size (); i++)
			apis[i] = (RtMidiApi) v.at (i);
		return 0;
#else
		try {
			std::vector<RtMidi::Api> v;
			RtMidi::getCompiledApi (v);
			for (unsigned int i = 0; i < v.size (); i++)
				apis[i] = (RtMidiApi) v.at (i);
			return 0;
		} catch (...) {
			return -1;
		}
#endif
	}
}

void rtmidi_error (MidiApi *api, enum RtMidiErrorType type, const char* errorString)
{
	std::string msg = errorString;
	api->error ((RtMidiError::Type) type, msg);
}

void rtmidi_open_port (RtMidiPtr device, unsigned int portNumber, const char *portName)
{
    std::string name = portName;
#if defined(__NO_EXCEPTIONS__)
    RtMidi* rtm = (RtMidi*) device->ptr;
	 rtm->resetError();
    rtm->openPort (portNumber, name);
    if (rtm->isError()) {
        device->ok  = false;
        device->msg = rtm->getError().what ();
    }
#else
    try {
        ((RtMidi*) device->ptr)->openPort (portNumber, name);
    
    } catch (const RtMidiError & err) {
        device->ok  = false;
        device->msg = err.what ();
    }
#endif
}

void rtmidi_open_virtual_port (RtMidiPtr device, const char *portName)
{
    std::string name = portName;
#if defined(__NO_EXCEPTIONS__)
    RtMidi* rtm = (RtMidi*) device->ptr;
	 rtm->resetError();
    rtm->openVirtualPort (name);
    if (rtm->isError()) {
        device->ok  = false;
        device->msg = rtm->getError().what ();
    }
#else
    try {
        ((RtMidi*) device->ptr)->openVirtualPort (name);
    
    } catch (const RtMidiError & err) {
        device->ok  = false;
        device->msg = err.what ();
    }
#endif

}

void rtmidi_close_port (RtMidiPtr device)
{
#if defined(__NO_EXCEPTIONS__)
    RtMidi* rtm = (RtMidi*) device->ptr;
	 rtm->resetError();
    rtm->closePort ();
    if (rtm->isError()) {
        device->ok  = false;
        device->msg = rtm->getError().what ();
    }
#else
    try { 
        ((RtMidi*) device->ptr)->closePort ();

    } catch (const RtMidiError & err) {
        device->ok  = false;
        device->msg = err.what ();
    }
#endif
}

unsigned int rtmidi_get_port_count (RtMidiPtr device)
{
#if defined(__NO_EXCEPTIONS__)
    RtMidi* rtm = (RtMidi*) device->ptr;
	 rtm->resetError();
    unsigned int ports = rtm->getPortCount();
    if (rtm->isError()) {
        device->ok  = false;
        device->msg = rtm->getError().what ();
        return -1;
    }
	 return ports;
#else
    try {
        return ((RtMidi*) device->ptr)->getPortCount ();

    } catch (const RtMidiError & err) {
        device->ok  = false;
        device->msg = err.what ();
        return -1;
    }
#endif
}

const char* rtmidi_get_port_name (RtMidiPtr device, unsigned int portNumber)
{
#if defined(__NO_EXCEPTIONS__)
    RtMidi* rtm = (RtMidi*) device->ptr;
	 rtm->resetError();
    std::string name = rtm->getPortName (portNumber);
    if (rtm->isError()) {
        device->ok  = false;
        device->msg = rtm->getError().what ();
        return "";
    }
    return strdup (name.c_str ());
#else
    try {
        std::string name = ((RtMidi*) device->ptr)->getPortName (portNumber);
        return strdup (name.c_str ());
    
    } catch (const RtMidiError & err) {
        device->ok  = false;
        device->msg = err.what ();
        return "";
    }
#endif
}

/* RtMidiIn API */
RtMidiInPtr rtmidi_in_create_default ()
{
    RtMidiWrapper* wrp = new RtMidiWrapper;

#if defined(__NO_EXCEPTIONS__)
    RtMidiIn* rIn = new RtMidiIn ();
    if (rIn->isError()) {
        wrp->ptr = 0;
        wrp->data = 0;
        wrp->ok  = false;
        wrp->msg = rIn->getError().what ();
    }
    else {
        wrp->ptr = (void*) rIn;
        wrp->data = 0;
        wrp->ok  = true;
        wrp->msg = "";
    }
#else
    try {
        RtMidiIn* rIn = new RtMidiIn ();
        
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
#endif

    return wrp;
}

RtMidiInPtr rtmidi_in_create (enum RtMidiApi api, const char *clientName, unsigned int queueSizeLimit)
{
    std::string name = clientName;
    RtMidiWrapper* wrp = new RtMidiWrapper;
    
#if defined(__NO_EXCEPTIONS__)
    RtMidiIn* rIn = new RtMidiIn ((RtMidi::Api) api, name, queueSizeLimit);
    if (rIn->isError()) {
        wrp->ptr = 0;
        wrp->data = 0;
        wrp->ok  = false;
        wrp->msg = rIn->getError().what ();
    }
    else {
        wrp->ptr = (void*) rIn;
        wrp->data = 0;
        wrp->ok  = true;
        wrp->msg = "";
    }
#else
    try {
        RtMidiIn* rIn = new RtMidiIn ((RtMidi::Api) api, name, queueSizeLimit);
        
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
#endif

    return wrp;
}

void rtmidi_in_free (RtMidiInPtr device)
{
    if (device->data)
      delete (CallbackProxyUserData*) device->data;
    delete (RtMidiIn*) device->ptr;
    delete device;
}

enum RtMidiApi rtmidi_in_get_current_api (RtMidiPtr device)
{
#if defined(__NO_EXCEPTIONS__)
    RtMidiIn* rtm = (RtMidiIn*) device->ptr;
	 rtm->resetError();
    RtMidiApi curApi = (RtMidiApi) rtm->getCurrentApi ();
    if (rtm->isError()) {
        device->ok  = false;
        device->msg = rtm->getError().what ();
        return RT_MIDI_API_UNSPECIFIED;
    }
	 return curApi;
#else
    try {
        return (RtMidiApi) ((RtMidiIn*) device->ptr)->getCurrentApi ();
    
    } catch (const RtMidiError & err) {
        device->ok  = false;
        device->msg = err.what ();

        return RT_MIDI_API_UNSPECIFIED;
    }
#endif
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
#if defined(__NO_EXCEPTIONS__)
    RtMidiIn* rtm = (RtMidiIn*) device->ptr;
	 rtm->resetError();
    rtm->setCallback (callback_proxy, device->data);
    if (rtm->isError()) {
        device->ok  = false;
        device->msg = rtm->getError().what ();
        delete (CallbackProxyUserData*) device->data;
        device->data = 0;
    }
#else
    try {
        ((RtMidiIn*) device->ptr)->setCallback (callback_proxy, device->data);
    } catch (const RtMidiError & err) {
        device->ok  = false;
        device->msg = err.what ();
        delete (CallbackProxyUserData*) device->data;
        device->data = 0;
    }
#endif
}

void rtmidi_in_cancel_callback (RtMidiInPtr device)
{
#if defined(__NO_EXCEPTIONS__)
    RtMidiIn* rtm = (RtMidiIn*) device->ptr;
	 rtm->resetError();
    rtm->cancelCallback ();
    if (rtm->isError()) {
        device->ok  = false;
        device->msg = rtm->getError().what ();
    }
    else {
        delete (CallbackProxyUserData*) device->data;
        device->data = 0;
    }
#else
    try {
        ((RtMidiIn*) device->ptr)->cancelCallback ();
        delete (CallbackProxyUserData*) device->data;
        device->data = 0;
    } catch (const RtMidiError & err) {
        device->ok  = false;
        device->msg = err.what ();
    }
#endif
}

void rtmidi_in_ignore_types (RtMidiInPtr device, bool midiSysex, bool midiTime, bool midiSense)
{
	((RtMidiIn*) device->ptr)->ignoreTypes (midiSysex, midiTime, midiSense);
}

double rtmidi_in_get_message (RtMidiInPtr device, 
                              unsigned char *message,
                              size_t *size)
{
#if defined(__NO_EXCEPTIONS__)
    // FIXME: use allocator to achieve efficient buffering
    std::vector<unsigned char> v;
    RtMidiIn* rtm = (RtMidiIn*) device->ptr;
	 rtm->resetError();
    double ret = rtm->getMessage (&v);
    if (rtm->isError()) {
        device->ok  = false;
        device->msg = rtm->getError().what ();
		  return -1;
    }

    if (v.size () > 0 && v.size() <= *size) {
        memcpy (message, v.data (), (int) v.size ());
    }

    *size = v.size();
    return ret;
#else
    try {
        // FIXME: use allocator to achieve efficient buffering
        std::vector<unsigned char> v;
        double ret = ((RtMidiIn*) device->ptr)->getMessage (&v);

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
#endif
}

/* RtMidiOut API */
RtMidiOutPtr rtmidi_out_create_default ()
{
    RtMidiWrapper* wrp = new RtMidiWrapper;

#if defined(__NO_EXCEPTIONS__)
    RtMidiOut* rOut = new RtMidiOut ();
    if (rOut->isError()) {
        wrp->ptr = 0;
        wrp->data = 0;
        wrp->ok  = false;
        wrp->msg = rOut->getError().what ();
    }
    else {
        wrp->ptr = (void*) rOut;
        wrp->data = 0;
        wrp->ok  = true;
        wrp->msg = "";
    }
#else
    try {
        RtMidiOut* rOut = new RtMidiOut ();
        
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
#endif

    return wrp;
}

RtMidiOutPtr rtmidi_out_create (enum RtMidiApi api, const char *clientName)
{
    RtMidiWrapper* wrp = new RtMidiWrapper;
    std::string name = clientName;

#if defined(__NO_EXCEPTIONS__)
    RtMidiOut* rOut = new RtMidiOut ((RtMidi::Api) api, name);
    if (rOut->isError()) {
        wrp->ptr = 0;
        wrp->data = 0;
        wrp->ok  = false;
        wrp->msg = rOut->getError().what ();
    }
    else {
        wrp->ptr = (void*) rOut;
        wrp->data = 0;
        wrp->ok  = true;
        wrp->msg = "";
    }
#else
    try {
        RtMidiOut* rOut = new RtMidiOut ((RtMidi::Api) api, name);
        
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
#endif


    return wrp;
}

void rtmidi_out_free (RtMidiOutPtr device)
{
    delete (RtMidiOut*) device->ptr;
    delete device;
}

enum RtMidiApi rtmidi_out_get_current_api (RtMidiPtr device)
{
#if defined(__NO_EXCEPTIONS__)
    RtMidiOut* rtm = (RtMidiOut*) device->ptr;
	 rtm->resetError();
    RtMidiApi curApi = (RtMidiApi) rtm->getCurrentApi ();
    if (rtm->isError()) {
        device->ok  = false;
        device->msg = rtm->getError().what ();
        return RT_MIDI_API_UNSPECIFIED;
    }
	 return curApi;
#else
    try {
        return (RtMidiApi) ((RtMidiOut*) device->ptr)->getCurrentApi ();

    } catch (const RtMidiError & err) {
        device->ok  = false;
        device->msg = err.what ();

        return RT_MIDI_API_UNSPECIFIED;
    }
#endif
}

int rtmidi_out_send_message (RtMidiOutPtr device, const unsigned char *message, int length)
{
#if defined(__NO_EXCEPTIONS__)
    RtMidiOut* rtm = (RtMidiOut*) device->ptr;
	 rtm->resetError();
    rtm->sendMessage (message, length);
    if (rtm->isError()) {
        device->ok  = false;
        device->msg = rtm->getError().what ();
        return -1;
    }
	 return 0;
#else
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
#endif
}
