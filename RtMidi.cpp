/**********************************************************************/
/*! \class Midi
  \brief An abstract base class for realtime MIDI input/output.

  This class implements some common functionality for the realtime
  MIDI input/output subclasses MidiIn and MidiOut.

    RtMidi WWW site: http://music.mcgill.ca/~gary/rtmidi/

  Midi: realtime MIDI i/o C++ classes
  Copyright (c) 2003-2017 Gary P. Scavone
  Forked by Tobias Schlemmer, 2014-2018.

  Permission is hereby granted, free of charge, to any person
  obtaining a copy of this software and associated documentation files
  (the "Software"), to deal in the Software without restriction,
  including without limitation the rights to use, copy, modify, merge,
  publish, distribute, sublicense, and/or sell copies of the Software,
  and to permit persons to whom the Software is furnished to do so,
  subject to the following conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  Any person wishing to distribute modifications to the Software is
  asked to send the modifications to the original developer so that
  they can be incorporated into the canonical version.  This is,
  however, not a binding provision of this license.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
  ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
  CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
/**********************************************************************/

#include "RtMidi.h"
#include <sstream>
#include <cstring>
#include <cctype>
#include <algorithm>
#include <functional>
#ifndef RTMIDI_FALLTHROUGH
#define RTMIDI_FALLTHROUGH
#endif

#if defined(__MACOSX_COREMIDI__)
#if TARGET_OS_IPHONE
#define AudioGetCurrentHostTime CAHostTimeBase::GetCurrentTime
#define AudioConvertHostTimeToNanos CAHostTimeBase::ConvertToNanos
#endif
#endif

// Default for Windows is to add an identifier to the port names; this
// flag can be defined (e.g. in your project file) to disable this behaviour.
//#define RTMIDI_DO_NOT_ENSURE_UNIQUE_PORTNAMES

// **************************************************************** //
//
// MidiInApi and MidiOutApi subclass prototypes.
//
// **************************************************************** //

#if !defined(__LINUX_ALSA__) && !defined(__UNIX_JACK__) && !defined(__MACOSX_CORE__) && !defined(__WINDOWS_MM__)
  #define __RTMIDI_DUMMY__
#endif

RTMIDI_NAMESPACE_START
#if defined(__MACOSX_COREMIDI__)
RTMIDI_NAMESPACE_END
struct MIDIPacketList;
RTMIDI_NAMESPACE_START

class MidiInCore: public MidiInApi
{
 public:
  MidiInCore( const std::string &clientName, unsigned int queueSizeLimit );
  ~MidiInCore( void );
  ApiType getCurrentApi( void ) throw() { return rtmidi::MACOSX_CORE; };
  bool hasVirtualPorts() const { return true; }
  void openPort( unsigned int portNumber, const std::string &portName );
  void openVirtualPort( const std::string &portName );
  void openPort( const PortDescriptor & port, const std::string &portName);
  Pointer<PortDescriptor> getDescriptor(bool local=false);
  PortList getPortList(int capabilities);
  void closePort( void );
  void setClientName( const std::string &clientName );
  void setPortName( const std::string &portName );
  unsigned int getPortCount( void );
  std::string getPortName( unsigned int portNumber );

 protected:
  static void midiInputCallback( const MIDIPacketList *list,
				 void *procRef,
				 void */*srcRef*/) throw();
  void initialize( const std::string& clientName );
  template<int locking>
  friend class CoreSequencer;
};

class MidiOutCore: public MidiOutApi
{
 public:
  MidiOutCore( const std::string &clientName );
  ~MidiOutCore( void );
  ApiType getCurrentApi( void ) throw() { return rtmidi::MACOSX_CORE; };
  bool hasVirtualPorts() const { return true; }
  void openPort( unsigned int portNumber, const std::string &portName );
  void openVirtualPort( const std::string &portName );
  void openPort( const PortDescriptor & port, const std::string &portName);
  Pointer<PortDescriptor> getDescriptor(bool local=false);
  PortList getPortList(int capabilities);
  void closePort( void );
  void setClientName( const std::string &clientName );
  void setPortName( const std::string &portName );
  unsigned int getPortCount( void );
  std::string getPortName( unsigned int portNumber );
  void sendMessage( const unsigned char *message, size_t size );

 protected:
  void initialize( const std::string& clientName );
};

#endif

#if defined(__UNIX_JACK__)

class MidiInJack: public MidiInApi
{
 public:
  MidiInJack( const std::string &clientName, unsigned int queueSizeLimit );
  ~MidiInJack( void );
  ApiType getCurrentApi( void ) throw() { return rtmidi::UNIX_JACK; };
  bool hasVirtualPorts() const { return true; }
  void openPort( unsigned int portNumber, const std::string &portName );
  void openVirtualPort( const std::string &portName );
  void openPort( const PortDescriptor & port, const std::string &portName);
  Pointer<PortDescriptor> getDescriptor(bool local=false);
  PortList getPortList(int capabilities);
  void closePort( void );
  void setClientName( const std::string &clientName );
  void setPortName( const std::string &portName);
  unsigned int getPortCount( void );
  std::string getPortName( unsigned int portNumber );

 protected:
  std::string clientName;

  void connect( void );
  void initialize( const std::string& clientName );
};

class MidiOutJack: public MidiOutApi
{
 public:
  MidiOutJack( const std::string &clientName );
  ~MidiOutJack( void );
  ApiType getCurrentApi( void ) throw() { return rtmidi::UNIX_JACK; };
  bool hasVirtualPorts() const { return true; }
  void openPort( unsigned int portNumber, const std::string &portName );
  void openVirtualPort( const std::string &portName );
  void openPort( const PortDescriptor & port, const std::string &portName);
  Pointer<PortDescriptor> getDescriptor(bool local=false);
  PortList getPortList(int capabilities);
  void closePort( void );
  void setClientName( const std::string &clientName );
  void setPortName( const std::string &portName);
  unsigned int getPortCount( void );
  std::string getPortName( unsigned int portNumber );
  void sendMessage( const unsigned char *message, size_t size );

 protected:
  std::string clientName;

  void connect( void );
  void initialize( const std::string& clientName );
};

#endif

#if defined(__LINUX_ALSA__)

class MidiInAlsa: public MidiInApi
{
 public:
  MidiInAlsa( const std::string &clientName, unsigned int queueSizeLimit );
  ~MidiInAlsa( void );
  ApiType getCurrentApi( void ) throw() { return rtmidi::LINUX_ALSA; };
  bool hasVirtualPorts() const { return true; }
  void openPort( unsigned int portNumber, const std::string &portName );
  void openVirtualPort( const std::string &portName );
  void openPort( const PortDescriptor & port, const std::string &portName);
  Pointer<PortDescriptor> getDescriptor(bool local=false);
  PortList getPortList(int capabilities);
  void closePort( void );
  void setClientName( const std::string &clientName );
  void setPortName( const std::string &portName);
  unsigned int getPortCount( void );
  std::string getPortName( unsigned int portNumber );

 protected:
  static void * alsaMidiHandler( void *ptr ) throw();
  void initialize( const std::string &clientName );
  friend struct AlsaMidiData;
};

class MidiOutAlsa: public MidiOutApi
{
 public:
  MidiOutAlsa( const std::string &clientName );
  ~MidiOutAlsa( void );
  ApiType getCurrentApi( void ) throw() { return rtmidi::LINUX_ALSA; };
  bool hasVirtualPorts() const { return true; }
  void openPort( unsigned int portNumber, const std::string &portName );
  void openVirtualPort( const std::string &portName );
  void openPort( const PortDescriptor & port, const std::string &portName);
  Pointer<PortDescriptor> getDescriptor(bool local=false);
  PortList getPortList(int capabilities);
  void closePort( void );
  void setClientName( const std::string &clientName );
  void setPortName( const std::string &portName);
  unsigned int getPortCount( void );
  std::string getPortName( unsigned int portNumber );
  void sendMessage( const unsigned char *message, size_t size );

 protected:
  void initialize( const std::string& clientName );
};

#endif

#if defined(__WINDOWS_MM__)

class MidiInWinMM: public MidiInApi
{
 public:
  MidiInWinMM( const std::string &clientName, unsigned int queueSizeLimit );
  ~MidiInWinMM( void );
  ApiType getCurrentApi( void ) throw() { return rtmidi::WINDOWS_MM; };
  bool hasVirtualPorts() const { return false; }
  void openPort( unsigned int portNumber, const std::string &portName );
  void openVirtualPort( const std::string &portName );
  void openPort( const PortDescriptor & port, const std::string &portName);
  Pointer<PortDescriptor> getDescriptor(bool local=false);
  PortList getPortList(int capabilities);
  void closePort( void );
  void setClientName( const std::string &clientName );
  void setPortName( const std::string &portName );
  unsigned int getPortCount( void );
  std::string getPortName( unsigned int portNumber );

 protected:
  void initialize( const std::string& clientName );
 friend struct WinMMCallbacks;
};

class MidiOutWinMM: public MidiOutApi
{
 public:
  MidiOutWinMM( const std::string &clientName );
  ~MidiOutWinMM( void );
  ApiType getCurrentApi( void ) throw() { return rtmidi::WINDOWS_MM; };
  bool hasVirtualPorts() const { return false; }
  void openPort( unsigned int portNumber, const std::string &portName );
  void openVirtualPort( const std::string &portName );
  void openPort( const PortDescriptor & port, const std::string &portName);
  Pointer<PortDescriptor> getDescriptor(bool local=false);
  PortList getPortList(int capabilities);
  void closePort( void );
  void setClientName( const std::string &clientName );
  void setPortName( const std::string &portName );
  unsigned int getPortCount( void );
  std::string getPortName( unsigned int portNumber );
  void sendMessage( const unsigned char *message, size_t size );

 protected:
  void initialize( const std::string& clientName );
};

#endif

#if defined(__RTMIDI_DUMMY__)

#define RTMIDI_CLASSNAME "MidiInDummy"
class MidiInDummy: public MidiInApi
{
 public:
 MidiInDummy( const std::string &/*clientName*/, unsigned int queueSizeLimit )
   : MidiInApi( queueSizeLimit ) {
    error( RTMIDI_ERROR(rtmidi_gettext("No valid MIDI interfaces. I'm using a dummy input interface that never receives anything."),
			Error::WARNING) );
 }
  ApiType getCurrentApi( void ) throw() { return rtmidi::RTMIDI_DUMMY; }
  bool hasVirtualPorts() const { return false; }
  void openPort( unsigned int /*portNumber*/, const std::string &/*portName*/ ) {}
  void openVirtualPort( const std::string &/*portName*/ ) {}
  void openPort( const PortDescriptor & /* port */,
		 const std::string &/* portName */) {}
  Pointer<PortDescriptor> getDescriptor(bool /* local=false */) {
    return 0;
  }
  PortList getPortList(int /* capabilities */) {
    return PortList();
  }
  void closePort( void ) {}
  void setClientName( const std::string &/*clientName*/ ) {};
  void setPortName( const std::string &/*portName*/ ) {};
  unsigned int getPortCount( void ) { return 0; }
  std::string getPortName( unsigned int /*portNumber*/ ) { return ""; }

 protected:
  void initialize( const std::string& /*clientName*/ ) {}
};

class MidiOutDummy: public MidiOutApi
{
 public:
  MidiOutDummy( const std::string &/*clientName*/ ) {
    error( RTMIDI_ERROR(rtmidi_gettext("No valid MIDI interfaces. I'm using a dummy output interface that does nothing."),
			Error::WARNING) );
  }
  ApiType getCurrentApi( void ) throw() { return rtmidi::RTMIDI_DUMMY; }
  bool hasVirtualPorts() const { return false; }
  void openPort( unsigned int /*portNumber*/, const std::string &/*portName*/ ) {}
  void openVirtualPort( const std::string &/*portName*/ ) {}
  void openPort( const PortDescriptor & /* port */,
		 const std::string &/* portName */) {}
  Pointer<PortDescriptor> getDescriptor(bool /* local=false */) {
    return 0;
  }
  PortList getPortList(int /* capabilities */) {
    return PortList();
  }
  void closePort( void ) {}
  void setClientName( const std::string &/*clientName*/ ) {};
  void setPortName( const std::string &/*portName*/ ) {};
  unsigned int getPortCount( void ) { return 0; }
  std::string getPortName( unsigned int /*portNumber*/ ) { return ""; }
  void sendMessage( const unsigned char * /*message*/, size_t /*size*/ ) {}

 protected:
  void initialize( const std::string& /*clientName*/ ) {}
};
#undef RTMIDI_CLASSNAME

#endif


//*********************************************************************//
//  RtMidi Definitions
//*********************************************************************//

#ifdef RTMIDI_GETTEXT
const char * rtmidi_gettext (const char * s) {
  init_rtmidi_gettext();
  return dgettext("rtmidi",s);
}

void init_rtmidi_gettext() {
  static bool initialized = false;
  if (initialized)
    return;
  bindtextdomain("rtmidi",LOCALEDIR);
  initialized = true;
}
#endif

//! The constructor.
Error::Error( const char * message,
	      Type type,
	      const char * class_name,
	      const char * function_name,
	      const char * file_name,
	      int line_number, ...) throw():exception(),
					    classname(class_name),
					    function(function_name),
					    file(file_name),
					    line(line_number),
					    type_(type)
{
#ifdef RTMIDI_GETTEXT
  message = rtmidi_gettext(message);
#endif
  std::va_list args;
  va_start(args,line_number);
  size_t length;
  length = vsnprintf(NULL,0,message,args);
  if (length > 0) {
    message_.resize(length+1);
    std::vsnprintf(&(message_[0]),length+1,message,args);
    message_.resize(length);
  } else {
    const char * fmt = gettext_noopt("Error formatting the error string:\n'%s'\nFound in %s::%s at \n%s:%d");
#ifdef RTMIDI_GETTEXT
    fmt = rtmidi_gettext(fmt);
#endif

    length = snprintf(NULL,0,fmt,message,class_name,function_name,file_name,line);
    if (length > 0) {
      message_.resize(length+1);
      snprintf(&(message_[0]),length+1,fmt,message,class_name,function_name,file_name,line);
      message_.resize(length);
    } else {
      const char * msg
	= gettext_noopt("Error: could not format the error message");
#ifdef RTMIDI_GETTEXT
      msg = rtmidi_gettext(msg);
#endif
      message_ = msg;
    }
  }
  va_end(args);

}

//*********************************************************************//
//  Midi Definitions
//*********************************************************************//
std::string Midi :: getVersion( void ) throw()
{
  return std::string( RTMIDI_VERSION );
}

void Midi :: getCompiledApi( std::vector<ApiType> &apis, bool preferSystem ) throw()
{
  apis.clear();

  // The order here will control the order of RtMidi's API search in
  // the constructor.

  if (!preferSystem) {
    // first check software and network backends
#if defined(__UNIX_JACK__)
    apis.push_back( rtmidi::UNIX_JACK );
#endif
  }

  // check OS provided backends
#if defined(__MACOSX_COREMIDI__)
  apis.push_back( rtmidi::MACOSX_CORE );
#endif
#if defined(__LINUX_ALSA__)
  apis.push_back( rtmidi::LINUX_ALSA );
#endif
#if defined(__WINDOWS_MM__)
  apis.push_back( rtmidi::WINDOWS_MM );
#endif

  if (preferSystem) {
    // if we prefer OS provided backends,
    // we should add the software backends, here.
#if defined(__UNIX_JACK__)
    apis.push_back( rtmidi::UNIX_JACK );
#endif
  }

  // DUMMY is a no-backend class so we add it at
  // the very end.
#if defined(__RTMIDI_DUMMY__)
  apis.push_back( rtmidi::DUMMY );
#endif
}



void Midi :: error(Error e)
{

  // use the callback if present.
  if (rtapi_) {
    rtapi_->error(e);
    return;
  }

  if ( e.getType() == Error::WARNING ) {
    e.printMessage();
  }
  else if ( e.getType() == Error::DEBUG_WARNING ) {
#if defined(__RTMIDI_DEBUG__)
    e.printMessage();
#endif
  }
  else {
    e.printMessage();
    throw e;
  }
}


//*********************************************************************//
//  MidiIn Definitions
//*********************************************************************//
#define RTMIDI_CLASSNAME "MidiIn"
void MidiIn :: openMidiApi( ApiType api )
{
  delete rtapi_;
  rtapi_ = 0;

  try {
    switch (api) {
    case rtmidi::UNIX_JACK:
#if defined(__UNIX_JACK__)
      rtapi_ = new MidiInJack( clientName, queueSizeLimit );
#endif
      break;
    case rtmidi::LINUX_ALSA:
#if defined(__LINUX_ALSA__)
      rtapi_ = new MidiInAlsa( clientName, queueSizeLimit );
#endif
      break;
    case rtmidi::WINDOWS_MM:
#if defined(__WINDOWS_MM__)
      rtapi_ = new MidiInWinMM( clientName, queueSizeLimit );
#endif
      break;
    case rtmidi::MACOSX_CORE:
#if defined(__MACOSX_COREMIDI__)
      rtapi_ = new MidiInCore( clientName, queueSizeLimit );
#endif
      break;
    case rtmidi::DUMMY:
#if defined(__RTMIDI_DUMMY__)
      rtapi_ = new MidiInDummy( clientName, queueSizeLimit );
#endif
      break;
    case rtmidi::ALL_API:
    case rtmidi::UNSPECIFIED:
    default:
      break;
    }
  } catch (const Error & e) {
    rtapi_ = 0;
    throw;
  }
}

MidiApiList MidiIn::queryApis;

RTMIDI_DLL_PUBLIC MidiIn :: MidiIn( ApiType api,
					   const std::string &clientName,
					   unsigned int queueSize,
					   bool pfsystem )
  : Midi(&queryApis,pfsystem,clientName),
    queueSizeLimit(queueSize)
{
  if ( api == rtmidi::ALL_API) {
    if (!queryApis.empty()) {
      rtapi_ = NULL;
      return;
    }

    std::vector< ApiType > apis;
    getCompiledApi( apis );
    for ( unsigned int i=0; i<apis.size(); i++ ) {
      try {
	openMidiApi( apis[i] );
	if ( rtapi_ ) {
	  queryApis.push_back(MidiApiPtr(rtapi_));
	  rtapi_=NULL;
	}
      } catch (const Error & e) {
	if (e.getType() != Error::NO_DEVICES_FOUND)
	  throw;
      }
    }
    return;
  }

  if ( api != rtmidi::UNSPECIFIED ) {
    // Attempt to open the specified API.
    openMidiApi( api );
    if ( rtapi_ ) return;

    // No compiled support for specified API value.  Issue a warning
    // and continue as if no API was specified.
    throw RTMIDI_ERROR1(gettext_noopt("Support for the selected MIDI system %d has not been compiled into the RtMidi library."),
			Error::INVALID_PARAMETER,
			api);
  }

  // Iterate through the compiled APIs and return as soon as we find
  // one with at least one port or we reach the end of the list.
  std::vector< ApiType > apis;
  getCompiledApi( apis );
  for ( unsigned int i=0; i<apis.size(); i++ ) {
    openMidiApi( apis[i] );
    if ( rtapi_ && rtapi_->getPortCount() ) break;
  }

  if ( rtapi_ ) return;

  // We may reach this point if the only API is JACK,
  // but no JACK devices are found.
  throw( RTMIDI_ERROR( gettext_noopt("No supported MIDI system has been found."),
		       Error::NO_DEVICES_FOUND ) );
}

MidiIn :: ~MidiIn() throw()
{
}
#undef RTMIDI_CLASSNAME


//*********************************************************************//
//  MidiOut Definitions
//*********************************************************************//

#define RTMIDI_CLASSNAME "MidiOut"
void MidiOut :: openMidiApi( ApiType api )
{
  delete rtapi_;
  rtapi_ = 0;

  try {
    switch (api) {
    case rtmidi::UNIX_JACK:
#if defined(__UNIX_JACK__)
      rtapi_ = new MidiOutJack( clientName );
#endif
      break;
    case rtmidi::LINUX_ALSA:
#if defined(__LINUX_ALSA__)
      rtapi_ = new MidiOutAlsa( clientName );
#endif
      break;
    case rtmidi::WINDOWS_MM:
#if defined(__WINDOWS_MM__)
      rtapi_ = new MidiOutWinMM( clientName );
#endif
      break;
    case rtmidi::MACOSX_CORE:
#if defined(__MACOSX_COREMIDI__)
      rtapi_ = new MidiOutCore( clientName );
#endif
      break;
    case rtmidi::DUMMY:
#if defined(__RTMIDI_DUMMY__)
      rtapi_ = new MidiOutDummy( clientName );
#endif
      break;
    case rtmidi::UNSPECIFIED:
    case rtmidi::ALL_API:
    default:
      break;
    }
  } catch (const Error & e) {
    rtapi_ = 0;
    throw;
  }

}


MidiApiList MidiOut::queryApis;

RTMIDI_DLL_PUBLIC MidiOut :: MidiOut( ApiType api, const std::string &clientName, bool pfsystem )
  : Midi(&queryApis, pfsystem, clientName)
{
  if ( api == rtmidi::ALL_API) {
    if (!queryApis.empty()) {
      rtapi_ = NULL;
      return;
    }

    std::vector< ApiType > apis;
    getCompiledApi( apis );
    for ( unsigned int i=0; i<apis.size(); i++ ) {
      try {
	openMidiApi( apis[i] );
	if ( rtapi_ ) {
	  queryApis.push_back(MidiApiPtr(rtapi_));
	  rtapi_ = NULL;
	}
      } catch (const Error & e) {
	if (e.getType() != Error::NO_DEVICES_FOUND)
	  throw;
      }
    }
    return;
  }

  if ( api != rtmidi::UNSPECIFIED ) {
    // Attempt to open the specified API.
    openMidiApi( api );
    if ( rtapi_ ) return;

    // No compiled support for specified API value.  Issue a warning
    // and continue as if no API was specified.
    throw RTMIDI_ERROR1(gettext_noopt("Support for the selected MIDI system %d has not been compiled into the RtMidi library."),
			Error::INVALID_PARAMETER, api);
  }

  // Iterate through the compiled APIs and return as soon as we find
  // one with at least one port or we reach the end of the list.
  std::vector< ApiType > apis;
  getCompiledApi( apis );
  for ( unsigned int i=0; i<apis.size(); i++ ) {
    openMidiApi( apis[i] );
    if ( rtapi_ && rtapi_->getPortCount() ) break;
  }

  if ( rtapi_ ) return;

  // We may reach this point, e.g. if JACK is the only
  // compiled API, but no JACK devices are found.
  throw( RTMIDI_ERROR(gettext_noopt("No supported MIDI system has been found."),
		      Error::NO_DEVICES_FOUND ) );
}

MidiOut :: ~MidiOut() throw()
{
}
#undef RTMIDI_CLASSNAME


MidiApi :: MidiApi( void )
  : apiData_( 0 ),
    connected_( false ),
    firstErrorOccurred_ (false),
    errorCallback_(0)
{
}

MidiApi :: ~MidiApi( void )
{
}

void MidiApi :: setErrorCallback( ErrorCallback errorCallback, void *userData )
{
  errorCallback_ = new CompatibilityErrorInterface(errorCallback,userData);
}

void MidiApi :: setErrorCallback(ErrorInterface * callback) {
  errorCallback_ = callback;
}


void MidiApi :: error(Error e)
{
  if ( errorCallback_ ) {

    if ( firstErrorOccurred_ )
      return;

    firstErrorOccurred_ = true;
    std::ostringstream s;
    e.printMessage(s);

    errorCallback_->rtmidi_error(e);
    firstErrorOccurred_ = false;
    return;
  }

  if ( e.getType() == Error::WARNING ) {
    e.printMessage();
  }
  else if ( e.getType() == Error::DEBUG_WARNING ) {
#if defined(__RTMIDI_DEBUG__)
    e.printMessage();
#endif
  }
  else {
    e.printMessage();
    throw e;
  }
}

//*********************************************************************//
//  Common MidiInApi Definitions
//*********************************************************************//

#define RTMIDI_CLASSNAME "MidiInApi"
MidiInApi :: MidiInApi( unsigned int queueSizeLimit )
  : MidiApi(), ignoreFlags(7), doInput(false), firstMessage(true),
    userCallback(0),
    continueSysex(false)
{
  // Allocate the MIDI queue.
  queue.ringSize = queueSizeLimit;
  if ( queue.ringSize > 0 )
    queue.ring = new MidiMessage[ queue.ringSize ];
}

MidiInApi :: ~MidiInApi( void )
{
  if (userCallback)
    userCallback->delete_me();
  // Delete the MIDI queue.
  if ( queue.ringSize > 0 ) delete [] queue.ring;
}

void MidiInApi :: setCallback( MidiCallback callback, void *userData )
{
  if ( userCallback ) {
    error(RTMIDI_ERROR(gettext_noopt("A callback function is already set."),
		       Error::WARNING));
    return;
  }

  if ( !callback ) {
    error(RTMIDI_ERROR(gettext_noopt("The callback function value is invalid."),
		       Error::WARNING));
    return;
  }

  userCallback = new CompatibilityMidiInterface(callback,userData);
}

void MidiInApi :: setCallback( MidiInterface * callback )
{
  if ( userCallback ) {
    error(RTMIDI_ERROR(gettext_noopt("A callback function is already set."),
		       Error::WARNING));
    return;
  }

  if ( !callback ) {
    error(RTMIDI_ERROR(gettext_noopt("The callback function value is invalid."),
		       Error::WARNING));
    return;
  }

  userCallback = callback;
}

void MidiInApi :: cancelCallback()
{
  if ( !userCallback ) {
    error(RTMIDI_ERROR(gettext_noopt("No callback function was set."),
		       Error::WARNING));
    return;
  }

  userCallback->delete_me();
  userCallback = 0;
}

void MidiInApi :: ignoreTypes( bool midiSysex, bool midiTime, bool midiSense )
{
  ignoreFlags = 0;
  if ( midiSysex ) ignoreFlags = 0x01;
  if ( midiTime ) ignoreFlags |= 0x02;
  if ( midiSense ) ignoreFlags |= 0x04;
}

double MidiInApi :: getMessage( std::vector<unsigned char> &message )
{
  message.clear();

  if ( userCallback ) {
    error(RTMIDI_ERROR(gettext_noopt("Returning an empty MIDI message as all input is handled by a callback function."),
		       Error::WARNING));
    return 0.0;
  }

  double timeStamp;
  if (!queue.pop(message, timeStamp))
    return 0.0;

  return timeStamp;
}

unsigned int MidiInApi::MidiQueue::size(unsigned int *__back,
					unsigned int *__front)
{
  // Access back/front members exactly once and make stack copies for
  // size calculation
  unsigned int _back = back, _front = front, _size;
  if (_back >= _front)
    _size = _back - _front;
  else
    _size = ringSize - _front + _back;

  // Return copies of back/front so no new and unsynchronized accesses
  // to member variables are needed.
  if (__back) *__back = _back;
  if (__front) *__front = _front;
  return _size;
}

// As long as we haven't reached our queue size limit, push the message.
bool MidiInApi::MidiQueue::push(const MidiInApi::MidiMessage& msg)
{
  // Local stack copies of front/back
  unsigned int _back, _front, _size;

  // Get back/front indexes exactly once and calculate current size
  _size = size(&_back, &_front);

  if ( _size < ringSize-1 )
  {
    ring[_back] = msg;
    back = (back+1)%ringSize;
    return true;
  }

  return false;
}

bool MidiInApi::MidiQueue::pop(std::vector<unsigned char> &msg, double& timeStamp)
{
  // Local stack copies of front/back
  unsigned int _back, _front, _size;

  // Get back/front indexes exactly once and calculate current size
  _size = size(&_back, &_front);

  if (_size == 0)
    return false;

  // Copy queued message to the vector pointer argument and then "pop" it.
  msg.assign( ring[_front].bytes.begin(), ring[_front].bytes.end() );
  timeStamp = ring[_front].timeStamp;

  // Update front
  front = (front+1)%ringSize;
  return true;
}
#undef RTMIDI_CLASSNAME

//*********************************************************************//
//  Common MidiOutApi Definitions
//*********************************************************************//

#define RTMIDI_CLASSNAME "MidiOutApi"
MidiOutApi :: MidiOutApi( void )
  : MidiApi()
{
}

MidiOutApi :: ~MidiOutApi( void )
{
}


// trim from start
static inline std::string &ltrim(std::string &s) {
  s.erase(s.begin(),
	  std::find_if(s.begin(), s.end(), [](int ch) {
	      return ! std::isspace(ch);
	    }));
  return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
	return ! std::isspace(ch);
      }).base(), s.end());
  return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
  return ltrim(rtrim(s));
}
RTMIDI_NAMESPACE_END
#undef RTMIDI_CLASSNAME

// *************************************************** //
//
// OS/API-specific methods.
//
// *************************************************** //

#if defined(__MACOSX_COREMIDI__)

// The CoreMIDI API is based on the use of a callback function for
// MIDI input.  We convert the system specific time stamps to delta
// time values.

// OS-X CoreMIDI header files.
#include <CoreMIDI/CoreMIDI.h>
#include <CoreAudio/HostTime.h>
#include <CoreServices/CoreServices.h>

RTMIDI_NAMESPACE_START
/*! An abstraction layer for the CORE sequencer layer. It provides
  the following functionality:
  - dynamic allocation of the sequencer
  - optionallay avoid concurrent access to the CORE sequencer,
  which is not thread proof. This feature is controlled by
  the parameter \ref locking.
*/

/* A wrapper for temporary CFString objects */
class CFStringWrapper {
public:
  CFStringWrapper(const std::string &s):
    cfname(CFStringCreateWithCStringNoCopy( NULL,
					    s.c_str(),
					    kCFStringEncodingUTF8,
					    kCFAllocatorNull))
  {}
  CFStringWrapper(const char * s):
    cfname(CFStringCreateWithCStringNoCopy( NULL,
					    s,
					    kCFStringEncodingUTF8,
					    kCFAllocatorNull))
  {}
  ~CFStringWrapper() {
    CFRelease(cfname);
  }

  operator CFStringRef&() { return cfname; }
protected:
  CFStringRef cfname;
};

// This function was submitted by Douglas Casey Tucker and apparently
// derived largely from PortMidi.
// or copied from the Apple developer Q&A website
// https://developer.apple.com/library/mac/qa/qa1374/_index.html

CFStringRef EndpointName( MIDIEndpointRef endpoint, bool isExternal )
{
  CFMutableStringRef result = CFStringCreateMutable( NULL, 0 );
  CFStringRef str;

  // Begin with the endpoint's name.
  str = NULL;
  MIDIObjectGetStringProperty( endpoint, kMIDIPropertyName, &str );
  if ( str != NULL ) {
    CFStringAppend( result, str );
    CFRelease( str );
  }

  MIDIEntityRef entity = 0;
  MIDIEndpointGetEntity( endpoint, &entity );
  if ( entity == 0 )
    // probably virtual
    return result;

  if ( CFStringGetLength( result ) == 0 ) {
    // endpoint name has zero length -- try the entity
    str = NULL;
    MIDIObjectGetStringProperty( entity, kMIDIPropertyName, &str );
    if ( str != NULL ) {
      CFStringAppend( result, str );
      CFRelease( str );
    }
  }
  // now consider the device's name
  MIDIDeviceRef device = 0;
  MIDIEntityGetDevice( entity, &device );
  if ( device == 0 )
    return result;

  str = NULL;
  MIDIObjectGetStringProperty( device, kMIDIPropertyName, &str );
  if ( CFStringGetLength( result ) == 0 ) {
    CFRelease( result );
    return str;
  }
  if ( str != NULL ) {
    // if an external device has only one entity, throw away
    // the endpoint name and just use the device name
    if ( isExternal && MIDIDeviceGetNumberOfEntities( device ) < 2 ) {
      CFRelease( result );
      return str;
    } else {
      if ( CFStringGetLength( str ) == 0 ) {
	CFRelease( str );
	return result;
      }
      // does the entity name already start with the device name?
      // (some drivers do this though they shouldn't)
      // if so, do not prepend
      if ( CFStringCompareWithOptions( result, /* endpoint name */
				       str /* device name */,
				       CFRangeMake(0, CFStringGetLength( str ) ), 0 ) != kCFCompareEqualTo ) {
	// prepend the device name to the entity name
	if ( CFStringGetLength( result ) > 0 )
	  CFStringInsert( result, 0, CFSTR(" ") );
	CFStringInsert( result, 0, str );
      }
      CFRelease( str );
    }
  }
  return result;
}

// This function was submitted by Douglas Casey Tucker and apparently
// derived largely from PortMidi.
// Nearly the same text can be found in the Apple Q&A qa1374:
// https://developer.apple.com/library/mac/qa/qa1374/_index.html
static CFStringRef ConnectedEndpointName( MIDIEndpointRef endpoint )
{
  CFMutableStringRef result = CFStringCreateMutable( NULL, 0 );
  CFStringRef str;
  OSStatus err;
  int i;

  // Does the endpoint have connections?
  CFDataRef connections = NULL;
  int nConnected = 0;
  bool anyStrings = false;
  err = MIDIObjectGetDataProperty( endpoint, kMIDIPropertyConnectionUniqueID, &connections );
  if ( connections != NULL ) {
    // It has connections, follow them
    // Concatenate the names of all connected devices
    nConnected = CFDataGetLength( connections ) / sizeof(MIDIUniqueID);
    if ( nConnected ) {
      const SInt32 *pid = (const SInt32 *)(CFDataGetBytePtr(connections));
      for ( i=0; i<nConnected; ++i, ++pid ) {
	MIDIUniqueID id = EndianS32_BtoN( *pid );
	MIDIObjectRef connObject;
	MIDIObjectType connObjectType;
	err = MIDIObjectFindByUniqueID( id, &connObject, &connObjectType );
	if ( err == noErr ) {
	  if ( connObjectType == kMIDIObjectType_ExternalSource  ||
	       connObjectType == kMIDIObjectType_ExternalDestination ) {
	    // Connected to an external device's endpoint (10.3 and later).
	    str = EndpointName( (MIDIEndpointRef)(connObject), true );
	  } else {
	    // Connected to an external device (10.2) (or something else, catch-
	    str = NULL;
	    MIDIObjectGetStringProperty( connObject, kMIDIPropertyName, &str );
	  }
	  if ( str != NULL ) {
	    if ( anyStrings )
	      CFStringAppend( result, CFSTR(", ") );
	    else anyStrings = true;
	    CFStringAppend( result, str );
	    CFRelease( str );
	  }
	}
      }
    }
    CFRelease( connections );
  }
  if ( anyStrings )
    return result;

  CFRelease( result );

  // Here, either the endpoint had no connections, or we failed to obtain names
  return EndpointName( endpoint, false );
}


#define RTMIDI_CLASSNAME "CoreSequencer"
template <int locking=1>
class CoreSequencer {
public:
  CoreSequencer():seq(0)
  {
    if (locking) {
      pthread_mutexattr_t attr;
      pthread_mutexattr_init(&attr);
      pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
      pthread_mutex_init(&mutex, &attr);
    }
  }

  CoreSequencer(const std::string &n):seq(0),name(n)
  {
    if (locking) {
      pthread_mutexattr_t attr;
      pthread_mutexattr_init(&attr);
      pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
      pthread_mutex_init(&mutex, &attr);
    }
    init();
  }

  ~CoreSequencer()
  {
    if (seq) {
      scoped_lock lock(mutex);
      MIDIClientDispose(seq);
      seq = 0;
    }
    if (locking) {
      pthread_mutex_destroy(&mutex);
    }
  }

  bool setName(const std::string &n) {
    /* we don't want to rename the client after opening it. */
    if (seq) return false;
    name = n;
    return true;
  }

  static std::string str(CFStringRef s) {
    const char * cstr =
      CFStringGetCStringPtr(s,kCFStringEncodingUTF8);
    if (cstr) return cstr;

    CFIndex len = CFStringGetLength(s);
    std::string retval;
    retval.resize(CFStringGetMaximumSizeForEncoding(len,
						    kCFStringEncodingUTF8)+1);
    CFStringGetBytes(s,
		     CFRangeMake(0, len),
		     kCFStringEncodingUTF8,
		     0,
		     false,
		     reinterpret_cast<uint8*>(&retval[0]),
		     retval.size()-1,
		     &len);
    retval.resize(len);
    return trim(retval);
  }


#if 0
  // Obtain the name of an endpoint, following connections.

  // The result should be released by the caller.

  static CFStringRef CreateConnectedEndpointName(MIDIEndpointRef endpoint)
  {
    CFMutableStringRef result = CFStringCreateMutable(NULL, 0);
    CFStringRef str;
    OSStatus err;


    // Does the endpoint have connections?
    CFDataRef connections = NULL;
    int nConnected = 0;
    bool anyStrings = false;
    err = MIDIObjectGetDataProperty(endpoint, kMIDIPropertyConnectionUniqueID, &connections);
    if (connections != NULL) {
      // It has connections, follow them
      // Concatenate the names of all connected devices
      nConnected = CFDataGetLength(connections) / sizeof(MIDIUniqueID);

      if (nConnected) {
	const SInt32 *pid = reinterpret_cast<const SInt32 *>(CFDataGetBytePtr(connections));
	for (int i = 0; i < nConnected; ++i, ++pid) {
	  MIDIUniqueID id = EndianS32_BtoN(*pid);
	  MIDIObjectRef connObject;
	  MIDIObjectType connObjectType;
	  err = MIDIObjectFindByUniqueID(id, &connObject, &connObjectType);
	  if (err == noErr) {
	    if (connObjectType == kMIDIObjectType_ExternalSource  ||
		connObjectType == kMIDIObjectType_ExternalDestination) {
	      // Connected to an external device's endpoint (10.3 and later).
	      str = EndpointName(static_cast<MIDIEndpointRef>(connObject), true);
	    } else {
	      // Connected to an external device (10.2) (or something else, catch-all)
	      str = NULL;
	      MIDIObjectGetStringProperty(connObject, kMIDIPropertyName, &str);
	    }

	    if (str != NULL) {
	      if (anyStrings)
		CFStringAppend(result, CFSTR(", "));
	      else anyStrings = true;
	      CFStringAppend(result, str);
	      CFRelease(str);
	    }
	  }
	}
      }
      CFRelease(connections);
    }


    if (anyStrings)
      return result;
    else
      CFRelease(result);

    // Here, either the endpoint had no connections, or we failed to obtain names for any of them.
    return CreateEndpointName(endpoint, false);
  }



  //////////////////////////////////////
  // Obtain the name of an endpoint without regard for whether it has connections.
  // The result should be released by the caller.

  static CFStringRef CreateEndpointName(MIDIEndpointRef endpoint, bool isExternal)
  {
    CFMutableStringRef result = CFStringCreateMutable(NULL, 0);
    CFStringRef str;

    // begin with the endpoint's name
    str = NULL;
    MIDIObjectGetStringProperty(endpoint, kMIDIPropertyName, &str);
    if (str != NULL) {
      CFStringAppend(result, str);
      CFRelease(str);
    }

    MIDIEntityRef entity = NULL;
    MIDIEndpointGetEntity(endpoint, &entity);
    if (entity == NULL)
      // probably virtual
      return result;

    if (CFStringGetLength(result) == 0) {
      // endpoint name has zero length -- try the entity
      str = NULL;
      MIDIObjectGetStringProperty(entity, kMIDIPropertyName, &str);
      if (str != NULL) {
	CFStringAppend(result, str);
	CFRelease(str);
      }
    }



    // now consider the device's name
    MIDIDeviceRef device = NULL;
    MIDIEntityGetDevice(entity, &device);
    if (device == NULL) return result;

    str = NULL;
    MIDIObjectGetStringProperty(device, kMIDIPropertyName, &str);
    if (str != NULL) {
      // if an external device has only one entity, throw away
      // the endpoint name and just use the device name
      if (isExternal && MIDIDeviceGetNumberOfEntities(device) < 2) {
	CFRelease(result);
	return str;
      } else {
	// does the entity name already start with the device name?
	// (some drivers do this though they shouldn't)
	// if so, do not prepend

	if (CFStringCompareWithOptions(str /* device name */,
				       result /* endpoint name */,
				       CFRangeMake(0,
						   CFStringGetLength(str)),
				       0)
	    != kCFCompareEqualTo) {
	  // prepend the device name to the entity name
	  if (CFStringGetLength(result) > 0)
	    CFStringInsert(result, 0, CFSTR(" "));
	  CFStringInsert(result, 0, str);
	}
	CFRelease(str);
      }
    }

    return result;
  }
#endif

  static std::string getConnectionsString(MIDIEndpointRef port)
  {
    /* This function is derived from
       CreateConnectedEndpointName at Apple Q&A */
    std::ostringstream result;
    CFDataRef connections = NULL;
    OSStatus err = MIDIObjectGetDataProperty(port,
					     kMIDIPropertyConnectionUniqueID,
					     &connections);
    if (err != noErr)
      return result.str();

    if (!connections)
      return result.str();
    CFIndex size = CFDataGetLength( connections ) / sizeof(MIDIUniqueID);
    if (!size) {
      CFRelease(connections);
      return result.str();
    }

    CFStringRef strRef;
    const SInt32 *pid
      = reinterpret_cast<const SInt32 *>(CFDataGetBytePtr(connections));
    bool anyStrings = false;
    for (int i = 0; i < size; ++i, ++pid) {
      MIDIUniqueID id = EndianS32_BtoN(*pid);
      MIDIObjectRef connObject;
      MIDIObjectType connObjectType;
      err = MIDIObjectFindByUniqueID(id, &connObject, &connObjectType);
      if (err != noErr)
	continue;

      if (connObjectType == kMIDIObjectType_ExternalSource  ||
	  connObjectType == kMIDIObjectType_ExternalDestination) {
	// Connected to an external
	// device's endpoint
	// (10.3 and later).
	strRef = EndpointName(static_cast<MIDIEndpointRef>(connObject),
			      true);
      } else {
	// Connected to an external device
	// (10.2) (or something else, catch-all)
	strRef = NULL;
	MIDIObjectGetStringProperty(connObject,
				    kMIDIPropertyName, &strRef);
      }

      if (strRef != NULL) {
	if (anyStrings)
	  result << ", ";
	else anyStrings = true;
	result << str(strRef);
	CFRelease(strRef);
      }
    }
    CFRelease(connections);
    return result.str();
  }

  static std::string getPortName(MIDIEndpointRef port, int flags) {
    //			std::string clientname;
    std::string devicename;
    std::string portname;
    std::string entityname;
    //			std::string externaldevicename;
    std::string connections;
    std::string recommendedname;
    //			bool isVirtual;
    bool hasManyEntities = false;
    bool hasManyEndpoints = false;
    CFStringRef nameRef;
    MIDIObjectGetStringProperty(port,
				kMIDIPropertyDisplayName,
				&nameRef);
    recommendedname = str(nameRef);
    connections = getConnectionsString(port);

    MIDIObjectGetStringProperty(port,
				kMIDIPropertyName,
				&nameRef);
    portname = str(nameRef);
    CFRelease( nameRef );

    MIDIEntityRef entity = 0;
    MIDIEndpointGetEntity(port, &entity);
    // entity == NULL: probably virtual
    if (entity != 0) {
      nameRef = NULL;
      MIDIObjectGetStringProperty(entity, kMIDIPropertyName, &nameRef);
      if (nameRef != NULL) {
	entityname = str(nameRef);
	CFRelease(nameRef);
      }
      hasManyEndpoints =
	MIDIEntityGetNumberOfSources(entity) >= 2 ||
	MIDIEntityGetNumberOfDestinations(entity)
	>= 2;

      // now consider the device's name
      MIDIDeviceRef device = 0;
      MIDIEntityGetDevice(entity, &device);
      if (device != 0) {
	hasManyEntities = MIDIDeviceGetNumberOfEntities(device) >= 2;
	MIDIObjectGetStringProperty(device,
				    kMIDIPropertyName,
				    &nameRef);
	devicename = str(nameRef);
	CFRelease(nameRef);
      }
      // does the entity name already start with the device name?
      // (some drivers do this though they shouldn't)
      if (entityname.substr(0,devicename.length())
	  == devicename) {
	int start = devicename.length();
	while (isspace(entityname[start]))
	  start++;
	entityname = entityname.substr(start);
      }
    }

    int naming = flags & PortDescriptor::NAMING_MASK;

    std::ostringstream os;
    bool needcolon;
    switch (naming) {
    case PortDescriptor::SESSION_PATH:
      if (flags & PortDescriptor::INCLUDE_API)
	os << "CORE:";
      os << port;
      break;
    case PortDescriptor::STORAGE_PATH:
      if (flags & PortDescriptor::INCLUDE_API)
	os << "CORE:";
      // os << clientname;
      os << devicename;
      os << ":" << portname;
      os << ":" << entityname;
      //				os << ":" << externaldevicename;
      os << ":" << connections;
      //				os << ":" << recommendedname;
      if (flags & PortDescriptor::UNIQUE_PORT_NAME)
	os << ";" << port;
      break;
    case PortDescriptor::LONG_NAME:
      needcolon = !devicename.empty();
      os << devicename;
      if (hasManyEndpoints ||
	  hasManyEntities ||
	  devicename.empty()) {
	if (!entityname.empty()) {
	  if (needcolon)
	    os << ": ";
	  os << entityname;
	  needcolon = true;
	}
	if ((hasManyEndpoints
	     || entityname.empty())
	    && !portname.empty()) {
	  if (needcolon)
	    os << ": ";
	  os << portname;
	}
      }
      if (!connections.empty()) {
	os << " ⇒ ";
	os << connections;
      }
      if (flags &
	  (PortDescriptor::INCLUDE_API
	   | PortDescriptor::UNIQUE_PORT_NAME)) {
	os << " (";
	if (flags &
	    PortDescriptor::INCLUDE_API) {
	  os << "CORE";
	  if (flags & PortDescriptor::UNIQUE_PORT_NAME)
	    os << ":";
	}
	if (flags & PortDescriptor::UNIQUE_PORT_NAME) {
	  os << port;
	}
	os << ")";
      }
      break;
    case PortDescriptor::SHORT_NAME:
    default:
      if (!recommendedname.empty()) {
	os << recommendedname;
      } else
	if (!connections.empty()) {
	  os << connections;
	} else {
	  os << devicename;
	  if (hasManyEntities ||
	      hasManyEndpoints ||
	      devicename.empty()) {
	    if (!devicename.empty())
	      os << " ";
	    if (!portname.empty()) {
	      os << portname;
	    } else  if (!entityname.empty()) {
	      os << entityname;
	    } else
	      os << "???";
	  }
	}
      if (flags &
	  (PortDescriptor::INCLUDE_API
	   | PortDescriptor::UNIQUE_PORT_NAME)) {
	os << " (";
	if (flags &
	    PortDescriptor::INCLUDE_API) {
	  os << "CORE";
	  if (flags & PortDescriptor::UNIQUE_PORT_NAME)
	    os << ":";
	}
	if (flags & PortDescriptor::UNIQUE_PORT_NAME) {
	  os << port;
	}
	os << ")";
      }
      break;
    }
    return os.str();
  }

  int getPortCapabilities(MIDIEndpointRef port) {
    int retval = 0;
    MIDIEntityRef entity = 0;
    OSStatus stat =
      MIDIEndpointGetEntity(port,&entity);
    if (stat == kMIDIObjectNotFound) {
      // plan B for virtual ports
      MIDIUniqueID uid;
      stat = MIDIObjectGetIntegerProperty (port,
					   kMIDIPropertyUniqueID,
					   &uid);
      if (stat != noErr) {
	throw RTMIDI_ERROR(gettext_noopt("Could not get the unique identifier of a midi endpoint."),
			   Error::WARNING);
	return 0;
      }
      MIDIObjectRef obj;
      MIDIObjectType type;
      stat = MIDIObjectFindByUniqueID (uid,
				       &obj,
				       &type);
      if (stat != noErr || obj != port) {
	throw RTMIDI_ERROR(gettext_noopt("Could not get the endpoint back from the unique identifier of a midi endpoint."),
			   Error::WARNING);
	return 0;
      }
      if (type == kMIDIObjectType_Source
	  || type == kMIDIObjectType_ExternalSource)
	return PortDescriptor::INPUT;
      else if (type == kMIDIObjectType_Destination
	       || type == kMIDIObjectType_ExternalDestination)
	return PortDescriptor::OUTPUT;
      else {
	return 0;
      }

    } else if (stat != noErr) {
      throw RTMIDI_ERROR(gettext_noopt("Could not get the entity of a midi endpoint."),
			 Error::WARNING);
      return 0;
    }
    /* Theoretically Mac OS X could silently use
       the same endpoint reference for input and
       output. We might benefit from this
       behaviour.
       \todo: Find a way to query the object
       whether it can act as source or destination.
    */
    ItemCount count =
      MIDIEntityGetNumberOfDestinations(entity);
    for (ItemCount i = 0; i < count ; i++) {
      MIDIEndpointRef dest=
	MIDIEntityGetDestination(entity,i);
      if (dest == port) {
	retval |=
	  PortDescriptor::OUTPUT;
	break;
      }
    }
    count =
      MIDIEntityGetNumberOfSources(entity);
    for (ItemCount i = 0; i < count ; i++) {
      MIDIEndpointRef src=
	MIDIEntityGetSource(entity,i);
      if (src == port) {
	retval |=
	  PortDescriptor::INPUT;
      }
    }
    return retval;
  }


  MIDIPortRef createPort (const std::string &portName,
			  int flags,
			  MidiInCore * data = NULL)
  {
    init();
    scoped_lock lock (mutex);
    MIDIPortRef port = 0;
    OSStatus result;
    switch (flags) {
    case PortDescriptor::INPUT: {
      result = MIDIInputPortCreate(seq,
				   CFStringWrapper(portName),
				   MidiInCore::midiInputCallback,
				   (void *)data,
				   &port);
    }
      break;
    case PortDescriptor::OUTPUT: {
      result
	= MIDIOutputPortCreate(seq,
			       CFStringWrapper(portName),
			       &port);
    }
      break;
    default:
      throw RTMIDI_ERROR(gettext_noopt("Error creating OS X MIDI port because of invalid port flags."),
			 Error::INVALID_PARAMETER);
    }
    if ( result != noErr ) {
      throw RTMIDI_ERROR(gettext_noopt("Error creating OS-X MIDI port."),
			 Error::DRIVER_ERROR);
    }
    return port;
  }

  MIDIEndpointRef createVirtualPort (const std::string &portName,
				     int flags,
				     MidiInCore * data = NULL)
  {
    init();
    scoped_lock lock (mutex);
    MIDIEndpointRef port = 0;
    OSStatus result;
    switch (flags) {
    case PortDescriptor::INPUT: {
      result
	= MIDIDestinationCreate(seq,
				CFStringWrapper(portName),
				MidiInCore::midiInputCallback,
				(void *)data,
				&port);
    }
      break;
    case PortDescriptor::OUTPUT: {
      result
	= MIDISourceCreate(seq,
			   CFStringWrapper(portName),
			   &port);
    }
      break;
    default:
      throw RTMIDI_ERROR(gettext_noopt("Error creating OS X MIDI port because of invalid port flags."),
			 Error::INVALID_PARAMETER);
    }
    if ( result != noErr ) {
      throw RTMIDI_ERROR(gettext_noopt("Error creating OS-X MIDI port."),
			 Error::DRIVER_ERROR);
    }
    return port;
  }


  /*! Use CoreSequencer like a C pointer.
    \note This function breaks the design to control thread safety
    by the selection of the \ref locking parameter to the class.
    It should be removed as soon as possible in order ensure the
    thread policy that has been intended by creating this class.
  */
  operator MIDIClientRef ()
  {
    return seq;
  }
protected:
  struct scoped_lock {
    pthread_mutex_t * mutex;
    scoped_lock(pthread_mutex_t & m): mutex(&m)
    {
      if (locking)
	pthread_mutex_lock(mutex);
    }
    ~scoped_lock()
    {
      if (locking)
	pthread_mutex_unlock(mutex);
    }
  };
  pthread_mutex_t mutex;
  MIDIClientRef seq;
  std::string name;


  void init()
  {
    init (seq);
  }

  void init(MIDIClientRef &client)
  {
    if (client) return;
    {
      scoped_lock lock(mutex);

      OSStatus result = MIDIClientCreate(CFStringWrapper(name), NULL, NULL, &client );
      if ( result != noErr ) {
	throw RTMIDI_ERROR1(gettext_noopt("Error creating OS-X MIDI client object (Error no: %d)."),
			    Error::DRIVER_ERROR,
			    result);
	return;
      }
    }
  }
};
#undef RTMIDI_CLASSNAME

typedef CoreSequencer<1> LockingCoreSequencer;
typedef CoreSequencer<0> NonLockingCoreSequencer;

#define RTMIDI_CLASSNAME "CorePortDescriptor"
struct CorePortDescriptor:public PortDescriptor	{
  CorePortDescriptor(const std::string &name):api(0),
					       clientName(name),
					       endpoint(0)
  {
  }
  CorePortDescriptor(MIDIEndpointRef p,
		     const std::string &name):api(0),
					       clientName(name),
					       endpoint(p)
  {
    seq.setName(name);
  }
  CorePortDescriptor(CorePortDescriptor &
		     other):PortDescriptor(other),
			    api(other.api),
			    clientName(other.clientName),
			    endpoint(other.endpoint)
  {
    seq.setName(clientName);
  }
  ~CorePortDescriptor() {}

  MidiInApi * getInputApi(unsigned int queueSizeLimit = 100) const {
    if (getCapabilities() & INPUT)
      return new MidiInCore(clientName,queueSizeLimit);
    else
      return 0;
  }

  MidiOutApi * getOutputApi() const {
    if (getCapabilities() & OUTPUT)
      return new MidiOutCore(clientName);
    else
      return 0;
  }

  void setEndpoint(MIDIEndpointRef e)
  {
    endpoint = e;
  }
  MIDIEndpointRef getEndpoint() const
  {
    return endpoint;
  }

  std::string getName(int flags = SHORT_NAME | UNIQUE_PORT_NAME) {
    return seq.getPortName(endpoint,flags);
  }

  const std::string &getClientName() {
    return clientName;
  }
  int getCapabilities() const {
    if (!endpoint) return 0;
    return seq.getPortCapabilities(endpoint);
  }
  static PortList getPortList(int capabilities, const std::string &clientName);
protected:
  MidiApi * api;
  static LockingCoreSequencer seq;

  std::string clientName;
  MIDIEndpointRef endpoint;
};

LockingCoreSequencer CorePortDescriptor::seq;



PortList CorePortDescriptor :: getPortList(int capabilities, const std::string &clientName)
{
  PortList list;

  CFRunLoopRunInMode( kCFRunLoopDefaultMode, 0, false );
  int caps = capabilities & PortDescriptor::INOUTPUT;
  // bool unlimited = capabilities & PortDescriptor::UNLIMITED;
  bool forceInput = PortDescriptor::INPUT & caps;
  bool forceOutput = PortDescriptor::OUTPUT & caps;
  bool allowOutput = forceOutput || !forceInput;
  bool allowInput = forceInput || !forceOutput;
  if (allowOutput) {
    ItemCount count =
      MIDIGetNumberOfDestinations();
    for (ItemCount i = 0 ; i < count; i++) {
      MIDIEndpointRef destination =
	MIDIGetDestination(i);
      try {
	if ((seq.getPortCapabilities(destination)
	     & caps) == caps)
	  list.push_back(Pointer<PortDescriptor>(
						 new CorePortDescriptor(destination, clientName)));
      } catch (Error & e) {
	if (e.getType() == Error::WARNING ||
	    e.getType() == Error::DEBUG_WARNING)
	  e.printMessage();
	else throw;
      }
    }
    // Combined sources and destinations
    // should be both occur as destinations and as
    // sources. So we have finished the search, here.
  } else if (allowInput) {
    ItemCount count =
      MIDIGetNumberOfSources();
    for (ItemCount i = 0 ; i < count; i++) {
      MIDIEndpointRef src =
	MIDIGetSource(i);
      try {
	if ((seq.getPortCapabilities(src)
	     & caps) == caps)
	  list.push_back(Pointer<PortDescriptor>(
						 new CorePortDescriptor(src, clientName)));
      } catch (Error & e) {
	if (e.getType() == Error::WARNING ||
	    e.getType() == Error::DEBUG_WARNING)
	  e.printMessage();
	else throw;
      }
    }
  }
  return list;
}
#undef RTMIDI_CLASSNAME


#define RTMIDI_CLASSNAME "CoreMidiData"
// A structure to hold variables related to the CoreMIDI API
// implementation.
struct CoreMidiData:public CorePortDescriptor {
  CoreMidiData(const std::string &clientname):CorePortDescriptor(clientname),
					       client(clientname),
					       localEndpoint(0),
					       localPort(0) {}
  ~CoreMidiData() {
    if (localEndpoint)
      MIDIEndpointDispose(localEndpoint);
    localEndpoint = 0;
  }

  void openPort(const std::string &name,
		int flags,
		MidiInCore * data = NULL) {
    localPort = client.createPort(name, flags, data);
  }

  void setRemote(const CorePortDescriptor & remote)
  {
    setEndpoint(remote.getEndpoint());
  }

  NonLockingCoreSequencer client;
  MIDIEndpointRef localEndpoint;
  MIDIPortRef localPort;
  unsigned long long lastTime;
  MIDISysexSendRequest sysexreq;
};
#undef RTMIDI_CLASSNAME


//*********************************************************************//
//  API: OS-X
//  Class Definitions: MidiInCore
//*********************************************************************//

#define RTMIDI_CLASSNAME "MidiInCore"
void MidiInCore::midiInputCallback( const MIDIPacketList *list,
				    void *procRef,
				    void */*srcRef*/ ) throw()
{
  MidiInCore *data = static_cast<MidiInCore *> (procRef);
  CoreMidiData *apiData = static_cast<CoreMidiData *> (data->apiData_);

  unsigned char status;
  unsigned short nBytes, iByte, size;
  unsigned long long time;

  bool& continueSysex = data->continueSysex;
  MidiInApi::MidiMessage& message = data->message;

  const MIDIPacket *packet = &list->packet[0];
  for ( unsigned int i=0; i<list->numPackets; ++i ) {

    // My interpretation of the CoreMIDI documentation: all message
    // types, except sysex, are complete within a packet and there may
    // be several of them in a single packet.  Sysex messages can be
    // broken across multiple packets and PacketLists but are bundled
    // alone within each packet (these packets do not contain other
    // message types).  If sysex messages are split across multiple
    // MIDIPacketLists, they must be handled by multiple calls to this
    // function.

    nBytes = packet->length;
    if ( nBytes == 0 ) {
      packet = MIDIPacketNext(packet);
      continue;
    }

    // Calculate time stamp.

    if ( data->firstMessage ) {
      message.timeStamp = 0.0;
      data->firstMessage = false;
    }
    else {
      time = packet->timeStamp;
      if ( time == 0 ) { // this happens when receiving asynchronous sysex messages
	time = AudioGetCurrentHostTime();
      }
      time -= apiData->lastTime;
      time = AudioConvertHostTimeToNanos( time );
      if ( !continueSysex )
	message.timeStamp = time * 0.000000001;
    }
    // Track whether any non-filtered messages were found in this
    // packet for timestamp calculation
    bool foundNonFiltered = false;
    //std::cout << "TimeStamp = " << packet->timeStamp << std::endl;

    iByte = 0;
    if ( continueSysex ) {
      // We have a continuing, segmented sysex message.
      if ( !( data->ignoreFlags & 0x01 ) ) {
	// If we're not ignoring sysex messages, copy the entire packet.
	for ( unsigned int j=0; j<nBytes; ++j )
	  message.bytes.push_back( packet->data[j] );
      }
      continueSysex = packet->data[nBytes-1] != 0xF7;

      if ( !( data->ignoreFlags & 0x01 ) ) {
	if ( !continueSysex ) {
	  // If not a continuing sysex message, invoke the user callback function or queue the message.
	  if ( data->userCallback ) {
	    data->userCallback->rtmidi_midi_in( message.timeStamp,
						message.bytes);
	  }
	  else {
	    // As long as we haven't reached our queue size limit, push the message.
	    if (!data->queue.push(message)) {
	      try {
		data->error(RTMIDI_ERROR(rtmidi_gettext("Error: Message queue limit reached."),
					 Error::WARNING));
	      } catch (Error & e) {
		// don't bother ALSA with an unhandled exception
	      }
	    }
	  }
	  message.bytes.clear();
	}
      }
    }
    else {
      while ( iByte < nBytes ) {
	size = 0;
	// We are expecting that the next byte in the packet is a status byte.
	status = packet->data[iByte];
	if ( !(status & 0x80) ) break;
	// Determine the number of bytes in the MIDI message.
	if ( status < 0xC0 ) size = 3;
	else if ( status < 0xE0 ) size = 2;
	else if ( status < 0xF0 ) size = 3;
	else if ( status == 0xF0 ) {
	  // A MIDI sysex
	  if ( data->ignoreFlags & 0x01 ) {
	    size = 0;
	    iByte = nBytes;
	  }
	  else size = nBytes - iByte;
	  continueSysex = packet->data[nBytes-1] != 0xF7;
	}
	else if ( status == 0xF1 ) {
	  // A MIDI time code message
	  if ( data->ignoreFlags & 0x02 ) {
	    size = 0;
	    iByte += 2;
	  }
	  else size = 2;
	}
	else if ( status == 0xF2 ) size = 3;
	else if ( status == 0xF3 ) size = 2;
	else if ( status == 0xF8 && ( data->ignoreFlags & 0x02 ) ) {
	  // A MIDI timing tick message and we're ignoring it.
	  size = 0;
	  iByte += 1;
	}
	else if ( status == 0xFE && ( data->ignoreFlags & 0x04 ) ) {
	  // A MIDI active sensing message and we're ignoring it.
	  size = 0;
	  iByte += 1;
	}
	else size = 1;

	// Copy the MIDI data to our vector.
	if ( size ) {
	  foundNonFiltered = true;
	  message.bytes.assign( &packet->data[iByte], &packet->data[iByte+size] );
	  if ( !continueSysex ) {
	    // If not a continuing sysex message, invoke the user callback function or queue the message.
	    if ( data->userCallback ) {
	      data->userCallback->rtmidi_midi_in( message.timeStamp,
						  message.bytes);
	    }
	    else {
	      // As long as we haven't reached our queue size limit, push the message.
	      if (!data->queue.push(message)) {
		try {
		  data->error(RTMIDI_ERROR(rtmidi_gettext("Error: Message queue limit reached."),
					   Error::WARNING));
		} catch (Error & e) {
		  // don't bother WinMM with an unhandled exception
		}
	      }
	    }
	    message.bytes.clear();
	  }
	  iByte += size;
	}
      }
    }
    packet = MIDIPacketNext(packet);

    // Save the time of the last non-filtered message
    if (foundNonFiltered)
    {
      apiData->lastTime = packet->timeStamp;
      if ( apiData->lastTime == 0 ) { // this happens when receiving asynchronous sysex messages
        apiData->lastTime = AudioGetCurrentHostTime();
      }
    }

  }
}

MidiInCore :: MidiInCore( const std::string &clientName,
			  unsigned int queueSizeLimit ) :
  MidiInApi( queueSizeLimit )
{
  MidiInCore::initialize( clientName );
}

MidiInCore :: ~MidiInCore( void )
{
  // Cleanup.
  CoreMidiData *data = static_cast<CoreMidiData *> (apiData_);
  try {
    // Close a connection if it exists.
    MidiInCore::closePort();

  } catch (Error & e) {
    delete data;
    throw;
  }
  delete data;

}

void MidiInCore :: initialize( const std::string &clientName )
{
  // Save our api-specific connection information.
  CoreMidiData *data = (CoreMidiData *) new CoreMidiData(clientName);
  apiData_ = (void *) data;
}

void MidiInCore :: openPort( unsigned int portNumber,
			     const std::string &portName )
{
  if ( connected_ ) {
    error(RTMIDI_ERROR(gettext_noopt("A valid connection already exists."),
		       Error::WARNING));
    return;
  }

  CFRunLoopRunInMode( kCFRunLoopDefaultMode, 0, false );
  unsigned int nSrc = MIDIGetNumberOfSources();
  if (nSrc < 1) {
    error(RTMIDI_ERROR(gettext_noopt("No MIDI input sources found."),
		       Error::NO_DEVICES_FOUND));
    return;
  }

  if ( portNumber >= nSrc ) {
    std::ostringstream ost;
    ost << "";
    errorString_ = ost.str();
    error(RTMIDI_ERROR1(gettext_noopt("The 'portNumber' argument (%d) is invalid."),
			Error::INVALID_PARAMETER, portNumber) );
    return;
  }

  MIDIPortRef port;
  CoreMidiData *data = static_cast<CoreMidiData *> (apiData_);
  OSStatus result = MIDIInputPortCreate( data->client,
					 CFStringWrapper(portName),
					 midiInputCallback, (void *)this, &port );
  if ( result != noErr ) {
    MIDIClientDispose( data->client );
    error(RTMIDI_ERROR(gettext_noopt("Error creating OS-X MIDI input port."),
		       Error::DRIVER_ERROR));
    return;
  }

  // Get the desired input source identifier.
  MIDIEndpointRef endpoint = MIDIGetSource( portNumber );
  if ( endpoint == 0 ) {
    MIDIPortDispose( port );
    port = 0;
    MIDIClientDispose( data->client );
    error(RTMIDI_ERROR(gettext_noopt("Error getting MIDI input source reference."),
		       Error::DRIVER_ERROR) );
    return;
  }

  // Make the connection.
  result = MIDIPortConnectSource( port, endpoint, NULL );
  if ( result != noErr ) {
    MIDIPortDispose( port );
    port = 0;
    MIDIClientDispose( data->client );
    error(RTMIDI_ERROR(gettext_noopt("Error connecting OS-X MIDI input port."),
		       Error::DRIVER_ERROR) );
    return;
  }

  // Save our api-specific port information.
  data->localPort = port;
  data->setEndpoint(endpoint);

  connected_ = true;
}

void MidiInCore :: openVirtualPort( const std::string &portName )
{
  CoreMidiData *data = static_cast<CoreMidiData *> (apiData_);

  // Create a virtual MIDI input destination.
  MIDIEndpointRef endpoint;
  OSStatus result = MIDIDestinationCreate( data->client,
					   CFStringWrapper(portName),
					   midiInputCallback, (void *)this, &endpoint );
  if ( result != noErr ) {
    error(RTMIDI_ERROR(gettext_noopt("Error creating virtual OS-X MIDI destination."),
		       Error::DRIVER_ERROR) );
    return;
  }

  // Save our api-specific connection information.
  data->localEndpoint = endpoint;
}

void MidiInCore :: openPort( const PortDescriptor & port,
			     const std::string &portName)
{
  CoreMidiData *data = static_cast<CoreMidiData *> (apiData_);
  const CorePortDescriptor * remote = dynamic_cast<const CorePortDescriptor *>(&port);

  if ( !data ) {
    error(RTMIDI_ERROR(gettext_noopt("Data has not been allocated."),
		       Error::SYSTEM_ERROR) );
    return;
  }
  if ( connected_ || data -> localEndpoint) {
    error(RTMIDI_ERROR(gettext_noopt("A valid connection already exists."),
		       Error::WARNING) );
    return;
  }
  if (!remote) {
    error(RTMIDI_ERROR(gettext_noopt("Core MIDI has been instructed to open a non-Core MIDI port. This doesn't work."),
		       Error::INVALID_DEVICE) );
    return;
  }

  data->openPort (portName,
		  PortDescriptor::INPUT,
		  this);
  data->setRemote(*remote);
  OSStatus result =
    MIDIPortConnectSource(data->localPort,
			  data->getEndpoint(),
			  NULL);
  if ( result != noErr ) {
    error(RTMIDI_ERROR(gettext_noopt("Error creating OS-X MIDI port."),
		       Error::DRIVER_ERROR));
  }

  connected_ = true;
}

Pointer<PortDescriptor> MidiInCore :: getDescriptor(bool local)
{
  CoreMidiData *data = static_cast<CoreMidiData *>
    (apiData_);
  if (!data) {
    return NULL;
  }
  if (local) {
    if (data && data->localEndpoint) {
      return Pointer<PortDescriptor>(new
				     CorePortDescriptor(data->localEndpoint, data->getClientName()));
    }
  } else {
    if (data->getEndpoint()) {
      return Pointer<PortDescriptor>(new CorePortDescriptor(*data));
    }
  }
  return NULL;
}

PortList MidiInCore :: getPortList(int capabilities)
{
  CoreMidiData *data = static_cast<CoreMidiData *> (apiData_);
  try {
    return CorePortDescriptor::getPortList(capabilities | PortDescriptor::INPUT,
					   data->getClientName());
  } catch (Error & e) {
    error(e);
    return PortList();
  }
}

void MidiInCore :: closePort( void )
{
  CoreMidiData *data = static_cast<CoreMidiData *> (apiData_);

  if ( data->localPort ) {
    MIDIPortDispose( data->localPort );
    data->localPort = 0;
  }

  if (data->localEndpoint) {
    MIDIEndpointDispose( data->localEndpoint );
    data->localEndpoint = 0;
  }

  connected_ = false;
}

void MidiInCore :: setClientName ( const std::string& )
{
  error(RTMIDI_ERROR(gettext_noopt("Setting client names is not implemented for Mac OS X CoreMIDI."),
		     Error::WARNING));
}

void MidiInCore :: setPortName ( const std::string& )
{
  error(RTMIDI_ERROR(gettext_noopt("Setting port names is not implemented for Mac OS X CoreMIDI."),
		     Error::WARNING));
}

unsigned int MidiInCore :: getPortCount()
{
  CFRunLoopRunInMode( kCFRunLoopDefaultMode, 0, false );
  return MIDIGetNumberOfSources();
}

std::string MidiInCore :: getPortName( unsigned int portNumber )
{
  CFStringRef nameRef;
  MIDIEndpointRef portRef;
  char name[128];

  std::string stringName;
  CFRunLoopRunInMode( kCFRunLoopDefaultMode, 0, false );
  if ( portNumber >= MIDIGetNumberOfSources() ) {
    error(RTMIDI_ERROR1(gettext_noopt("The 'portNumber' argument (%d) is invalid."),
			Error::WARNING, portNumber));
    return stringName;
  }

  portRef = MIDIGetSource( portNumber );
  nameRef = ConnectedEndpointName(portRef);
  CFStringGetCString( nameRef, name, sizeof(name), kCFStringEncodingUTF8);
  CFRelease( nameRef );

  return stringName = name;
}
#undef RTMIDI_CLASSNAME


//*********************************************************************//
//  API: OS-X
//  Class Definitions: MidiOutCore
//*********************************************************************//

#define RTMIDI_CLASSNAME "MidiOutCore"
MidiOutCore :: MidiOutCore( const std::string &clientName ) : MidiOutApi()
{
  MidiOutCore::initialize( clientName );
}

MidiOutCore :: ~MidiOutCore( void )
{
  // Close a connection if it exists.
  MidiOutCore::closePort();

  // Cleanup.
  CoreMidiData *data = static_cast<CoreMidiData *> (apiData_);
  delete data;
}

void MidiOutCore :: initialize( const std::string &clientName )
{
  // Save our api-specific connection information.
  CoreMidiData *data = (CoreMidiData *) new CoreMidiData(clientName);
  apiData_ = (void *) data;
}

unsigned int MidiOutCore :: getPortCount()
{
  CFRunLoopRunInMode( kCFRunLoopDefaultMode, 0, false );
  return MIDIGetNumberOfDestinations();
}

std::string MidiOutCore :: getPortName( unsigned int portNumber )
{
  CFStringRef nameRef;
  MIDIEndpointRef portRef;
  char name[128];

  std::string stringName;
  CFRunLoopRunInMode( kCFRunLoopDefaultMode, 0, false );
  if ( portNumber >= MIDIGetNumberOfDestinations() ) {
    error(RTMIDI_ERROR1(gettext_noopt("The 'portNumber' argument (%d) is invalid."),
			Error::WARNING, portNumber) );
    return stringName;
  }

  portRef = MIDIGetDestination( portNumber );
  nameRef = ConnectedEndpointName(portRef);
  CFStringGetCString( nameRef, name, sizeof(name), kCFStringEncodingUTF8 );
  CFRelease( nameRef );

  return stringName = name;
}

void MidiOutCore :: openPort( unsigned int portNumber,
			      const std::string &portName )
{
  if ( connected_ ) {
    error(RTMIDI_ERROR(gettext_noopt("A valid connection already exists."),
		       Error::WARNING) );
    return;
  }

  CFRunLoopRunInMode( kCFRunLoopDefaultMode, 0, false );
  unsigned int nDest = MIDIGetNumberOfDestinations();
  if (nDest < 1) {
    error(RTMIDI_ERROR(gettext_noopt("No MIDI output destinations found."),
		       Error::NO_DEVICES_FOUND) );
    return;
  }

  if ( portNumber >= nDest ) {
    error(RTMIDI_ERROR1(gettext_noopt("The 'portNumber' argument (%d) is invalid."),
			Error::INVALID_PARAMETER, portNumber) );
    return;
  }

  MIDIPortRef port;
  CoreMidiData *data = static_cast<CoreMidiData *> (apiData_);
  OSStatus result = MIDIOutputPortCreate( data->client,
					  CFStringWrapper( portName ),
					  &port );
  if ( result != noErr ) {
    MIDIClientDispose( data->client );
    error(RTMIDI_ERROR(gettext_noopt("Error creating OS-X MIDI output port."),
		       Error::DRIVER_ERROR) );
    return;
  }

  // Get the desired output port identifier.
  MIDIEndpointRef destination = MIDIGetDestination( portNumber );
  if ( destination == 0 ) {
    MIDIPortDispose( port );
    port = 0;
    MIDIClientDispose( data->client );
    error(RTMIDI_ERROR(gettext_noopt("Error getting MIDI output destination reference."),
		       Error::DRIVER_ERROR) );
    return;
  }

  // Save our api-specific connection information.
  data->localPort = port;
  data->setEndpoint(destination);
  connected_ = true;
}

void MidiOutCore :: closePort( void )
{
  CoreMidiData *data = static_cast<CoreMidiData *> (apiData_);

  if ( data->localPort ) {
    MIDIPortDispose( data->localPort );
    data->localPort = 0;
  }

  if (data->localEndpoint) {
    MIDIEndpointDispose( data->localEndpoint );
    data->localEndpoint = 0;
  }

  connected_ = false;
}

void MidiOutCore :: setClientName ( const std::string& )
{
  error(RTMIDI_ERROR(gettext_noopt("Setting client names is not implemented for Mac OS X CoreMIDI."),
		     Error::WARNING));
}

void MidiOutCore :: setPortName ( const std::string& )
{
  error(RTMIDI_ERROR(gettext_noopt("Setting port names is not implemented for Mac OS X CoreMIDI."),
		     Error::WARNING));
}

void MidiOutCore :: openVirtualPort( const std::string &portName )
{
  CoreMidiData *data = static_cast<CoreMidiData *> (apiData_);

  if ( data->localEndpoint ) {
    error(RTMIDI_ERROR(gettext_noopt("A virtual output port already exists."),
		       Error::WARNING) );
    return;
  }

  // Create a virtual MIDI output source.
  MIDIEndpointRef endpoint;
  OSStatus result = MIDISourceCreate( data->client,
				      CFStringWrapper( portName ),
				      &endpoint );
  if ( result != noErr ) {
    error(RTMIDI_ERROR(gettext_noopt("Error creating OS-X virtual MIDI source."),
		       Error::DRIVER_ERROR) );
    return;
  }

  // Save our api-specific connection information.
  data->localEndpoint = endpoint;
}

void MidiOutCore :: openPort( const PortDescriptor & port,
			      const std::string &portName)
{
  CoreMidiData *data = static_cast<CoreMidiData *> (apiData_);
  const CorePortDescriptor * remote = dynamic_cast<const CorePortDescriptor *>(&port);

  if ( !data ) {
    error(RTMIDI_ERROR(gettext_noopt("Data has not been allocated."),
		       Error::SYSTEM_ERROR) );
    return;
  }
  if ( connected_ || data -> localEndpoint) {
    error(RTMIDI_ERROR(gettext_noopt("A valid connection already exists."),
		       Error::WARNING) );
    return;
  }
  if (!remote) {
    error(RTMIDI_ERROR(gettext_noopt("Core MIDI has been instructed to open a non-Core MIDI port. This doesn't work."),
		       Error::INVALID_DEVICE) );
    return;
  }

  try {
    data->openPort (portName,
		    PortDescriptor::OUTPUT);
    data->setRemote(*remote);
    connected_ = true;
  } catch (Error & e) {
    error(e);
  }
}

Pointer<PortDescriptor> MidiOutCore :: getDescriptor(bool local)
{
  CoreMidiData *data = static_cast<CoreMidiData *>
    (apiData_);
  if (!data) {
    return NULL;
  }
  try {
    if (local) {
      if (data && data->localEndpoint) {
	return Pointer<PortDescriptor>(
				       new CorePortDescriptor(data->localEndpoint, data->getClientName()));
      }
    } else {
      if (data->getEndpoint()) {
	return Pointer<PortDescriptor>(
				       new CorePortDescriptor(*data));
      }
    }
  } catch (Error & e) {
    error(e);
  }
  return NULL;
}

PortList MidiOutCore :: getPortList(int capabilities)
{
  CoreMidiData *data = static_cast<CoreMidiData *> (apiData_);
  try {
    return CorePortDescriptor::getPortList(capabilities | PortDescriptor::OUTPUT,
					   data->getClientName());
  } catch (Error & e) {
    error(e);
    return PortList();
  }
}


// Not necessary if we don't treat sysex messages any differently than
// normal messages ... see below.
//static void sysexCompletionProc( MIDISysexSendRequest *sreq )
//{
//  free( sreq );
//}

void MidiOutCore :: sendMessage( const unsigned char *message, size_t size )
{
  // We use the MIDISendSysex() function to asynchronously send sysex
  // messages.  Otherwise, we use a single CoreMidi MIDIPacket.
  if ( size == 0 ) {
    error(RTMIDI_ERROR(gettext_noopt("No data in message argument."),
		       Error::WARNING));
    return;
  }

  //  unsigned int packetBytes, bytesLeft = size;
  //  unsigned int messageIndex = 0;
  MIDITimeStamp timeStamp = AudioGetCurrentHostTime();
  CoreMidiData *data = static_cast<CoreMidiData *> (apiData_);
  OSStatus result;

  if ( message[0] != 0xF0 && size > 3 ) {
    error(RTMIDI_ERROR(gettext_noopt("message format problem ... not sysex but > 3 bytes?"),
		       Error::WARNING ));
    return;
  }

  Byte buffer[size+(sizeof(MIDIPacketList))];
  ByteCount listSize = sizeof(buffer);
  MIDIPacketList *packetList = (MIDIPacketList*)buffer;
  MIDIPacket *packet = MIDIPacketListInit( packetList );

  ByteCount remainingBytes = size;
  while (remainingBytes && packet) {
    ByteCount bytesForPacket = remainingBytes > 65535 ? 65535 : remainingBytes; // 65535 = maximum size of a MIDIPacket
    const Byte* dataStartPtr = (const Byte *) &message[size - remainingBytes];
    packet = MIDIPacketListAdd( packetList, listSize, packet, timeStamp, bytesForPacket, dataStartPtr);
    remainingBytes -= bytesForPacket;
  }

  if ( !packet ) {
    error(RTMIDI_ERROR(gettext_noopt("Could not allocate packet list."),
		       Error::DRIVER_ERROR) );
    return;
  }

  // Send to any destinations that may have connected to us.
  if ( data->localEndpoint ) {
    result = MIDIReceived( data->localEndpoint, packetList );
    if ( result != noErr ) {
      error(RTMIDI_ERROR(gettext_noopt("Error sending MIDI to virtual destinations."),
			 Error::WARNING) );
    }
  }

  // And send to an explicit destination port if we're connected.
  if ( connected_ ) {
    result = MIDISend( data->localPort, data->getEndpoint(), packetList );
    if ( result != noErr ) {
      error(RTMIDI_ERROR(gettext_noopt("Error sending MIDI message to port."),
			 Error::WARNING) );
    }
  }
}
#undef RTMIDI_CLASSNAME
RTMIDI_NAMESPACE_END
#endif  // __MACOSX_COREMIDI__


//*********************************************************************//
//  API: LINUX ALSA SEQUENCER
//*********************************************************************//

// API information found at:
//   - http://www.alsa-project.org/documentation.php#Library

#if defined(__LINUX_ALSA__)

// The ALSA Sequencer API is based on the use of a callback function for
// MIDI input.
//
// Thanks to Pedro Lopez-Cabanillas for help with the ALSA sequencer
// time stamps and other assorted fixes!!!

// If you don't need timestamping for incoming MIDI events, define the
// preprocessor definition AVOID_TIMESTAMPING to save resources
// associated with the ALSA sequencer queues.

#include <pthread.h>
#include <sys/time.h>

// ALSA header file.
#include <alsa/asoundlib.h>

RTMIDI_NAMESPACE_START
struct AlsaMidiData;

/*! An abstraction layer for the ALSA sequencer layer. It provides
  the following functionality:
  - dynamic allocation of the sequencer
  - optionallay avoid concurrent access to the ALSA sequencer,
  which is not thread proof. This feature is controlled by
  the parameter \ref locking.
*/

#define RTMIDI_CLASSNAME "AlsaSequencer"
template <int locking=1>
class AlsaSequencer {
public:
  AlsaSequencer():seq(0)
  {
    if (locking) {
      pthread_mutexattr_t attr;
      pthread_mutexattr_init(&attr);
      pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
      pthread_mutex_init(&mutex, &attr);
    }
  }

  AlsaSequencer(const std::string &n):seq(0),name(n)
  {
    if (locking) {
      pthread_mutexattr_t attr;
      pthread_mutexattr_init(&attr);
      pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
      pthread_mutex_init(&mutex, &attr);
    }
    init();
    {
      scoped_lock lock(mutex);
      snd_seq_set_client_name( seq, name.c_str() );
    }
  }

  ~AlsaSequencer()
  {
    if (seq) {
      scoped_lock lock(mutex);
      snd_seq_close(seq);
      seq = 0;
    }
    if (locking) {
      pthread_mutex_destroy(&mutex);
    }
  }

  int setName(const std::string &n) {
    /* we don't want to rename the client after opening it. */
    name = n;
    if (seq) {
      return snd_seq_set_client_name( seq, name.c_str() );
    }
    return 0;
  }

  std::string GetPortName(int client, int port, int flags) {
    init();
    snd_seq_client_info_t *cinfo;
    snd_seq_client_info_alloca( &cinfo );
    {
      scoped_lock lock (mutex);
      snd_seq_get_any_client_info(seq,client,cinfo);
    }

    snd_seq_port_info_t *pinfo;
    snd_seq_port_info_alloca( &pinfo );
    {
      scoped_lock lock (mutex);
      snd_seq_get_any_port_info(seq,client,port,pinfo);
    }

    int naming = flags & PortDescriptor::NAMING_MASK;

    std::ostringstream os;
    switch (naming) {
    case PortDescriptor::SESSION_PATH:
      if (flags & PortDescriptor::INCLUDE_API)
	os << "ALSA:";
      os << client << ":" << port;
      break;
    case PortDescriptor::STORAGE_PATH:
      if (flags & PortDescriptor::INCLUDE_API)
	os << "ALSA:";
      os << snd_seq_client_info_get_name(cinfo);
      os << ":";
      os << snd_seq_port_info_get_name(pinfo);
      if (flags & PortDescriptor::UNIQUE_PORT_NAME)
	os << ";" << client << ":" << port;
      break;
    case PortDescriptor::LONG_NAME:
      os << snd_seq_client_info_get_name( cinfo );
      if (flags & PortDescriptor::UNIQUE_PORT_NAME) {
	os << " " << client;
      }
      os << ":";
      if (flags & PortDescriptor::UNIQUE_PORT_NAME) {
	os << port;
      }

      os << " " << snd_seq_port_info_get_name(pinfo);
      if (flags & PortDescriptor::INCLUDE_API)
	os << " (ALSA)";
      break;
    case PortDescriptor::SHORT_NAME:
    default:
      os << snd_seq_client_info_get_name( cinfo );
      if (flags & PortDescriptor::UNIQUE_PORT_NAME) {
	os << " ";
	os << client;
      }
      os << ":" << port;
      if (flags & PortDescriptor::INCLUDE_API)
	os << " (ALSA)";

      break;
    }
    return os.str();
  }

  void setPortName(const std::string &name, int port) {
    snd_seq_port_info_t *pinfo = NULL;
    int error;
    snd_seq_port_info_alloca( &pinfo );
    if (pinfo == NULL) {
      throw RTMIDI_ERROR(gettext_noopt("Could not allocate ALSA port info structure."),
			Error::MEMORY_ERROR);
    }
    if ((error = snd_seq_get_port_info( seq, port, pinfo ))<0) {
      throw RTMIDI_ERROR1(gettext_noopt("Could not get ALSA port information: %s"),
			  Error::DRIVER_ERROR,
			  snd_strerror(error));      
    }
    snd_seq_port_info_set_name( pinfo, name.c_str() );
    if ((error = snd_seq_set_port_info( seq, port, pinfo ) )) {
      throw RTMIDI_ERROR1(gettext_noopt("Could not set ALSA port information: %s"),
			  Error::DRIVER_ERROR,
			  snd_strerror(error));      
    }
  }

  int getPortCapabilities(int client, int port) {
    init();
    snd_seq_port_info_t *pinfo;
    snd_seq_port_info_alloca( &pinfo );
    {
      scoped_lock lock (mutex);
      snd_seq_get_any_port_info(seq,client,port,pinfo);
    }
    unsigned int caps = snd_seq_port_info_get_capability(pinfo);
    int retval = (caps & (SND_SEQ_PORT_CAP_READ|SND_SEQ_PORT_CAP_SUBS_READ))?
      PortDescriptor::INPUT:0;
    if (caps & (SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE))
      retval |= PortDescriptor::OUTPUT;
    return retval;
  }

  int getNextClient(snd_seq_client_info_t * cinfo ) {
    init();
    scoped_lock lock (mutex);
    return snd_seq_query_next_client (seq, cinfo);
  }

  int getNextPort(snd_seq_port_info_t * pinfo ) {
    init();
    scoped_lock lock (mutex);
    return snd_seq_query_next_port (seq, pinfo);
  }

  int createPort (snd_seq_port_info_t *pinfo) {
    init();
    scoped_lock lock (mutex);
    return snd_seq_create_port(seq, pinfo);
  }

  void deletePort(int port) {
    init();
    scoped_lock lock (mutex);
    snd_seq_delete_port( seq, port );
  }

  snd_seq_port_subscribe_t * connectPorts(const snd_seq_addr_t & from,
					  const snd_seq_addr_t & to,
					  bool real_time) {
    init();
    snd_seq_port_subscribe_t *subscription;

    if (snd_seq_port_subscribe_malloc( &subscription ) < 0) {
      throw RTMIDI_ERROR(gettext_noopt("Could not allocate ALSA port subscription."),
			 Error::DRIVER_ERROR );
      return 0;
    }
    snd_seq_port_subscribe_set_sender(subscription, &from);
    snd_seq_port_subscribe_set_dest(subscription, &to);
    if (real_time) {
      snd_seq_port_subscribe_set_time_update(subscription, 1);
      snd_seq_port_subscribe_set_time_real(subscription, 1);
    }
    {
      scoped_lock lock (mutex);
      if ( snd_seq_subscribe_port(seq, subscription) ) {
	snd_seq_port_subscribe_free( subscription );
	subscription = 0;
	throw RTMIDI_ERROR(gettext_noopt("Error making ALSA port connection."),
			   Error::DRIVER_ERROR);
	return 0;
      }
    }
    return subscription;
  }

  void closePort(snd_seq_port_subscribe_t * subscription ) {
    init();
    scoped_lock lock(mutex);
    snd_seq_unsubscribe_port( seq, subscription );
  }

  void startQueue(int queue_id) {
    init();
    scoped_lock lock(mutex);
    snd_seq_start_queue( seq, queue_id, NULL );
    snd_seq_drain_output( seq );
  }

  /*! Use AlsaSequencer like a C pointer.
    \note This function breaks the design to control thread safety
    by the selection of the \ref locking parameter to the class.
    It should be removed as soon as possible in order ensure the
    thread policy that has been intended by creating this class.
  */
  operator snd_seq_t * ()
  {
    return seq;
  }
protected:
  struct scoped_lock {
    pthread_mutex_t * mutex;
    scoped_lock(pthread_mutex_t & m): mutex(&m)
    {
      if (locking)
	pthread_mutex_lock(mutex);
    }
    ~scoped_lock()
    {
      if (locking)
	pthread_mutex_unlock(mutex);
    }
  };
  pthread_mutex_t mutex;
  snd_seq_t * seq;
  std::string name;


  snd_seq_client_info_t * GetClient(int id) {
    init();
    snd_seq_client_info_t * cinfo;
    scoped_lock lock(mutex);
    snd_seq_get_any_client_info(seq,id,cinfo);
    return cinfo;
  }

  void init()
  {
    init (seq);
  }

  void init(snd_seq_t * &s)
  {
    if (s) return;
    {
      scoped_lock lock(mutex);
      int result = snd_seq_open(&s, "default", SND_SEQ_OPEN_DUPLEX, SND_SEQ_NONBLOCK);
      if ( result < 0 ) {
	switch (result) {
	case -ENOENT: // /dev/snd/seq does not exist
	  // Error numbers are defined to be positive
	case ENOENT: // just in case ...
	  throw RTMIDI_ERROR(snd_strerror(result),
			     Error::NO_DEVICES_FOUND);
	  return;
	default:
	  std::cerr << __FILE__ << ":" << __LINE__
		    << "Got unhandled error number " << result << std::endl;
	  throw RTMIDI_ERROR(snd_strerror(result),
			     Error::DRIVER_ERROR );
	  return;
	}
      }
      snd_seq_set_client_name( seq, name.c_str() );
    }
  }
};
#undef RTMIDI_CLASSNAME
typedef AlsaSequencer<1> LockingAlsaSequencer;
typedef AlsaSequencer<0> NonLockingAlsaSequencer;

#define RTMIDI_CLASSNAME "AlsaPortDescriptor"
struct AlsaPortDescriptor:public PortDescriptor,
			  public snd_seq_addr_t
{
  MidiApi * api;
  static LockingAlsaSequencer seq;
  AlsaPortDescriptor(const std::string &name):api(0),clientName(name)
  {
    client = 0;
    port   = 0;
  }
  AlsaPortDescriptor(int c, int p, const std::string &name):api(0),clientName(name)
  {
    client = c;
    port   = p;
    seq.setName(name);
  }
  AlsaPortDescriptor(snd_seq_addr_t & other,
		     const std::string &name):snd_seq_addr_t(other),
					       clientName(name) {
    seq.setName(name);
  }
  ~AlsaPortDescriptor() {}
  MidiInApi * getInputApi(unsigned int queueSizeLimit = 100) const {
    if (getCapabilities() & INPUT)
      return new MidiInAlsa(clientName,queueSizeLimit);
    else
      return 0;
  }
  MidiOutApi * getOutputApi() const {
    if (getCapabilities() & OUTPUT)
      return new MidiOutAlsa(clientName);
    else
      return 0;
  }
  std::string getName(int flags = SHORT_NAME | UNIQUE_PORT_NAME) {
    return seq.GetPortName(client,port,flags);
  }

  const std::string &getClientName() {
    return clientName;
  }

  int getCapabilities() const {
    if (!client) return 0;
    return seq.getPortCapabilities(client,port);
  }
  static PortList getPortList(int capabilities, const std::string &clientName);
protected:
  std::string clientName;
};

LockingAlsaSequencer AlsaPortDescriptor::seq;



PortList AlsaPortDescriptor :: getPortList(int capabilities, const std::string &clientName)
{
  PortList list;
  snd_seq_client_info_t *cinfo;
  snd_seq_port_info_t *pinfo;
  int client;
  snd_seq_client_info_alloca( &cinfo );
  snd_seq_port_info_alloca( &pinfo );

  snd_seq_client_info_set_client( cinfo, -1 );
  while ( seq.getNextClient(cinfo ) >= 0 ) {
    client = snd_seq_client_info_get_client( cinfo );
    // ignore default device (it is included in the following results again)
    if ( client == 0 ) continue;
    // Reset query info
    snd_seq_port_info_set_client( pinfo, client );
    snd_seq_port_info_set_port( pinfo, -1 );
    while ( seq.getNextPort( pinfo ) >= 0 ) {
      unsigned int atyp = snd_seq_port_info_get_type( pinfo );
      // otherwise we get ports without any
      if ( !(capabilities & UNLIMITED) &&
	   !( atyp & SND_SEQ_PORT_TYPE_MIDI_GENERIC )  &&
	   !( atyp & SND_SEQ_PORT_TYPE_SYNTH )
	   ) continue;
      unsigned int caps = snd_seq_port_info_get_capability( pinfo );
      if (capabilities & INPUT) {
	/* we need both READ and SUBS_READ */
	if ((caps & (SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ))
	    != (SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ))
	  continue;
      }
      if (capabilities & OUTPUT) {
	/* we need both WRITE and SUBS_WRITE */
	if ((caps & (SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE))
	    != (SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE))
	  continue;
      }
      list.push_back(Pointer<PortDescriptor>(
					     new AlsaPortDescriptor(client,snd_seq_port_info_get_port(pinfo),clientName)));
    }
  }
  return list;
}
#undef RTMIDI_CLASSNAME


/*! A structure to hold variables related to the ALSA API
  implementation.

  \note After all sequencer handling is covered by the \ref
  AlsaSequencer class, we should make seq to be a pointer in order
  to allow a common client implementation.
*/
#define RTMIDI_CLASSNAME "AlsaMidiData"

struct AlsaMidiData:public AlsaPortDescriptor {
  /*
    AlsaMidiData():seq()
    {
    init();
    }
  */
  AlsaMidiData(const std::string &clientName):AlsaPortDescriptor(clientName),
					      seq(clientName)
  {
    init();
  }
  ~AlsaMidiData()
  {
    if (local.client)
      deletePort();
  }
  void init () {
    local.port   = 0;
    local.client = 0;
    port = -1;
    subscription = 0;
    coder = 0;
    bufferSize = 32;
    buffer = 0;
    dummy_thread_id = pthread_self();
    thread = dummy_thread_id;
    queue_id = -1;
    trigger_fds[0] = -1;
    trigger_fds[1] = -1;
  }
  snd_seq_addr_t local; /*!< Our port and client id. If client = 0 (default) this means we didn't aquire a port so far. */
  NonLockingAlsaSequencer seq;
  //		unsigned int portNum;
  snd_seq_port_subscribe_t *subscription;
  snd_midi_event_t *coder;
  unsigned int bufferSize;
  unsigned char *buffer;
  pthread_t thread;
  pthread_t dummy_thread_id;
  snd_seq_real_time_t lastTime;
  int queue_id; // an input queue is needed to get timestamped events
  int trigger_fds[2];

  void setRemote(const AlsaPortDescriptor * remote) {
    port   = remote->port;
    client = remote->client;
  }
  void connectPorts(const snd_seq_addr_t &from,
		    const snd_seq_addr_t &to,
		    bool real_time) {
    subscription = seq.connectPorts(from, to, real_time);
  }

  int openPort(int alsaCapabilities,
	       const std::string &portName) {
    if (subscription) {
      api->error( RTMIDI_ERROR(gettext_noopt("Could not allocate ALSA port subscription."),
			       Error::DRIVER_ERROR ));
      return -99;
    }

    snd_seq_port_info_t *pinfo;
    snd_seq_port_info_alloca( &pinfo );

    snd_seq_port_info_set_client( pinfo, 0 );
    snd_seq_port_info_set_port( pinfo, 0 );
    snd_seq_port_info_set_capability( pinfo,
				      alsaCapabilities);
    snd_seq_port_info_set_type( pinfo,
				SND_SEQ_PORT_TYPE_MIDI_GENERIC |
				SND_SEQ_PORT_TYPE_APPLICATION );
    snd_seq_port_info_set_midi_channels(pinfo, 16);
#ifndef AVOID_TIMESTAMPING
    snd_seq_port_info_set_timestamping(pinfo, 1);
    snd_seq_port_info_set_timestamp_real(pinfo, 1);
    snd_seq_port_info_set_timestamp_queue(pinfo, queue_id);
#endif
    snd_seq_port_info_set_name(pinfo,  portName.c_str() );
    int createok = seq.createPort(pinfo);

    if ( createok < 0 ) {
      api->error(RTMIDI_ERROR("ALSA error while creating input port.",
			      Error::DRIVER_ERROR));
      return createok;
    }

    local.client = snd_seq_port_info_get_client( pinfo );
    local.port   = snd_seq_port_info_get_port(pinfo);
    return 0;
  }

  void deletePort() {
    seq.deletePort(local.port);
    local.client = 0;
    local.port   = 0;
  }

  void closePort() {
    seq.closePort(subscription );
    snd_seq_port_subscribe_free( subscription );
    subscription = 0;
  }

  void setName(const std::string &name) {
    seq.setPortName(name, local.port);
  }

  void setClientName(const std::string &name) {
    seq.setName(name);
  }

  bool startQueue(void * userdata) {
    // Start the input queue
#ifndef AVOID_TIMESTAMPING
    seq.startQueue(queue_id);
#endif
    // Start our MIDI input thread.
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setschedpolicy(&attr, SCHED_OTHER);

    int err = pthread_create(&thread, &attr, MidiInAlsa::alsaMidiHandler, userdata);
    pthread_attr_destroy(&attr);
    if ( err ) {
      closePort();
      api->error(RTMIDI_ERROR(gettext_noopt("Error starting MIDI input thread!"),
			      Error::THREAD_ERROR));
      return false;
    }
    return true;
  }
};
#undef RTMIDI_CLASSNAME

#define PORT_TYPE( pinfo, bits ) ((snd_seq_port_info_get_capability(pinfo) & (bits)) == (bits))

//*********************************************************************//
//  API: LINUX ALSA
//  Class Definitions: MidiInAlsa
//*********************************************************************//

#define RTMIDI_CLASSNAME "MidiInAlsa"
// static function:
void * MidiInAlsa::alsaMidiHandler( void *ptr ) throw()
{
  MidiInAlsa *data = static_cast<MidiInAlsa *> (ptr);
  AlsaMidiData *apiData = static_cast<AlsaMidiData *> (data->apiData_);

  long nBytes;
  double time;
  bool continueSysex = false;
  bool doDecode = false;
  MidiInApi::MidiMessage message;
  int poll_fd_count;
  struct pollfd *poll_fds;

  snd_seq_event_t *ev;
  int result;
  apiData->bufferSize = 32;
  result = snd_midi_event_new( 0, &apiData->coder );
  if ( result < 0 ) {
    data->doInput = false;
    try {
      data->error(RTMIDI_ERROR(rtmidi_gettext("Error initializing MIDI event parser."),
			       Error::WARNING));
    } catch (Error & e) {
      // don't bother ALSA with an unhandled exception
    }
    return 0;
  }
  unsigned char *buffer = (unsigned char *) malloc( apiData->bufferSize );
  if ( buffer == NULL ) {
    data->doInput = false;
    snd_midi_event_free( apiData->coder );
    apiData->coder = 0;
    try {
      data->error(RTMIDI_ERROR(rtmidi_gettext("Error initializing buffer memory."),
			       Error::WARNING));
    } catch (Error & e) {
      // don't bother ALSA with an unhandled exception
    }
    return 0;
  }
  snd_midi_event_init( apiData->coder );
  snd_midi_event_no_status( apiData->coder, 1 ); // suppress running status messages

  poll_fd_count = snd_seq_poll_descriptors_count( apiData->seq, POLLIN ) + 1;
  poll_fds = (struct pollfd*)alloca( poll_fd_count * sizeof( struct pollfd ));
  snd_seq_poll_descriptors( apiData->seq, poll_fds + 1, poll_fd_count - 1, POLLIN );
  poll_fds[0].fd = apiData->trigger_fds[0];
  poll_fds[0].events = POLLIN;

  while ( data->doInput ) {

    if ( snd_seq_event_input_pending( apiData->seq, 1 ) == 0 ) {
      // No data pending
      if ( poll( poll_fds, poll_fd_count, -1) >= 0 ) {
	if ( poll_fds[0].revents & POLLIN ) {
	  bool dummy;
	  int res = read( poll_fds[0].fd, &dummy, sizeof(dummy) );
	  (void) res;
	}
      }
      continue;
    }

    // If here, there should be data.
    result = snd_seq_event_input( apiData->seq, &ev );
    if ( result == -ENOSPC ) {
      try {
	data->error(RTMIDI_ERROR(rtmidi_gettext("MIDI input buffer overrun."),
				 Error::WARNING));
      } catch (Error & e) {
	// don't bother ALSA with an unhandled exception
      }

      continue;
    }
    else if ( result == -EAGAIN ) {
      try {
	data->error(RTMIDI_ERROR(rtmidi_gettext("ALSA returned without providing a MIDI event."),
				 Error::WARNING));
      } catch (Error & e) {
	// don't bother ALSA with an unhandled exception
      }

      continue;
    }
    else if ( result <= 0 ) {
      try {
	data->error(RTMIDI_ERROR1(rtmidi_gettext("Unknown MIDI input error.\nThe system reports:\n%s"),
				  Error::WARNING,
				  strerror(-result)));
      } catch (Error & e) {
	// don't bother ALSA with an unhandled exception
      }
      continue;
    }

    // This is a bit weird, but we now have to decode an ALSA MIDI
    // event (back) into MIDI bytes.  We'll ignore non-MIDI types.
    if ( !continueSysex ) message.bytes.clear();

    doDecode = false;
    switch ( ev->type ) {

    case SND_SEQ_EVENT_PORT_SUBSCRIBED:
#if defined(__RTMIDI_DEBUG__)
      std::cout << "MidiInAlsa::alsaMidiHandler: port connection made!\n";
#endif
      break;

    case SND_SEQ_EVENT_PORT_UNSUBSCRIBED:
#if defined(__RTMIDI_DEBUG__)
      std::cerr << "MidiInAlsa::alsaMidiHandler: port connection has closed!\n";
      std::cout << "sender = " << (int) ev->data.connect.sender.client << ":"
		<< (int) ev->data.connect.sender.port
		<< ", dest = " << (int) ev->data.connect.dest.client << ":"
		<< (int) ev->data.connect.dest.port
		<< std::endl;
#endif
      break;

    case SND_SEQ_EVENT_QFRAME: // MIDI time code
      if ( !( data->ignoreFlags & 0x02 ) ) doDecode = true;
      break;

    case SND_SEQ_EVENT_TICK: // 0xF9 ... MIDI timing tick
      if ( !( data->ignoreFlags & 0x02 ) ) doDecode = true;
      break;

    case SND_SEQ_EVENT_CLOCK: // 0xF8 ... MIDI timing (clock) tick
      if ( !( data->ignoreFlags & 0x02 ) ) doDecode = true;
      break;

    case SND_SEQ_EVENT_SENSING: // Active sensing
      if ( !( data->ignoreFlags & 0x04 ) ) doDecode = true;
      break;

    case SND_SEQ_EVENT_SYSEX:
      if ( (data->ignoreFlags & 0x01) ) break;
      if ( ev->data.ext.len > apiData->bufferSize ) {
	apiData->bufferSize = ev->data.ext.len;
	free( buffer );
	buffer = (unsigned char *) malloc( apiData->bufferSize );
	if ( buffer == NULL ) {
	  data->doInput = false;
	  try {
	    data->error(RTMIDI_ERROR(rtmidi_gettext("Error resizing buffer memory."),
				     Error::WARNING));
	  } catch (Error & e) {
	    // don't bother ALSA with an unhandled exception
	  }
	  break;
	}
      }
      RTMIDI_FALLTHROUGH;
    default:
      doDecode = true;
    }

    if ( doDecode ) {

      nBytes = snd_midi_event_decode( apiData->coder, buffer, apiData->bufferSize, ev );
      if ( nBytes > 0 ) {
	// The ALSA sequencer has a maximum buffer size for MIDI sysex
	// events of 256 bytes.  If a device sends sysex messages larger
	// than this, they are segmented into 256 byte chunks.  So,
	// we'll watch for this and concatenate sysex chunks into a
	// single sysex message if necessary.
	if ( !continueSysex )
	  message.bytes.assign( buffer, &buffer[nBytes] );
	else
	  message.bytes.insert( message.bytes.end(), buffer, &buffer[nBytes] );

	continueSysex = ( ( ev->type == SND_SEQ_EVENT_SYSEX ) && ( message.bytes.back() != 0xF7 ) );
	if ( !continueSysex ) {

	  // Calculate the time stamp:
	  message.timeStamp = 0.0;

	  // Method 1: Use the system time.
	  //(void)gettimeofday(&tv, (struct timezone *)NULL);
	  //time = (tv.tv_sec * 1000000) + tv.tv_usec;

	  // Method 2: Use the ALSA sequencer event time data.
	  // (thanks to Pedro Lopez-Cabanillas!).
	  // time = ( ev->time.time.tv_sec * 1000000 ) + ( ev->time.time.tv_nsec/1000 );
	  // lastTime = time;
	  // time -= apiData->lastTime;
	  // apiData->lastTime = lastTime;
	  // Using method from:
	  // https://www.gnu.org/software/libc/manual/html_node/Elapsed-Time.html

	  // Perform the carry for the later subtraction by updating y.
	  snd_seq_real_time_t &x(ev->time.time);
	  snd_seq_real_time_t &y(apiData->lastTime);
	  if (x.tv_nsec < y.tv_nsec) {
	    int nsec = (y.tv_nsec - x.tv_nsec) / 1000000000 + 1;
	    y.tv_nsec -= 1000000000 * nsec;
	    y.tv_sec += nsec;
	  }
	  if (x.tv_nsec - y.tv_nsec > 1000000000) {
	    int nsec = (x.tv_nsec - y.tv_nsec) / 1000000000;
	    y.tv_nsec += 1000000000 * nsec;
	    y.tv_sec -= nsec;
	  }

	  // Compute the time difference.
	  time = x.tv_sec - y.tv_sec + (x.tv_nsec - y.tv_nsec)*1e-9;

	  apiData->lastTime = ev->time.time;

	  if ( data->firstMessage == true )
	    data->firstMessage = false;
	  else
	    message.timeStamp = time * 0.000001;
	}
	else {
#if defined(__RTMIDI_DEBUG__)
	  try {
	    data->error(RTMIDI_ERROR(rtmidi_gettext("Event parsing error or not a MIDI event."),
				     Error::WARNING));
	  } catch (Error & e) {
	    // don't bother ALSA with an unhandled exception
	  }
#endif
	}
      }
    }

    snd_seq_free_event( ev );
    ev = 0;
    if ( message.bytes.size() == 0 || continueSysex ) continue;

    if ( data->userCallback ) {
      data->userCallback->rtmidi_midi_in( message.timeStamp, message.bytes);
    }
    else {
      // As long as we haven't reached our queue size limit, push the message.
      if (!data->queue.push(message)) {
	try {
	  data->error(RTMIDI_ERROR(rtmidi_gettext("Error: Message queue limit reached."),
				   Error::WARNING));
	} catch (Error & e) {
	  // don't bother ALSA with an unhandled exception
	}
      }
    }
  }

  if ( buffer ) free( buffer );
  snd_midi_event_free( apiData->coder );
  apiData->coder = 0;
  apiData->thread = apiData->dummy_thread_id;
  return 0;
}

MidiInAlsa :: MidiInAlsa( const std::string &clientName,
			  unsigned int queueSizeLimit ) : MidiInApi( queueSizeLimit )
{
  MidiInAlsa::initialize( clientName );
}

MidiInAlsa :: ~MidiInAlsa()
{
  // Close a connection if it exists.
  MidiInAlsa::closePort();

  // Shutdown the input thread.
  AlsaMidiData *data = static_cast<AlsaMidiData *> (apiData_);
  if ( doInput ) {
    doInput = false;
    int res = write( data->trigger_fds[1], &doInput, sizeof(doInput) );
    (void) res;
    if ( !pthread_equal(data->thread, data->dummy_thread_id) )
      pthread_join( data->thread, NULL );
  }

  // Cleanup.
  close ( data->trigger_fds[0] );
  close ( data->trigger_fds[1] );
  if ( data->local.client ) data->deletePort();
#ifndef AVOID_TIMESTAMPING
  snd_seq_free_queue( data->seq, data->queue_id );
  data->queue_id = -1;
#endif
  delete data;
}

void MidiInAlsa :: initialize( const std::string &clientName )
{

  // Save our api-specific connection information.
  AlsaMidiData *data = new AlsaMidiData (clientName);
  apiData_ = (void *) data;

  if ( pipe(data->trigger_fds) == -1 ) {
    error(RTMIDI_ERROR(gettext_noopt("Error creating pipe objects."),
		       Error::DRIVER_ERROR) );
    return;
  }

  // Create the input queue
#ifndef AVOID_TIMESTAMPING
  data->queue_id = snd_seq_alloc_named_queue(data->seq, "Midi Queue");
  // Set arbitrary tempo (mm=100) and resolution (240)
  snd_seq_queue_tempo_t *qtempo;
  snd_seq_queue_tempo_alloca(&qtempo);
  snd_seq_queue_tempo_set_tempo(qtempo, 600000);
  snd_seq_queue_tempo_set_ppq(qtempo, 240);
  snd_seq_set_queue_tempo(data->seq, data->queue_id, qtempo);
  snd_seq_drain_output(data->seq);
#endif
}

// This function is used to count or get the pinfo structure for a given port number.
unsigned int portInfo( snd_seq_t *seq, snd_seq_port_info_t *pinfo, unsigned int type, int portNumber )
{
  snd_seq_client_info_t *cinfo;
  int client;
  int count = 0;
  snd_seq_client_info_alloca( &cinfo );

  snd_seq_client_info_set_client( cinfo, -1 );
  while ( snd_seq_query_next_client( seq, cinfo ) >= 0 ) {
    client = snd_seq_client_info_get_client( cinfo );
    if ( client == 0 ) continue;
    // Reset query info
    snd_seq_port_info_set_client( pinfo, client );
    snd_seq_port_info_set_port( pinfo, -1 );
    while ( snd_seq_query_next_port( seq, pinfo ) >= 0 ) {
      unsigned int atyp = snd_seq_port_info_get_type( pinfo );
      if ( ( ( atyp & SND_SEQ_PORT_TYPE_MIDI_GENERIC ) == 0 ) &&
	   ( ( atyp & SND_SEQ_PORT_TYPE_SYNTH ) == 0 ) ) continue;
      unsigned int caps = snd_seq_port_info_get_capability( pinfo );
      if ( ( caps & type ) != type ) continue;
      if ( count == portNumber ) return 1;
      ++count;
    }
  }

  // If a negative portNumber was used, return the port count.
  if ( portNumber < 0 ) return count;
  return 0;
}

unsigned int MidiInAlsa :: getPortCount()
{
  snd_seq_port_info_t *pinfo;
  snd_seq_port_info_alloca( &pinfo );

  AlsaMidiData *data = static_cast<AlsaMidiData *> (apiData_);
  return portInfo( data->seq, pinfo, SND_SEQ_PORT_CAP_READ|SND_SEQ_PORT_CAP_SUBS_READ, -1 );
}

std::string MidiInAlsa :: getPortName( unsigned int portNumber )
{
  snd_seq_client_info_t *cinfo;
  snd_seq_port_info_t *pinfo;
  snd_seq_client_info_alloca( &cinfo );
  snd_seq_port_info_alloca( &pinfo );

  std::string stringName;
  AlsaMidiData *data = static_cast<AlsaMidiData *> (apiData_);
  if ( portInfo( data->seq, pinfo, SND_SEQ_PORT_CAP_READ|SND_SEQ_PORT_CAP_SUBS_READ, (int) portNumber ) ) {
    int cnum = snd_seq_port_info_get_client( pinfo );
    snd_seq_get_any_client_info( data->seq, cnum, cinfo );
    std::ostringstream os;
    os << snd_seq_client_info_get_name( cinfo );
    os << ":";
    os << snd_seq_port_info_get_name( pinfo );
    os << " ";                                    // These lines added to make sure devices are listed
    os << snd_seq_port_info_get_client( pinfo );  // with full portnames added to ensure individual device names
    os << ":";
    os << snd_seq_port_info_get_port( pinfo );
    stringName = os.str();
    return stringName;
  }

  // If we get here, we didn't find a match.
  error( RTMIDI_ERROR(gettext_noopt("Error looking for port name."),
		      Error::WARNING) );
  return stringName;
}

void MidiInAlsa :: openPort( unsigned int portNumber, const std::string &portName )
{
  if ( connected_ ) {
    error(RTMIDI_ERROR(gettext_noopt("A valid connection already exists."),
		       Error::WARNING) );
    return;
  }

  unsigned int nSrc = this->getPortCount();
  if ( nSrc < 1 ) {
    error( RTMIDI_ERROR(gettext_noopt("No MIDI input sources found."),
			Error::NO_DEVICES_FOUND ));
    return;
  }

  snd_seq_port_info_t *src_pinfo;
  snd_seq_port_info_alloca( &src_pinfo );
  AlsaMidiData *data = static_cast<AlsaMidiData *> (apiData_);
  if ( portInfo( data->seq, src_pinfo, SND_SEQ_PORT_CAP_READ|SND_SEQ_PORT_CAP_SUBS_READ, (int) portNumber ) == 0 ) {
    std::ostringstream ost;
    error( RTMIDI_ERROR1(gettext_noopt("The 'portNumber' argument (%d) is invalid."),
			 Error::INVALID_PARAMETER,
			 portNumber) );
    return;
  }

  snd_seq_addr_t sender, receiver;
  sender.client = snd_seq_port_info_get_client( src_pinfo );
  sender.port = snd_seq_port_info_get_port( src_pinfo );

  snd_seq_port_info_t *pinfo;
  snd_seq_port_info_alloca( &pinfo );
  if ( !data->local.client ) {
    snd_seq_port_info_set_client( pinfo, 0 );
    snd_seq_port_info_set_port( pinfo, 0 );
    snd_seq_port_info_set_capability( pinfo,
				      SND_SEQ_PORT_CAP_WRITE |
				      SND_SEQ_PORT_CAP_SUBS_WRITE );
    snd_seq_port_info_set_type( pinfo,
				SND_SEQ_PORT_TYPE_MIDI_GENERIC |
				SND_SEQ_PORT_TYPE_APPLICATION );
    snd_seq_port_info_set_midi_channels(pinfo, 16);
#ifndef AVOID_TIMESTAMPING
    snd_seq_port_info_set_timestamping(pinfo, 1);
    snd_seq_port_info_set_timestamp_real(pinfo, 1);
    snd_seq_port_info_set_timestamp_queue(pinfo, data->queue_id);
#endif
    snd_seq_port_info_set_name(pinfo,  portName.c_str() );
    int createok = snd_seq_create_port(data->seq, pinfo);

    if ( createok < 0 ) {
      error( RTMIDI_ERROR(gettext_noopt("Error creating ALSA input port."),
			  Error::DRIVER_ERROR));
      return;
    }
    data->local.port   = snd_seq_port_info_get_port(pinfo);
    data->local.client = snd_seq_port_info_get_client(pinfo);
  }

  receiver = data->local;

  if ( !data->subscription ) {
    // Make subscription
    if (snd_seq_port_subscribe_malloc( &data->subscription ) < 0) {
      error( RTMIDI_ERROR(gettext_noopt("Could not allocate ALSA port subscription."),
			  Error::DRIVER_ERROR) );
      return;
    }
    snd_seq_port_subscribe_set_sender(data->subscription, &sender);
    snd_seq_port_subscribe_set_dest(data->subscription, &receiver);
    if ( snd_seq_subscribe_port(data->seq, data->subscription) ) {
      snd_seq_port_subscribe_free( data->subscription );
      data->subscription = 0;
      error( RTMIDI_ERROR(gettext_noopt("Error making ALSA port connection."),
			  Error::DRIVER_ERROR) );
      return;
    }
  }

  if ( doInput == false ) {
    // Start the input queue
#ifndef AVOID_TIMESTAMPING
    snd_seq_start_queue( data->seq, data->queue_id, NULL );
    snd_seq_drain_output( data->seq );
#endif
    // Start our MIDI input thread.
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setschedpolicy(&attr, SCHED_OTHER);

    doInput = true;
    int err = pthread_create(&data->thread, &attr, alsaMidiHandler, this);
    pthread_attr_destroy(&attr);
    if ( err ) {
      snd_seq_unsubscribe_port( data->seq, data->subscription );
      snd_seq_port_subscribe_free( data->subscription );
      data->subscription = 0;
      doInput = false;
      error( RTMIDI_ERROR(gettext_noopt("Error starting MIDI input thread!"),
			  Error::THREAD_ERROR) );
      return;
    }
  }

  connected_ = true;
}

void MidiInAlsa :: openPort( const PortDescriptor & port,
			     const std::string &portName)
{
  AlsaMidiData *data = static_cast<AlsaMidiData *> (apiData_);
  const AlsaPortDescriptor * remote = dynamic_cast<const AlsaPortDescriptor *>(&port);

  if ( !data ) {
    error( RTMIDI_ERROR(gettext_noopt("Data has not been allocated."),
			Error::SYSTEM_ERROR) );
    return;
  }
  if ( connected_ ) {
    error( RTMIDI_ERROR(gettext_noopt("A valid connection already exists."),
			Error::WARNING) );
    return;
  }
  if (data->subscription) {
    error( RTMIDI_ERROR(gettext_noopt("Could not allocate ALSA port subscription."),
			Error::DRIVER_ERROR));
    return;
  }
  if (!remote) {
    error( RTMIDI_ERROR(gettext_noopt("ALSA has been instructed to open a non-ALSA MIDI port. This doesn't work."),
			Error::INVALID_DEVICE) );
    return;
  }

  try {
    if (!data->local.client)
      data->openPort (SND_SEQ_PORT_CAP_WRITE
		      | SND_SEQ_PORT_CAP_SUBS_WRITE,
		      portName);
    data->setRemote(remote);
    data->connectPorts(*remote,
		       data->local,
		       false);


    if ( doInput == false ) {
      doInput
	= data->startQueue(this);
    }

    connected_ = true;
  } catch (Error & e) {
    error(e);
  }
}

Pointer<PortDescriptor> MidiInAlsa :: getDescriptor(bool local)
{
  AlsaMidiData *data = static_cast<AlsaMidiData *> (apiData_);
  try {
    if (local) {
      if (data && data->local.client) {
	return Pointer<PortDescriptor>(
				       new AlsaPortDescriptor(data->local,data->getClientName()));
      }
    } else {
      if (data && data->client) {
	return Pointer<PortDescriptor>(
				       new AlsaPortDescriptor(*data,data->getClientName()));
      }
    }
  } catch (Error & e) {
    error (e);
  }
  return NULL;
}
PortList MidiInAlsa :: getPortList(int capabilities)
{
  AlsaMidiData *data = static_cast<AlsaMidiData *> (apiData_);
  try {
    return AlsaPortDescriptor::getPortList(capabilities | PortDescriptor::INPUT,
					   data->getClientName());
  } catch (Error & e) {
    error (e);
    return PortList();
  }
}



void MidiInAlsa :: openVirtualPort(const std::string &portName )
{
  AlsaMidiData *data = static_cast<AlsaMidiData *> (apiData_);
  if ( !data->local.client ) {
    snd_seq_port_info_t *pinfo;
    snd_seq_port_info_alloca( &pinfo );
    snd_seq_port_info_set_capability( pinfo,
				      SND_SEQ_PORT_CAP_WRITE |
				      SND_SEQ_PORT_CAP_SUBS_WRITE );
    snd_seq_port_info_set_type( pinfo,
				SND_SEQ_PORT_TYPE_MIDI_GENERIC |
				SND_SEQ_PORT_TYPE_APPLICATION );
    snd_seq_port_info_set_midi_channels(pinfo, 16);
#ifndef AVOID_TIMESTAMPING
    snd_seq_port_info_set_timestamping(pinfo, 1);
    snd_seq_port_info_set_timestamp_real(pinfo, 1);
    snd_seq_port_info_set_timestamp_queue(pinfo, data->queue_id);
#endif
    snd_seq_port_info_set_name(pinfo, portName.c_str());
    int createok = snd_seq_create_port(data->seq, pinfo);

    if ( createok < 0 ) {
      errorString_ = "MidiInAlsa::openVirtualPort: ";
      error( RTMIDI_ERROR(gettext_noopt("Error creating ALSA virtual port."),
			  Error::DRIVER_ERROR) );
      return;
    }
    data->local.port   = snd_seq_port_info_get_port(pinfo);
    data->local.client = snd_seq_port_info_get_client(pinfo);
  }

  if ( doInput == false ) {
    // Wait for old thread to stop, if still running
    if ( !pthread_equal(data->thread, data->dummy_thread_id) )
      pthread_join( data->thread, NULL );

    // Start the input queue
#ifndef AVOID_TIMESTAMPING
    snd_seq_start_queue( data->seq, data->queue_id, NULL );
    snd_seq_drain_output( data->seq );
#endif
    // Start our MIDI input thread.
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setschedpolicy(&attr, SCHED_OTHER);

    doInput = true;
    int err = pthread_create(&data->thread, &attr, alsaMidiHandler, this);
    pthread_attr_destroy(&attr);
    if ( err ) {
      if ( data->subscription ) {
	snd_seq_unsubscribe_port( data->seq, data->subscription );
	snd_seq_port_subscribe_free( data->subscription );
	data->subscription = 0;
      }
      doInput = false;
      error( RTMIDI_ERROR(gettext_noopt("Error starting MIDI input thread!"),
			  Error::THREAD_ERROR) );
      return;
    }
  }
}

void MidiInAlsa :: closePort( void )
{
  AlsaMidiData *data = static_cast<AlsaMidiData *> (apiData_);

  if ( connected_ ) {
    if ( data->subscription ) {
      snd_seq_unsubscribe_port( data->seq, data->subscription );
      snd_seq_port_subscribe_free( data->subscription );
      data->subscription = 0;
    }
    // Stop the input queue
#ifndef AVOID_TIMESTAMPING
    snd_seq_stop_queue( data->seq, data->queue_id, NULL );
    snd_seq_drain_output( data->seq );
#endif
    connected_ = false;
  }

  // Stop thread to avoid triggering the callback, while the port is intended to be closed
  if ( doInput ) {
    doInput = false;
    int res = write( data->trigger_fds[1], &doInput, sizeof(doInput) );
    (void) res;
    if ( !pthread_equal(data->thread, data->dummy_thread_id) )
      pthread_join( data->thread, NULL );
  }
}

void MidiInAlsa :: setClientName( const std::string &clientName )
{
    AlsaMidiData *data = static_cast<AlsaMidiData *> ( apiData_ );
    data->setClientName(clientName);
}

void MidiInAlsa :: setPortName( const std::string &portName )
{
  AlsaMidiData *data = static_cast<AlsaMidiData *> (apiData_);
  data->setName(portName);
}

#undef RTMIDI_CLASSNAME

//*********************************************************************//
//  API: LINUX ALSA
//  Class Definitions: MidiOutAlsa
//*********************************************************************//

#define RTMIDI_CLASSNAME "MidiOutAlsa"
MidiOutAlsa :: MidiOutAlsa( const std::string &clientName ) : MidiOutApi()
{
  MidiOutAlsa::initialize( clientName );
}

MidiOutAlsa :: ~MidiOutAlsa()
{
  // Close a connection if it exists.
  MidiOutAlsa::closePort();

  // Cleanup.
  AlsaMidiData *data = static_cast<AlsaMidiData *> (apiData_);
  if ( data->local.client > 0 ) {
    snd_seq_delete_port( data->seq, data->local.port );
    data->local.port = -1;
  }
  if ( data->coder ) {
    snd_midi_event_free( data->coder );
    data->coder = 0;
  }
  if ( data->buffer ) {
    free( data->buffer );
    data->buffer = 0;
  }
  delete data;
}

void MidiOutAlsa :: initialize( const std::string &clientName )
{
#if 0
  // Set up the ALSA sequencer client.
  snd_seq_t *seq;
  int result1 = snd_seq_open( &seq, "default", SND_SEQ_OPEN_OUTPUT, SND_SEQ_NONBLOCK );
  if ( result1 < 0 ) {
    error(RTMIDI_ERROR(gettext_noopt("Error creating ALSA sequencer client object."),
		       Error::DRIVER_ERROR, errorString_ ));
    return;
  }

  // Set client name.
  snd_seq_set_client_name( seq, clientName.c_str() );
#endif

  // Save our api-specific connection information.
  AlsaMidiData *data = new AlsaMidiData(clientName);
  // data->seq = seq;
  //	data->portNum = -1;

  int result = snd_midi_event_new( data->bufferSize, &data->coder );
  if ( result < 0 ) {
    delete data;
    error( RTMIDI_ERROR(gettext_noopt("Error initializing MIDI event parser."),
			Error::DRIVER_ERROR) );
    return;
  }
  data->buffer = (unsigned char *) malloc( data->bufferSize );
  if ( data->buffer == NULL ) {
    delete data;
    error( RTMIDI_ERROR(gettext_noopt("Error while allocating buffer memory."),
			Error::MEMORY_ERROR) );
    return;
  }
  snd_midi_event_init( data->coder );
  apiData_ = (void *) data;
}

unsigned int MidiOutAlsa :: getPortCount()
{
  snd_seq_port_info_t *pinfo;
  snd_seq_port_info_alloca( &pinfo );

  AlsaMidiData *data = static_cast<AlsaMidiData *> (apiData_);
  return portInfo( data->seq, pinfo, SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE, -1 );
}

std::string MidiOutAlsa :: getPortName( unsigned int portNumber )
{
  snd_seq_client_info_t *cinfo;
  snd_seq_port_info_t *pinfo;
  snd_seq_client_info_alloca( &cinfo );
  snd_seq_port_info_alloca( &pinfo );

  std::string stringName;
  AlsaMidiData *data = static_cast<AlsaMidiData *> (apiData_);
  if ( portInfo( data->seq, pinfo, SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE, (int) portNumber ) ) {
    int cnum = snd_seq_port_info_get_client(pinfo);
    snd_seq_get_any_client_info( data->seq, cnum, cinfo );
    std::ostringstream os;
    os << snd_seq_client_info_get_name(cinfo);
    os << ":";
    os << snd_seq_port_info_get_name( pinfo );
    os << " ";                                    // These lines added to make sure devices are listed
    os << snd_seq_port_info_get_client( pinfo );  // with full portnames added to ensure individual device names
    os << ":";
    os << snd_seq_port_info_get_port(pinfo);
    stringName = os.str();
    return stringName;
  }

  // If we get here, we didn't find a match.
  errorString_ = "MidiOutAlsa::getPortName: ";
  error( RTMIDI_ERROR(gettext_noopt("Error looking for port name."),
		      Error::WARNING) );
  return stringName;
}

void MidiOutAlsa :: openPort( unsigned int portNumber, const std::string &portName )
{
  if ( connected_ ) {
    error(RTMIDI_ERROR(gettext_noopt("A valid connection already exists."),
		       Error::WARNING) );
    return;
  }

  unsigned int nSrc = this->getPortCount();
  if (nSrc < 1) {
    errorString_ = "MidiOutAlsa::openPort: !";
    error(RTMIDI_ERROR(gettext_noopt("No MIDI output sinks found."),
		       Error::NO_DEVICES_FOUND) );
    return;
  }

  snd_seq_port_info_t *pinfo;
  snd_seq_port_info_alloca( &pinfo );
  AlsaMidiData *data = static_cast<AlsaMidiData *> (apiData_);
  if ( portInfo( data->seq, pinfo, SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE, (int) portNumber ) == 0 ) {
    std::ostringstream ost;
    ost << "MidiOutAlsa::openPort: ";
    errorString_ = ost.str();
    error(RTMIDI_ERROR1(gettext_noopt("The 'portNumber' argument (%d) is invalid."),
			Error::INVALID_PARAMETER, portNumber)  );
    return;
  }

  data->client = snd_seq_port_info_get_client( pinfo );
  data->port = snd_seq_port_info_get_port( pinfo );
  data->local.client = snd_seq_client_id( data->seq );

  if ( !data->local.client ) {
    int port = snd_seq_create_simple_port( data->seq, portName.c_str(),
					   SND_SEQ_PORT_CAP_READ|SND_SEQ_PORT_CAP_SUBS_READ,
					   SND_SEQ_PORT_TYPE_MIDI_GENERIC|SND_SEQ_PORT_TYPE_APPLICATION );
    if ( port < 0 ) {
      errorString_ = "MidiOutAlsa::openPort: ";
      error(RTMIDI_ERROR(gettext_noopt("Error creating ALSA output port."),
			 Error::DRIVER_ERROR));
      return;
    }

    data->local.port   = port;
  }

  // Make subscription
  if (snd_seq_port_subscribe_malloc( &data->subscription ) < 0) {
    snd_seq_port_subscribe_free( data->subscription );
    data->subscription = 0;
    error(RTMIDI_ERROR(gettext_noopt("Could not allocate ALSA port subscription."),
		       Error::DRIVER_ERROR) );
    return;
  }
  snd_seq_port_subscribe_set_sender(data->subscription, data);
  snd_seq_port_subscribe_set_dest(data->subscription, &data->local);
  snd_seq_port_subscribe_set_time_update(data->subscription, 1);
  snd_seq_port_subscribe_set_time_real(data->subscription, 1);
  if ( snd_seq_subscribe_port(data->seq, data->subscription) ) {
    snd_seq_port_subscribe_free( data->subscription );
    data->subscription = 0;
    error(RTMIDI_ERROR(gettext_noopt("Error making ALSA port connection."),
		       Error::DRIVER_ERROR) );
    return;
  }

  connected_ = true;
}

void MidiOutAlsa :: closePort( void )
{
  if ( connected_ ) {
    AlsaMidiData *data = static_cast<AlsaMidiData *> (apiData_);
    snd_seq_unsubscribe_port( data->seq, data->subscription );
    snd_seq_port_subscribe_free( data->subscription );
    data->subscription = 0;
    connected_ = false;
  }
}

void MidiOutAlsa :: setClientName( const std::string &clientName )
{
    AlsaMidiData *data = static_cast<AlsaMidiData *> ( apiData_ );
    data->setClientName(clientName);
}

void MidiOutAlsa :: setPortName( const std::string &portName )
{
  AlsaMidiData *data = static_cast<AlsaMidiData *> (apiData_);
  data->setName(portName);
}

void MidiOutAlsa :: openVirtualPort(const std::string &portName )
{
  AlsaMidiData *data = static_cast<AlsaMidiData *> (apiData_);
  if ( !data->local.client ) {
    int port = snd_seq_create_simple_port( data->seq, portName.c_str(),
					   SND_SEQ_PORT_CAP_READ|SND_SEQ_PORT_CAP_SUBS_READ,
					   SND_SEQ_PORT_TYPE_MIDI_GENERIC|SND_SEQ_PORT_TYPE_APPLICATION );

    if ( port < 0 ) {
      error(RTMIDI_ERROR(gettext_noopt("Error creating ALSA virtual port."),
			 Error::DRIVER_ERROR) );
    }
    data->local.port   = port;
    data->local.client = snd_seq_client_id(data->seq);
  }
}

void MidiOutAlsa :: sendMessage( const unsigned char *message, size_t size )
{
  int result;
  AlsaMidiData *data = static_cast<AlsaMidiData *> (apiData_);
  if ( size > data->bufferSize ) {
    data->bufferSize = size;
    result = snd_midi_event_resize_buffer ( data->coder, size);
    if ( result != 0 ) {
      error(RTMIDI_ERROR(gettext_noopt("ALSA error resizing MIDI event buffer."),
			 Error::DRIVER_ERROR) );
      return;
    }
    free (data->buffer);
    data->buffer = (unsigned char *) malloc( data->bufferSize );
    if ( data->buffer == NULL ) {
      error(RTMIDI_ERROR(gettext_noopt("Error while allocating buffer memory."),
			 Error::MEMORY_ERROR) );
      return;
    }
  }

  snd_seq_event_t ev;
  snd_seq_ev_clear(&ev);
  snd_seq_ev_set_source(&ev, data->local.port);
  snd_seq_ev_set_subs(&ev);
  snd_seq_ev_set_direct(&ev);
  for ( unsigned int i=0; i<size; ++i ) data->buffer[i] = message[i];
  result = snd_midi_event_encode( data->coder, data->buffer, (long)size, &ev );
  if ( result < (int)size ) {
    error(RTMIDI_ERROR(gettext_noopt("Event parsing error."),
		       Error::WARNING) );
    return;
  }

  // Send the event.
  result = snd_seq_event_output(data->seq, &ev);
  if ( result < 0 ) {
    error(RTMIDI_ERROR(gettext_noopt("Error sending MIDI message to port."),
		       Error::WARNING) );
    return;
  }
  snd_seq_drain_output(data->seq);
}

void MidiOutAlsa :: openPort( const PortDescriptor & port,
			      const std::string &portName)
{
  AlsaMidiData *data = static_cast<AlsaMidiData *> (apiData_);
  const AlsaPortDescriptor * remote = dynamic_cast<const AlsaPortDescriptor *>(&port);

  if ( !data ) {
    error(RTMIDI_ERROR(gettext_noopt("Data has not been allocated."),
		       Error::SYSTEM_ERROR) );
    return;
  }
  if ( connected_ ) {
    error(RTMIDI_ERROR(gettext_noopt("A valid connection already exists."),
		       Error::WARNING) );
    return;
  }
  if (data->subscription) {
    error(RTMIDI_ERROR(gettext_noopt("Error allocating ALSA port subscription."),
		       Error::DRIVER_ERROR) );
    return;
  }
  if (!remote) {
    error(RTMIDI_ERROR(gettext_noopt("ALSA has been instructed to open a non-ALSA MIDI port. This doesn't work."),
		       Error::INVALID_DEVICE) );
    return;
  }

  try {
    if (!data->local.client)
      data->openPort (SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ,
		      portName);
    data->setRemote(remote);
    data->connectPorts(data->local,*remote,true);

    connected_ = true;
  } catch (Error & e) {
    error (e);
  }
}
Pointer<PortDescriptor> MidiOutAlsa :: getDescriptor(bool local)
{
  AlsaMidiData *data = static_cast<AlsaMidiData *> (apiData_);
  try {
    if (local) {
      if (data && data->local.client) {
	return Pointer<PortDescriptor>(
				       new AlsaPortDescriptor(data->local, data->getClientName()));
      }
    } else {
      if (data && data->client) {
	return Pointer<PortDescriptor>(
				       new AlsaPortDescriptor(*data, data->getClientName()));
      }
    }
  } catch (Error & e) {
    error(e);
  }
  return NULL;
}
PortList MidiOutAlsa :: getPortList(int capabilities)
{
  AlsaMidiData *data = static_cast<AlsaMidiData *> (apiData_);
  try {
    return AlsaPortDescriptor::getPortList(capabilities | PortDescriptor::OUTPUT,
					   data->getClientName());
  } catch (Error & e) {
    return PortList();
  }
}
RTMIDI_NAMESPACE_END
#undef RTMIDI_CLASSNAME
#endif // __LINUX_ALSA__


//*********************************************************************//
//  API: Windows Multimedia Library (MM)
//*********************************************************************//

// API information deciphered from:
//  - http://msdn.microsoft.com/library/default.asp?url=/library/en-us/multimed/htm/_win32_midi_reference.asp

// Thanks to Jean-Baptiste Berruchon for the sysex code.

#if defined(__WINDOWS_MM__)

// The Windows MM API is based on the use of a callback function for
// MIDI input.  We convert the system specific time stamps to delta
// time values.

// Windows MM MIDI header files.
#include <windows.h>
#include <mmsystem.h>

// Convert a null-terminated wide string or ANSI-encoded string to UTF-8.
static std::string ConvertToUTF8(const TCHAR *str)
{
  std::string u8str;
  const WCHAR *wstr = L"";
#if defined( UNICODE ) || defined( _UNICODE )
  wstr = str;
#else
  // Convert from ANSI encoding to wide string
  int wlength = MultiByteToWideChar( CP_ACP, 0, str, -1, NULL, 0 );
  std::wstring wstrtemp;
  if ( wlength )
  {
    wstrtemp.assign( wlength - 1, 0 );
    MultiByteToWideChar( CP_ACP, 0, str, -1, &wstrtemp[0], wlength );
    wstr = &wstrtemp[0];
  }
#endif
  // Convert from wide string to UTF-8
  int length = WideCharToMultiByte( CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL );
  if ( length )
  {
    u8str.assign( length - 1, 0 );
    length = WideCharToMultiByte( CP_UTF8, 0, wstr, -1, &u8str[0], length, NULL, NULL );
  }
  return u8str;
}

#define  RT_SYSEX_BUFFER_SIZE 1024
#define  RT_SYSEX_BUFFER_COUNT 4

/* some header defines UNIQUE_PORT_NAME as a macro */
#ifdef UNIQUE_PORT_NAME
#undef UNIQUE_PORT_NAME
#endif
RTMIDI_NAMESPACE_START
/*! An abstraction layer for the ALSA sequencer layer. It provides
  the following functionality:
  - dynamic allocation of the sequencer
  - optionallay avoid concurrent access to the ALSA sequencer,
  which is not thread proof. This feature is controlled by
  the parameter \ref locking.
*/

#define RTMIDI_CLASSNAME "WinMMSequencer"
template <int locking=1>
class WinMMSequencer {
public:
  WinMMSequencer():mutex(0),name()
  {
    if (locking) {
#if 0
      // use mthreads instead
      pthread_mutexattr_t attr;
      pthread_mutexattr_init(&attr);
      pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
      pthread_mutex_init(&mutex, &attr);
#endif
    }
  }

  WinMMSequencer(const std::string &n):name(n)
  {
    if (locking) {
#if 0
      // use mthreads instead
      pthread_mutexattr_t attr;
      pthread_mutexattr_init(&attr);
      pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
      pthread_mutex_init(&mutex, &attr);
#endif
    }
    init();
    {
      scoped_lock lock(mutex);
    }
  }

  ~WinMMSequencer()
  {
    if (locking) {
#if 0
      // use mthreads instead
      pthread_mutex_destroy(&mutex);
#endif
    }
  }

  bool setName(const std::string &n) {
    /* we don't want to rename the client after opening it. */
    name = n;
    return true;
  }

  std::string getPortName(int port, bool is_input, int flags) {
    init();
    int naming = flags & PortDescriptor::NAMING_MASK;
    std::string name;

    unsigned int nDevices = is_input?midiInGetNumDevs()
      : midiOutGetNumDevs();
    if ( port < 0 || (unsigned int)port >= nDevices ) {
      throw Error(RTMIDI_ERROR1(gettext_noopt("The port argument %d is invalid."),
				Error::INVALID_PARAMETER,port));
    }

    if (is_input) {
      MIDIINCAPS deviceCaps;
      midiInGetDevCaps( port, &deviceCaps, sizeof(MIDIINCAPS));

#if defined( UNICODE ) || defined( _UNICODE )
      int length = WideCharToMultiByte(CP_UTF8, 0, deviceCaps.szPname, -1, NULL, 0, NULL, NULL) - 1;
      name.assign( length, 0 );
      length = WideCharToMultiByte(CP_UTF8,
				   0,
				   deviceCaps.szPname,
				   static_cast<int>(wcslen(deviceCaps.szPname)),
				   &name[0],
				   length,
				   NULL,
				   NULL);
#else
      name = deviceCaps.szPname;
#endif
    } else {
      MIDIOUTCAPS deviceCaps;
      midiOutGetDevCaps( port, &deviceCaps, sizeof(MIDIOUTCAPS));

#if defined( UNICODE ) || defined( _UNICODE )
      int length = WideCharToMultiByte(CP_UTF8, 0, deviceCaps.szPname, -1, NULL, 0, NULL, NULL) - 1;
      name.assign( length, 0 );
      length = WideCharToMultiByte(CP_UTF8,
				   0,
				   deviceCaps.szPname,
				   static_cast<int>(wcslen(deviceCaps.szPname)),
				   &name[0],
				   length,
				   NULL,
				   NULL);
#else
      name = deviceCaps.szPname;
#endif

    }


    std::ostringstream os;
    switch (naming) {
    case PortDescriptor::SESSION_PATH:
      if (flags & PortDescriptor::INCLUDE_API)
	os << "WinMM:";
      os << port << ":" << name.c_str();
      break;
    case PortDescriptor::STORAGE_PATH:
      if (flags & PortDescriptor::INCLUDE_API)
	os << "WinMM:";
      os << name.c_str();
      if (flags & PortDescriptor::UNIQUE_PORT_NAME)
	os << ";" << port;
      break;
    case PortDescriptor::LONG_NAME:
    case PortDescriptor::SHORT_NAME:
    default:
      os << name.c_str();
      if (flags & PortDescriptor::UNIQUE_PORT_NAME) {
	os << " ";
	os << port;
      }
      if (flags & PortDescriptor::INCLUDE_API)
	os << " (WinMM)";

      break;
    }
    return os.str();
  }

protected:
  struct scoped_lock {
    //			pthread_mutex_t * mutex;
    scoped_lock(unsigned int &)
    {
#if 0
      if (locking)
	pthread_mutex_lock(mutex);
#endif
    }
    ~scoped_lock()
    {
#if 0
      if (locking)
	pthread_mutex_unlock(mutex);
#endif
    }
  };
  // to keep the API simple
  int mutex;
  std::string name;


  void init()
  {
    // init (seq);
  }

};
//	typedef WinMMSequencer<1> LockingWinMMSequencer;
typedef WinMMSequencer<0> NonLockingWinMMSequencer;
#undef RTMIDI_CLASSNAME

struct WinMMPortDescriptor:public PortDescriptor
{
  static NonLockingWinMMSequencer seq;
  WinMMPortDescriptor(const std::string &/*cname*/):name(),port(0),clientName(name)
  {
  }
  WinMMPortDescriptor(unsigned int p, const std::string &pn, bool i_o, const std::string &n):
    name(pn),
    port(p),
    is_input(i_o),
    clientName(n)
  {
  }
  ~WinMMPortDescriptor() {}
  MidiInApi * getInputApi(unsigned int queueSizeLimit = 100) const {
    if (is_input)
      return new MidiInWinMM(clientName,queueSizeLimit);
    else
      return 0;
  }
  MidiOutApi * getOutputApi() const {
    if (!is_input)
      return new MidiOutWinMM(clientName);
    else
      return 0;
  }
  std::string getName(int flags = SHORT_NAME | UNIQUE_PORT_NAME) {
    return seq.getPortName(port,is_input,flags);
  }

  const std::string &getClientName() const {
    return clientName;
  }
  int getCapabilities() const {
    return is_input ? INPUT : OUTPUT;
  }

  int getCapabilities() {
    const WinMMPortDescriptor * self = this;
    return self->getCapabilities();
  }

  bool is_valid() const {
    if (is_input) {
      if (midiInGetNumDevs() <= port) {
	return false;
      }
    } else {
      if (midiOutGetNumDevs() <= port) {
	return false;
      }
    }
    return seq.getPortName(port,is_input,PortDescriptor::STORAGE_PATH)
      == name;
  }

  void setRemote(const WinMMPortDescriptor * remote) {
    port = remote->port;
    name = remote->name;
    is_input = remote->is_input;
  }


  unsigned int getPortNumber() const { return port; }

  static PortList getPortList(int capabilities, const std::string &clientName);
protected:
  /* There is no perfect port descriptor available in this API.
     We use the port number and issue an error if the port name has changed
     between the creation of the port descriptor and opening the port. */
  std::string name;
  unsigned int port;
  bool is_input;
  std::string clientName;
};

NonLockingWinMMSequencer WinMMPortDescriptor::seq;



PortList WinMMPortDescriptor :: getPortList(int capabilities, const std::string &clientName)
{
  PortList list;

  if (capabilities & INPUT && capabilities & OUTPUT) return list;

  if (capabilities & INPUT) {
    size_t n = midiInGetNumDevs();
    for (size_t i = 0 ; i < n ; i++) {
      std::string name = seq.getPortName(i,true,PortDescriptor::STORAGE_PATH);
      list.push_back(Pointer<PortDescriptor>(
					     new WinMMPortDescriptor(i,name,true,clientName)));
    }
  } else {
    size_t n = midiOutGetNumDevs();
    for (size_t i = 0 ; i < n ; i++) {
      std::string name = seq.getPortName(i,false,PortDescriptor::STORAGE_PATH);
      list.push_back(Pointer<PortDescriptor>(
					     new WinMMPortDescriptor(i,name,false,clientName)));
    }
  }
  return list;
}


/*! A structure to hold variables related to the WINMM API
  implementation.

  \note After all sequencer handling is covered by the \ref
  WinMMSequencer class, we should make seq to be a pointer in order
  to allow a common client implementation.
*/

struct WinMidiData:public WinMMPortDescriptor {
  /*
    WinMMMidiData():seq()
    {
    init();
    }
  */
  WinMidiData(const std::string &clientName):WinMMPortDescriptor(clientName) {}
  ~WinMidiData() {}

  HMIDIIN inHandle;    // Handle to Midi Input Device
  HMIDIOUT outHandle;  // Handle to Midi Output Device
  DWORD lastTime;
  MidiInApi::MidiMessage message;
  LPMIDIHDR sysexBuffer[RT_SYSEX_BUFFER_COUNT];
  CRITICAL_SECTION _mutex; // [Patrice] see https://groups.google.com/forum/#!topic/mididev/6OUjHutMpEo
};


//*********************************************************************//
//  API: Windows MM
//  Class Definitions: MidiInWinMM
//*********************************************************************//
#define RTMIDI_CLASSNAME "WinMMCallbacks"
//! Windows callbacks
/*! In order to avoid including too many header files in RtMidi.h, we use this
 *  class to callect all friend functions of Midi*WinMM.
 */
struct WinMMCallbacks {
  static void CALLBACK midiInputCallback( HMIDIIN /*hmin*/,
					  UINT inputStatus,
					  DWORD_PTR instancePtr,
					  DWORD_PTR midiMessage,
					  DWORD timestamp )
  {
    if ( inputStatus != MIM_DATA && inputStatus != MIM_LONGDATA && inputStatus != MIM_LONGERROR ) return;

    //MidiInApi::MidiInData *data = static_cast<MidiInApi::MidiInData *> (instancePtr);
    MidiInWinMM *data = (MidiInWinMM *)instancePtr;
    WinMidiData *apiData = static_cast<WinMidiData *> (data->apiData_);

    // Calculate time stamp.
    if ( data->firstMessage == true ) {
      apiData->message.timeStamp = 0.0;
      data->firstMessage = false;
    }
    else apiData->message.timeStamp = (double) ( timestamp - apiData->lastTime ) * 0.001;

    if ( inputStatus == MIM_DATA ) { // Channel or system message

      // Make sure the first byte is a status byte.
      unsigned char status = (unsigned char) (midiMessage & 0x000000FF);
      if ( !(status & 0x80) ) return;

      // Determine the number of bytes in the MIDI message.
      unsigned short nBytes = 1;
      if ( status < 0xC0 ) nBytes = 3;
      else if ( status < 0xE0 ) nBytes = 2;
      else if ( status < 0xF0 ) nBytes = 3;
      else if ( status == 0xF1 ) {
	if ( data->ignoreFlags & 0x02 ) return;
	else nBytes = 2;
      }
      else if ( status == 0xF2 ) nBytes = 3;
      else if ( status == 0xF3 ) nBytes = 2;
      else if ( status == 0xF8 && (data->ignoreFlags & 0x02) ) {
	// A MIDI timing tick message and we're ignoring it.
	return;
      }
      else if ( status == 0xFE && (data->ignoreFlags & 0x04) ) {
	// A MIDI active sensing message and we're ignoring it.
	return;
      }

      // Copy bytes to our MIDI message.
      unsigned char *ptr = (unsigned char *) &midiMessage;
      for ( int i=0; i<nBytes; ++i ) apiData->message.bytes.push_back( *ptr++ );
    }
    else { // Sysex message ( MIM_LONGDATA or MIM_LONGERROR )
      MIDIHDR *sysex = ( MIDIHDR *) midiMessage;
      if ( !( data->ignoreFlags & 0x01 ) && inputStatus != MIM_LONGERROR ) {
	// Sysex message and we're not ignoring it
	for ( int i=0; i<(int)sysex->dwBytesRecorded; ++i )
	  apiData->message.bytes.push_back( sysex->lpData[i] );
      }

      // The WinMM API requires that the sysex buffer be requeued after
      // input of each sysex message.  Even if we are ignoring sysex
      // messages, we still need to requeue the buffer in case the user
      // decides to not ignore sysex messages in the future.  However,
      // it seems that WinMM calls this function with an empty sysex
      // buffer when an application closes and in this case, we should
      // avoid requeueing it, else the computer suddenly reboots after
      // one or two minutes.
      if ( apiData->sysexBuffer[sysex->dwUser]->dwBytesRecorded > 0 ) {
	//if ( sysex->dwBytesRecorded > 0 ) {
	EnterCriticalSection( &(apiData->_mutex) );
	MMRESULT result = midiInAddBuffer( apiData->inHandle, apiData->sysexBuffer[sysex->dwUser], sizeof(MIDIHDR) );
	LeaveCriticalSection( &(apiData->_mutex) );
	if ( result != MMSYSERR_NOERROR ){
	  try {
	    data->error(RTMIDI_ERROR(rtmidi_gettext("Error sending sysex to Midi device."),
				     Error::WARNING));
	  } catch (Error & e) {
	    // don't bother WinMM with an unhandled exception
	  }
	}

	if ( data->ignoreFlags & 0x01 ) return;
      }
      else return;
    }

    // Save the time of the last non-filtered message
    apiData->lastTime = timestamp;

    if ( data->userCallback ) {
      data->userCallback->rtmidi_midi_in( apiData->message.timeStamp, apiData->message.bytes );
    }
    else {
      // As long as we haven't reached our queue size limit, push the message.
      if (!data->queue.push(apiData->message)) {
	try {
	  data->error(RTMIDI_ERROR(rtmidi_gettext("Error: Message queue limit reached."),
				   Error::WARNING));
	} catch (Error & e) {
	  // don't bother WinMM with an unhandled exception
	}
      }
    }

    // Clear the vector for the next input message.
    apiData->message.bytes.clear();
  }
};
#undef RTMIDI_CLASSNAME

#define RTMIDI_CLASSNAME "MidiInWinMM"
MidiInWinMM :: MidiInWinMM( const std::string &clientName,
			    unsigned int queueSizeLimit ) : MidiInApi( queueSizeLimit )
{
  MidiInWinMM::initialize( clientName );
}

MidiInWinMM :: ~MidiInWinMM()
{
  // Close a connection if it exists.
  MidiInWinMM::closePort();

  WinMidiData *data = static_cast<WinMidiData *> (apiData_);
  DeleteCriticalSection( &(data->_mutex) );

  // Cleanup.
  delete data;
}

void MidiInWinMM :: initialize( const std::string &clientName )
{
  // We'll issue a warning here if no devices are available but not
  // throw an error since the user can plugin something later.
  unsigned int nDevices = midiInGetNumDevs();
  if ( nDevices == 0 ) {
    error(RTMIDI_ERROR(gettext_noopt("No MIDI input devices currently available."),
		       Error::WARNING) );
  }

  // Save our api-specific connection information.
  WinMidiData *data = (WinMidiData *) new WinMidiData(clientName);
  apiData_ = (void *) data;
  data->message.bytes.clear();  // needs to be empty for first input message

  if ( !InitializeCriticalSectionAndSpinCount(&(data->_mutex), 0x00000400) ) {
    error(RTMIDI_ERROR(gettext_noopt("Failed to initialize a critical section."),
		       Error::WARNING) );
  }
}

void MidiInWinMM :: openPort( unsigned int portNumber, const std::string &/*portName*/ )
{
  if ( connected_ ) {
    error(RTMIDI_ERROR(gettext_noopt("A valid connection already exists."),
		       Error::WARNING) );
    return;
  }

  unsigned int nDevices = midiInGetNumDevs();
  if (nDevices == 0) {
    error(RTMIDI_ERROR(gettext_noopt("No MIDI input sources found."),
		       Error::NO_DEVICES_FOUND) );
    return;
  }

  if ( portNumber >= nDevices ) {
    std::ostringstream ost;
    ost << "MidiInWinMM::openPort: ";
    errorString_ = ost.str();
    error(RTMIDI_ERROR1(gettext_noopt("the 'portNumber' argument (%d) is invalid."),
			Error::INVALID_PARAMETER, portNumber) );
    return;
  }

  WinMidiData *data = static_cast<WinMidiData *> (apiData_);
  MMRESULT result = midiInOpen( &data->inHandle,
				portNumber,
				(DWORD_PTR)&WinMMCallbacks::midiInputCallback,
				(DWORD_PTR)this,
				CALLBACK_FUNCTION );
  if ( result != MMSYSERR_NOERROR ) {
    error(RTMIDI_ERROR(gettext_noopt("Error creating Windows MM MIDI input port."),
		       Error::DRIVER_ERROR) );
    return;
  }

  // Allocate and init the sysex buffers.
  for ( int i=0; i<RT_SYSEX_BUFFER_COUNT; ++i ) {
    data->sysexBuffer[i] = (MIDIHDR*) new char[ sizeof(MIDIHDR) ];
    data->sysexBuffer[i]->lpData = new char[ RT_SYSEX_BUFFER_SIZE ];
    data->sysexBuffer[i]->dwBufferLength = RT_SYSEX_BUFFER_SIZE;
    data->sysexBuffer[i]->dwUser = i; // We use the dwUser parameter as buffer indicator
    data->sysexBuffer[i]->dwFlags = 0;

    result = midiInPrepareHeader( data->inHandle, data->sysexBuffer[i], sizeof(MIDIHDR) );
    if ( result != MMSYSERR_NOERROR ) {
      midiInClose( data->inHandle );
      data->inHandle = 0;
      error(RTMIDI_ERROR(gettext_noopt("Error initializing data for Windows MM MIDI input port."),
			 Error::DRIVER_ERROR ));
      return;
    }

    // Register the buffer.
    result = midiInAddBuffer( data->inHandle, data->sysexBuffer[i], sizeof(MIDIHDR) );
    if ( result != MMSYSERR_NOERROR ) {
      midiInClose( data->inHandle );
      data->inHandle = 0;
      error(RTMIDI_ERROR(gettext_noopt("Could not register the input buffer for Windows MM MIDI input port."),
			 Error::DRIVER_ERROR) );
      return;
    }
  }

  result = midiInStart( data->inHandle );
  if ( result != MMSYSERR_NOERROR ) {
    midiInClose( data->inHandle );
    data->inHandle = 0;
    error(RTMIDI_ERROR(gettext_noopt("Error starting Windows MM MIDI input port."),
		       Error::DRIVER_ERROR) );
    return;
  }

  connected_ = true;
}

void MidiInWinMM :: openVirtualPort(const std::string &/*portName*/ )
{
  // This function cannot be implemented for the Windows MM MIDI API.
  error(RTMIDI_ERROR(gettext_noopt("Virtual ports are not available Windows Multimedia MIDI API."),
		     Error::WARNING ));
}

void MidiInWinMM :: openPort(const PortDescriptor & p, const std::string &portName) {
  const WinMMPortDescriptor * port = dynamic_cast <const WinMMPortDescriptor * >(&p);
  if ( !port) {
    error( RTMIDI_ERROR(gettext_noopt("Windows Multimedia (WinMM) has been instructed to open a non-WinMM MIDI port. This doesn't work."),
			Error::INVALID_DEVICE));
    return;
  }
  if ( connected_ ) {
    error( RTMIDI_ERROR(gettext_noopt("We are overwriting an existing connection. This is probably a programming error."),
			Error::WARNING) );
    return;
  }
  if (port->getCapabilities() != PortDescriptor::INPUT) {
    error(RTMIDI_ERROR(gettext_noopt("Trying to open a non-input port as input MIDI port. This doesn't work."),
		       Error::INVALID_DEVICE));
    return;
  }

  // there is a possible race condition between opening the port and
  // reordering of ports so we must check whether we opened the right port.
  try {
    openPort(port->getPortNumber(),portName);
  } catch (Error & e) {
    error(e);
  }
  if (!port->is_valid()) {
    closePort();
    error (RTMIDI_ERROR(gettext_noopt("Some change in the arrangement of MIDI input ports invalidated the port descriptor."),
			Error::DRIVER_ERROR));
    return;
  }
  connected_ = true;
}

Pointer<PortDescriptor> MidiInWinMM :: getDescriptor(bool local)
{
  if (local || !connected_) return 0;
  WinMidiData *data = static_cast<WinMidiData *> (apiData_);
  if (!data) return 0;
  UINT devid;
  switch (midiInGetID(data->inHandle,&devid)) {
  case MMSYSERR_INVALHANDLE:
    error (RTMIDI_ERROR(gettext_noopt("The handle is invalid. Did you disconnect the device?"),
			Error::DRIVER_ERROR));
    return 0;
  case MMSYSERR_NODRIVER:
    error (RTMIDI_ERROR(gettext_noopt("The system has no driver for our handle :-(. Did you disconnect the device?"),
			Error::DRIVER_ERROR));
    return 0;
  case MMSYSERR_NOMEM:
    error (RTMIDI_ERROR(gettext_noopt("Out of memory."),
			Error::DRIVER_ERROR));
    return 0;
  }
  WinMMPortDescriptor * retval = NULL;
  try {
    retval = new WinMMPortDescriptor(devid, getPortName(devid), true, data->getClientName());
  } catch (Error & e) {
    try {
      error(e);
    } catch (...) {
      if (retval) delete retval;
      throw;
    }
  }
  return Pointer<PortDescriptor>(retval);

}

PortList MidiInWinMM :: getPortList(int capabilities)
{
  WinMidiData *data = static_cast<WinMidiData *> (apiData_);
  if (!data || capabilities != PortDescriptor::INPUT) return PortList();
  try {
    return WinMMPortDescriptor::getPortList(PortDescriptor::INPUT,data->getClientName());
  } catch (Error & e) {
    error(e);
    return PortList();
  }
}


void MidiInWinMM :: closePort( void )
{
  if ( connected_ ) {
    WinMidiData *data = static_cast<WinMidiData *> (apiData_);
    EnterCriticalSection( &(data->_mutex) );
    midiInReset( data->inHandle );
    midiInStop( data->inHandle );

    for ( int i=0; i<RT_SYSEX_BUFFER_COUNT; ++i ) {
      int result = midiInUnprepareHeader(data->inHandle, data->sysexBuffer[i], sizeof(MIDIHDR));
      delete [] data->sysexBuffer[i]->lpData;
      delete [] data->sysexBuffer[i];
      if ( result != MMSYSERR_NOERROR ) {
        midiInClose( data->inHandle );
        data->inHandle = 0;
	error(RTMIDI_ERROR(gettext_noopt("Error closing Windows MM MIDI input port."),
			   Error::DRIVER_ERROR) );
	return;
      }
    }

    midiInClose( data->inHandle );
    data->inHandle = 0;
    connected_ = false;
    LeaveCriticalSection( &(data->_mutex) );
  }
}

void MidiInWinMM :: setClientName ( const std::string& )
{
  error(RTMIDI_ERROR(gettext_noopt("Setting the client name is not supported by Windows MM."),
		     Error::WARNING ));
}

void MidiInWinMM :: setPortName ( const std::string& )
{
  error(RTMIDI_ERROR(gettext_noopt("Setting the port name is not supported by Windows MM."),
		     Error::WARNING ));
}

unsigned int MidiInWinMM :: getPortCount()
{
  return midiInGetNumDevs();
}

std::string MidiInWinMM :: getPortName( unsigned int portNumber )
{
  std::string stringName;
  unsigned int nDevices = midiInGetNumDevs();
  if ( portNumber >= nDevices ) {
    error(RTMIDI_ERROR1(gettext_noopt("The 'portNumber' argument (%d) is invalid."),
			Error::WARNING,portNumber));
    return stringName;
  }

  MIDIINCAPS deviceCaps;
  midiInGetDevCaps( portNumber, &deviceCaps, sizeof(MIDIINCAPS));
  stringName = ConvertToUTF8( deviceCaps.szPname );

  // Next lines added to add the portNumber to the name so that
  // the device's names are sure to be listed with individual names
  // even when they have the same brand name
#ifndef RTMIDI_DO_NOT_ENSURE_UNIQUE_PORTNAMES
  std::ostringstream os;
  os << " ";
  os << portNumber;
  stringName += os.str();
#endif

  return stringName;
}
#undef RTMIDI_CLASSNAME



//*********************************************************************//
//  API: Windows MM
//  Class Definitions: MidiOutWinMM
//*********************************************************************//

#define RTMIDI_CLASSNAME "MidiOutWinMM"
MidiOutWinMM :: MidiOutWinMM( const std::string &clientName ) : MidiOutApi()
{
  MidiOutWinMM::initialize( clientName );
}

MidiOutWinMM :: ~MidiOutWinMM()
{
  // Close a connection if it exists.
  MidiOutWinMM::closePort();

  // Cleanup.
  WinMidiData *data = static_cast<WinMidiData *> (apiData_);
  delete data;
}

void MidiOutWinMM :: initialize( const std::string &clientName )
{
  // We'll issue a warning here if no devices are available but not
  // throw an error since the user can plug something in later.
  unsigned int nDevices = midiOutGetNumDevs();
  if ( nDevices == 0 ) {
    error(RTMIDI_ERROR(gettext_noopt("No MIDI output devices currently available."),
		       Error::WARNING));
  }

  // Save our api-specific connection information.
  WinMidiData *data = (WinMidiData *) new WinMidiData(clientName);
  apiData_ = (void *) data;
}

unsigned int MidiOutWinMM :: getPortCount()
{
  return midiOutGetNumDevs();
}

std::string MidiOutWinMM :: getPortName( unsigned int portNumber )
{
  std::string stringName;
  unsigned int nDevices = midiOutGetNumDevs();
  if ( portNumber >= nDevices ) {
    std::ostringstream ost;
    ost << "MidiOutWinMM::getPortName: ";
    errorString_ = ost.str();
    error(RTMIDI_ERROR(gettext_noopt("The 'portNumber' argument (%d) is invalid."),
		       Error::WARNING));
    return stringName;
  }

  MIDIOUTCAPS deviceCaps;
  midiOutGetDevCaps( portNumber, &deviceCaps, sizeof(MIDIOUTCAPS));
  stringName = ConvertToUTF8( deviceCaps.szPname );

  // Next lines added to add the portNumber to the name so that
  // the device's names are sure to be listed with individual names
  // even when they have the same brand name
  std::ostringstream os;
#ifndef RTMIDI_DO_NOT_ENSURE_UNIQUE_PORTNAMES
  os << " ";
  os << portNumber;
  stringName += os.str();
#endif

  return stringName;
}


void MidiOutWinMM :: openPort( unsigned int portNumber, const std::string &/*portName*/ )
{
  if ( connected_ ) {
    error(RTMIDI_ERROR(gettext_noopt("A valid connection already exists."),
		       Error::WARNING) );
    return;
  }

  unsigned int nDevices = midiOutGetNumDevs();
  if (nDevices < 1) {
    error(RTMIDI_ERROR(gettext_noopt("No MIDI output destinations found!"),
		       Error::NO_DEVICES_FOUND) );
    return;
  }

  if ( portNumber >= nDevices ) {
    error(RTMIDI_ERROR1(gettext_noopt("The 'portNumber' argument (%d) is invalid."),
			Error::INVALID_PARAMETER, portNumber) );
    return;
  }

  WinMidiData *data = static_cast<WinMidiData *> (apiData_);
  MMRESULT result = midiOutOpen( &data->outHandle,
				 portNumber,
				 (DWORD)NULL,
				 (DWORD)NULL,
				 CALLBACK_NULL );
  if ( result != MMSYSERR_NOERROR ) {
    error(RTMIDI_ERROR(gettext_noopt("Error creating Windows MM MIDI output port."),
		       Error::DRIVER_ERROR) );
    return;
  }

  connected_ = true;
}

void MidiOutWinMM :: closePort( void )
{
  if ( connected_ ) {
    WinMidiData *data = static_cast<WinMidiData *> (apiData_);
    midiOutReset( data->outHandle );
    midiOutClose( data->outHandle );
    data->outHandle = 0;
    connected_ = false;
  }
}

void MidiOutWinMM :: setClientName ( const std::string& )
{
  error(RTMIDI_ERROR(gettext_noopt("Setting the client name is not supported by Windows MM."),
		     Error::WARNING ));
}

void MidiOutWinMM :: setPortName ( const std::string& )
{
  error(RTMIDI_ERROR(gettext_noopt("Setting the port name is not supported by Windows MM."),
		     Error::WARNING ));
}

void MidiOutWinMM :: openVirtualPort(const std::string &/*portName*/ )
{
  // This function cannot be implemented for the Windows MM MIDI API.
  error(RTMIDI_ERROR(gettext_noopt("Virtual ports are not available Windows Multimedia MIDI API."),
		     Error::WARNING) );
}


void MidiOutWinMM :: openPort(const PortDescriptor & p, const std::string &portName) {
  const WinMMPortDescriptor * port = dynamic_cast <const WinMMPortDescriptor * >(&p);
  if ( !port) {
    error( RTMIDI_ERROR(gettext_noopt("Windows Multimedia (WinMM) has been instructed to open a non-WinMM MIDI port. This doesn't work."),
			Error::INVALID_DEVICE));
    return;
  }
  if ( connected_ ) {
    error( RTMIDI_ERROR(gettext_noopt("A valid connection already exists." ),
			Error::WARNING) );
    return;
  }
  if (port->getCapabilities() != PortDescriptor::OUTPUT) {
    error( RTMIDI_ERROR(gettext_noopt("The port descriptor cannot be used to open an output port."),
			Error::DRIVER_ERROR));
    return;
  }

  // there is a possible race condition between opening the port and
  // reordering of ports so we must check whether we opened the right port.
  try {
    openPort(port->getPortNumber(),portName);
  } catch (Error & e) {
    error(e);
  }
  if (!port->is_valid()) {
    closePort();
    error (RTMIDI_ERROR(gettext_noopt("Some change in the arrangement of MIDI input ports invalidated the port descriptor."),
			Error::DRIVER_ERROR));
    return;
  }
  connected_ = true;
}

Pointer<PortDescriptor> MidiOutWinMM :: getDescriptor(bool local)
{
  if (local || !connected_) return 0;
  WinMidiData *data = static_cast<WinMidiData *> (apiData_);
  if (!data) return 0;
  UINT devid;
  switch (midiOutGetID(data->outHandle,&devid)) {
  case MMSYSERR_INVALHANDLE:
    error (RTMIDI_ERROR(gettext_noopt("The internal handle is invalid. Did you disconnect the device?"),
			Error::DRIVER_ERROR));
    return 0;
  case MMSYSERR_NODRIVER:
    error (RTMIDI_ERROR(gettext_noopt("The system has no driver for our handle :-(. Did you disconnect the device?"),
			Error::DRIVER_ERROR));
    return 0;
  case MMSYSERR_NOMEM:
    error (RTMIDI_ERROR(gettext_noopt("Out of memory."),
			Error::DRIVER_ERROR));
    return 0;
  }
  return Pointer<PortDescriptor>(
				 new WinMMPortDescriptor(devid, getPortName(devid), true, data->getClientName()));

}

PortList MidiOutWinMM :: getPortList(int capabilities)
{
  WinMidiData *data = static_cast<WinMidiData *> (apiData_);
  if (!data || capabilities != PortDescriptor::OUTPUT) return PortList();
  try {
    return WinMMPortDescriptor::getPortList(PortDescriptor::OUTPUT,data->getClientName());
  } catch (Error & e) {
    error(e);
    return PortList();
  }
}


void MidiOutWinMM :: sendMessage( const unsigned char *message, size_t size )
{
  if ( !connected_ ) return;

  if ( size == 0 ) {
    error(RTMIDI_ERROR(gettext_noopt("Message argument is empty."),
		       Error::WARNING));
    return;
  }

  MMRESULT result;
  WinMidiData *data = static_cast<WinMidiData *> (apiData_);
  if ( message[0] == 0xF0 ) { // Sysex message

    // Allocate buffer for sysex data.
    char *buffer = (char *) malloc( size );
    if ( buffer == NULL ) {
      error(RTMIDI_ERROR(gettext_noopt("Error while allocating sysex message memory."),
			 Error::MEMORY_ERROR) );
      return;
    }

    // Copy data to buffer.
    for ( unsigned int i=0; i<size; ++i ) buffer[i] = message[i];

    // Create and prepare MIDIHDR structure.
    MIDIHDR sysex;
    sysex.lpData = (LPSTR) buffer;
    sysex.dwBufferLength = size;
    sysex.dwFlags = 0;
    result = midiOutPrepareHeader( data->outHandle,  &sysex, sizeof(MIDIHDR) );
    if ( result != MMSYSERR_NOERROR ) {
      free( buffer );
      error(RTMIDI_ERROR(gettext_noopt("Error preparing sysex header."),
			 Error::DRIVER_ERROR));
      return;
    }

    // Send the message.
    result = midiOutLongMsg( data->outHandle, &sysex, sizeof(MIDIHDR) );
    if ( result != MMSYSERR_NOERROR ) {
      free( buffer );
      error(RTMIDI_ERROR(gettext_noopt("Error sending sysex message."),
			 Error::DRIVER_ERROR) );
      return;
    }

    // Unprepare the buffer and MIDIHDR.
    while ( MIDIERR_STILLPLAYING == midiOutUnprepareHeader( data->outHandle, &sysex, sizeof (MIDIHDR) ) ) Sleep( 1 );
    free( buffer );
  }
  else { // Channel or system message.

    // Make sure the message size isn't too big.
    if ( size > 3 ) {
      error(RTMIDI_ERROR(gettext_noopt("Message size is greater than 3 bytes (and not sysex)."),
			 Error::WARNING) );
      return;
    }

    // Pack MIDI bytes into double word.
    DWORD packet;
    unsigned char *ptr = (unsigned char *) &packet;
    for ( unsigned int i=0; i<size; ++i ) {
      *ptr = message[i];
      ++ptr;
    }

    // Send the message immediately.
    result = midiOutShortMsg( data->outHandle, packet );
    if ( result != MMSYSERR_NOERROR ) {
      error(RTMIDI_ERROR(gettext_noopt("Error sending MIDI message."),
			 Error::DRIVER_ERROR ));
    }
  }
}
#undef RTMIDI_CLASSNAME
RTMIDI_NAMESPACE_END
#endif  // __WINDOWS_MM__


//*********************************************************************//
//  API: UNIX JACK
//
//  Written primarily by Alexander Svetalkin, with updates for delta
//  time by Gary Scavone, April 2011.
//
//  *********************************************************************//

#if defined(__UNIX_JACK__)

// JACK header files
#include <jack/jack.h>
#include <jack/midiport.h>
#include <jack/ringbuffer.h>
#ifdef HAVE_SEMAPHORE
  #include <semaphore.h>
#endif

#define JACK_RINGBUFFER_SIZE 16384 // Default size for ringbuffer

RTMIDI_NAMESPACE_START

struct JackMidiData;
struct JackBackendCallbacks {
  static int jackProcessIn( jack_nframes_t nframes, void *arg );
  static int jackProcessOut( jack_nframes_t nframes, void *arg );
};


#define RTMIDI_CLASSNAME "JackSequencer"
template <int locking=1>
class JackSequencer {
public:
  JackSequencer():client(0),name(),data(0)
  {
    if (locking) {
      pthread_mutexattr_t attr;
      pthread_mutexattr_init(&attr);
      pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
      pthread_mutex_init(&mutex, &attr);
    }
  }

  JackSequencer(const std::string &n, JackMidiData * d):client(0),name(n),data(d)
  {
    if (locking) {
      pthread_mutexattr_t attr;
      pthread_mutexattr_init(&attr);
      pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
      pthread_mutex_init(&mutex, &attr);
    }
  }

  ~JackSequencer()
  {
    {
      scoped_lock lock (mutex);
      if (client) {
	jack_deactivate (client);
	// the latter doesn't flush the queue
	jack_client_close (client);
	client = 0;
      }
    }
    if (locking) {
      pthread_mutex_destroy(&mutex);
    }
  }

  void init(bool startqueue) {
    init(client,startqueue);
  }

  bool setName(const std::string &n) {
    /* we don't want to rename the client after opening it. */
    if (client) return false;
    name = n;
    return true;
  }

  const char ** getPortList(unsigned long flags) {
    init();
    return jack_get_ports(client,
			  NULL,
			  "midi",
			  flags);
  }

  jack_port_t * getPort(const char * name) {
    init();
    return jack_port_by_name(client,name);
  }

  std::string getPortName(jack_port_t * port, int flags) {
    init();
    int naming = flags & PortDescriptor::NAMING_MASK;

    std::ostringstream os;
    switch (naming) {
    case PortDescriptor::SESSION_PATH:
      if (flags & PortDescriptor::INCLUDE_API)
	os << "JACK:";
#if __UNIX_JACK_HAS_UUID__
      os << "UUID:" << std::hex << jack_port_uuid(port);
#else
      os << jack_port_name(port);
#endif
      break;
    case PortDescriptor::STORAGE_PATH:
      if (flags & PortDescriptor::INCLUDE_API)
	os << "JACK:";
      os << jack_port_name(port);
      break;
    case PortDescriptor::LONG_NAME:
      os << jack_port_name(port);
      if (flags & PortDescriptor::INCLUDE_API)
	os << " (JACK)";
      break;
    case PortDescriptor::SHORT_NAME:
    default:
      os << jack_port_short_name(port);
      if (flags & PortDescriptor::INCLUDE_API)
	os << " (JACK)";
      break;
    }
    return os.str();
  }

  int getPortCapabilities(jack_port_t * port) {
    if (!port) return 0;
    const char * type = jack_port_type(port);
    if (strcmp(type,JACK_DEFAULT_MIDI_TYPE)) return 0;
    int flags = jack_port_flags(port);
    int retval = 0;
    /* a JACK input port is capable of handling output to it and vice versa */
    if (flags & JackPortIsInput)
      retval |= PortDescriptor::OUTPUT;
    if (flags & JackPortIsOutput)
      retval |= PortDescriptor::INPUT;

    return retval;
  }


  jack_port_t * createPort (const std::string &portName, unsigned long portOptions) {
    init();
    scoped_lock lock (mutex);
    return jack_port_register(client,
			      portName.c_str(),
			      JACK_DEFAULT_MIDI_TYPE,
			      portOptions,
			      0);
  }

  int renamePort(jack_port_t * port, const std::string &portName) {
#ifdef JACK_HAS_PORT_RENAME
    return jack_port_rename( client, port, portName.c_str() );
#else
    return jack_port_set_name( port, portName.c_str() );
#endif
  }
  
  void deletePort(jack_port_t * port) {
    init();
    scoped_lock lock (mutex);
    jack_port_unregister( client, port );
  }

  void connectPorts(jack_port_t * from,
		    jack_port_t * to)
  {
    init();
    jack_connect( client,
		  jack_port_name( from ),
		  jack_port_name( to ) );
  }

  void closePort(jack_port_t * port)
  {
    init();
    jack_port_disconnect( client, port);
  }


  /*! Use JackSequencer like a C pointer.
    \note This function breaks the design to control thread safety
    by the selection of the \ref locking parameter to the class.
    It should be removed as soon as possible in order ensure the
    thread policy that has been intended by creating this class.
  */
  operator jack_client_t * ()
  {
    return client;
  }

protected:
  struct scoped_lock {
    pthread_mutex_t * mutex;
    scoped_lock(pthread_mutex_t & m): mutex(&m)
    {
      if (locking)
	pthread_mutex_lock(mutex);
    }
    ~scoped_lock()
    {
      if (locking)
	pthread_mutex_unlock(mutex);
    }
  };
  pthread_mutex_t mutex;
  jack_client_t * client;
  std::string name;
  JackMidiData * data;


  void init()
  {
    init (client,false);
  }

  void init(jack_client_t * &c, bool isoutput)
  {
    if (c) return;
    {
      scoped_lock lock(mutex);
      if (( c = jack_client_open( name.c_str(),
				  JackNoStartServer,
				  NULL )) == 0) {
	c = NULL;
	throw RTMIDI_ERROR(gettext_noopt("Could not connect to JACK server. Is it runnig?"),
			   Error::NO_DEVICES_FOUND);
	return;
      }

      if (isoutput && data) {
	jack_set_process_callback( c, JackBackendCallbacks::jackProcessOut, data );
      } else if (data)
	jack_set_process_callback( c, JackBackendCallbacks::jackProcessIn, data );
      jack_activate( c );
    }
  }
};
typedef JackSequencer<1> LockingJackSequencer;
typedef JackSequencer<0> NonLockingJackSequencer;
#undef RTMIDI_CLASSNAME

#define RTMIDI_CLASSNAME "JackPortDescriptor"
struct JackPortDescriptor:public PortDescriptor
{
  MidiApi * api;
  static LockingJackSequencer seq;
  JackPortDescriptor(const std::string &name):api(0),clientName(name)
  {
    port = 0;
  }
  JackPortDescriptor(const char * portname, const std::string &name):api(0),clientName(name)
  {
    seq.setName(name);
    port = seq.getPort(portname);
  }
  JackPortDescriptor(jack_port_t * other,
		     const std::string &name):api(0),
					       clientName(name)
  {
    port = other;
    seq.setName(name);
  }
  JackPortDescriptor(JackPortDescriptor & other,
		     const std::string &name):api(0),
					       clientName(name)
  {
    port = other.port;
    seq.setName(name);
  }
  ~JackPortDescriptor()
  {
  }
  MidiInApi * getInputApi(unsigned int queueSizeLimit = 100) const {
    if (getCapabilities() & INPUT)
      return new MidiInJack(clientName,queueSizeLimit);
    else
      return 0;
  }
  MidiOutApi * getOutputApi() const {
    if (getCapabilities() & OUTPUT)
      return new MidiOutJack(clientName);
    else
      return 0;
  }


  std::string getName(int flags = SHORT_NAME | UNIQUE_PORT_NAME) {
    return seq.getPortName(port,flags);
  }

  const std::string &getClientName() {
    return clientName;
  }
  int getCapabilities() const {
    return seq.getPortCapabilities(port);
  }
  static PortList getPortList(int capabilities, const std::string &clientName);

  operator jack_port_t * () const { return port; }

protected:
  std::string clientName;
  jack_port_t * port;

  friend struct JackMidiData;
};

LockingJackSequencer JackPortDescriptor::seq;



PortList JackPortDescriptor :: getPortList(int capabilities, const std::string &clientName)
{
  PortList list;
  unsigned long flags = 0;

  if (capabilities & INPUT) {
    flags |= JackPortIsOutput;
  }
  if (capabilities & OUTPUT) {
    flags |= JackPortIsInput;
  }
  const char ** ports = seq.getPortList(flags);
  if (!ports) return list;
  for (const char ** port = ports; *port; port++) {
    list.push_back(Pointer<PortDescriptor>(
					   new JackPortDescriptor(*port, clientName)));
  }
  jack_free(ports);
  return list;
}
#undef RTMIDI_CLASSNAME

/*! A structure to hold variables related to the JACK API
  implementation.

  \note After all sequencer handling is covered by the \ref
  JackSequencer class, we should make seq to be a pointer in order
  to allow a common client implementation.
*/

#define RTMIDI_CLASSNAME "JackMidiData"
struct JackMidiData:public JackPortDescriptor {
  /* signal the JACK process what to do next */
  volatile enum {
    RUNNING, /*!< keep the client open, flag is owned by the controlling process */
    CLOSING, /*!< close the current port */
    DELETING /*!< Delete the client after delivering the contents of the ring buffer */
  } stateflags;
  /*! response/state from the JACK thread. See \ref jackProcessOut for details */
  volatile enum {
    OPEN,
    CLOSING2,
    CLOSED,
    DELETING2,
    DELETING3
    /* DELETED is useless as this doesn't exist anymore */
  } state_response;

  jack_port_t * local;
  jack_ringbuffer_t *buffSize;
  jack_ringbuffer_t *buffMessage;
  jack_time_t lastTime;
#ifdef HAVE_SEMAPHORE
  sem_t sem_cleanup;
  sem_t sem_needpost;
#endif
  MidiInJack *rtMidiIn;
  /*! Sequencer object: This must be deleted _before_ the MIDI data to avoid
    segmentation faults while queued data is still in the ring buffer. */
  NonLockingJackSequencer * seq;

  /*
    JackMidiData():seq()
    {
    init();
    }
  */
  JackMidiData(const std::string &clientName,
	       MidiInJack * inputData_):JackPortDescriptor(clientName),
					stateflags(RUNNING),
					local(0),
					buffSize(0),
					buffMessage(0),
					lastTime(0),
					rtMidiIn(inputData_),
					seq(new NonLockingJackSequencer(clientName,this))
  {
#ifdef HAVE_SEMAPHORE
    sem_init(&sem_cleanup, 0, 0);
    sem_init(&sem_needpost, 0, 0);
#endif
  }

  /**
   * Create output midi data.
   *
   * \param clientName
   *
   * \return
   */
  JackMidiData(const std::string &clientName):JackPortDescriptor(clientName),
					      stateflags(RUNNING),
					      local(0),
					      buffSize(jack_ringbuffer_create( JACK_RINGBUFFER_SIZE )),
					      buffMessage(jack_ringbuffer_create( JACK_RINGBUFFER_SIZE )),
					      lastTime(0),
					      rtMidiIn(),
					      seq(new NonLockingJackSequencer(clientName,this))
  {
#ifdef HAVE_SEMAPHORE
    sem_init(&sem_cleanup, 0, 0);
    sem_init(&sem_needpost, 0, 0);
#endif
  }


  ~JackMidiData()
  {
    if (local)
      deletePort();
    if (seq)
      delete seq;
#ifdef HAVE_SEMAPHORE
    sem_destroy(&sem_cleanup);
    sem_destroy(&sem_needpost);
#endif

    if (buffSize) {
      jack_ringbuffer_free( buffSize );
      buffSize = 0;
    }
    if (buffMessage) {
      jack_ringbuffer_free( buffMessage );
      buffMessage = 0;
    }
  }

  void init(bool isinput) {
    seq->init(!isinput);
  }


  void setRemote(jack_port_t * remote) {
    port   = remote;
  }

  void connectPorts(jack_port_t * from,
		    jack_port_t * to) {
    seq->connectPorts(from, to);
  }

  int openPort(unsigned long jackCapabilities,
	       const std::string &portName) {
    local = seq->createPort(portName, jackCapabilities);
    if (!local) {
      api->error(RTMIDI_ERROR(gettext_noopt("Error opening JACK port subscription."),
			      Error::DRIVER_ERROR) );
      return -99;
    }
    return 0;
  }

  int rename(const std::string &portName) {
    return seq->renamePort(local,portName);
  }

  void delayedDeletePort() {
    /* Closing the port can be twofold to ensure all data is sent:
       - Use a semaphore to wait for this state
       - Close the port from within jackProcessOut
    */
    if (local == NULL) return;

#ifdef HAVE_SEMAPHORE
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) != -1)
      {
	ts.tv_sec += 1; // wait max one second
	sem_post(&sem_needpost);
	sem_timedwait(&sem_cleanup, &ts);
      }

    deletePort();
#else
    if ( local == NULL || state_response == JackMidiData::CLOSED ) return;
    stateflags = JackMidiData::CLOSING;
#endif
#if defined(__RTMIDI_DEBUG__)
    std::cerr << "Closed Port" << std::endl;
#endif
  }

  void request_delete() {
    // signal the output callback to delete the data
    // after finishing its job.
    // this can be done twofold:
    //   - via signal in jackProcessOut
    //   - using a semaphore
#ifdef HAVE_SEMAPHORE
    closePort();

    // Cleanup
    delete this;
    return;
#else
    stateflags = JackMidiData::DELETING;
#endif
  }

  void deletePortIfRequested() {
#ifdef HAVE_SEMAPHORE
    if (!sem_trywait(&sem_needpost))
      sem_post(&sem_cleanup);
#else
    switch (stateflags) {
    case JackMidiData::RUNNING: break;
    case JackMidiData::CLOSING:
      if (state_response != JackMidiData::CLOSING2) {
	/* output the transferred data */
	state_response = JackMidiData::CLOSING2;
	return;
      }
      deletePort();
      state_response = JackMidiData::CLOSED;
      break;

    case JackMidiData::DELETING:
#if defined(__RTMIDI_DEBUG__)
      std::cerr << "deleting port" << std::endl;
#endif
      if (state_response != JackMidiData::DELETING2) {
	state_response = JackMidiData::DELETING2;
	/* output the transferred data */
	return;
      }

      delete this;
      return;
#if defined(__RTMIDI_DEBUG__)
      std::cerr << "deleted port" << std::endl;
#endif
      break;
    }
#endif
  }

  void deletePort() {
    if (local == NULL)
      return;

#ifdef HAVE_SEMAPHORE
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) != -1) {
      ts.tv_sec += 2; // wait max two seconds
      sem_post(&sem_needpost);
      sem_timedwait(&sem_cleanup, &ts);
    }
#endif

    seq->deletePort(local);
    local = NULL;
  }

  void closePort() {
    seq->closePort( local );
    local = NULL;
  }

  operator jack_port_t * () const { return port; }
};
#undef RTMIDI_CLASSNAME



//*********************************************************************//
//  API: JACK
//  Class Definitions: MidiInJack
//*********************************************************************//

#define RTMIDI_CLASSNAME "JackBackendCallbacks"
int JackBackendCallbacks::jackProcessIn( jack_nframes_t nframes, void *arg )
{
  JackMidiData *jData = (JackMidiData *) arg;
  MidiInJack *rtData = jData->rtMidiIn;
  jack_midi_event_t event;
  jack_time_t time;

  // Is port created?
  if ( jData->local == NULL ) return 0;
  void *buff = jack_port_get_buffer( jData->local, nframes );

  // We have midi events in buffer
  int evCount = jack_midi_get_event_count( buff );
  for (int j = 0; j < evCount; j++) {
    MidiInApi::MidiMessage message;
    message.bytes.clear();

    jack_midi_event_get( &event, buff, j );

    for ( unsigned int i = 0; i < event.size; i++ )
      message.bytes.push_back( event.buffer[i] );

    // Compute the delta time.
    time = jack_get_time();
    if ( rtData->firstMessage == true )
      rtData->firstMessage = false;
    else
      message.timeStamp = ( time - jData->lastTime ) * 0.000001;

    jData->lastTime = time;

    if ( !rtData->continueSysex ) {
      if ( rtData->userCallback ) {
	rtData->userCallback->rtmidi_midi_in( message.timeStamp, message.bytes);
      }
      else {
	// As long as we haven't reached our queue size limit, push the message.
	if (!rtData->queue.push(message)) {
	  try {
	    rtData->error(RTMIDI_ERROR(rtmidi_gettext("Error: Message queue limit reached."),
				       Error::WARNING));
	  } catch (Error & e) {
	    // don't bother JACK with an unhandled exception
	  }
	}
      }
    }
  }

  return 0;
}

// Jack process callback
int JackBackendCallbacks::jackProcessOut( jack_nframes_t nframes, void *arg )
{
  JackMidiData *data = (JackMidiData *) arg;
  jack_midi_data_t *midiData;
  int space;

  // Is port created?
  if ( data->local == NULL ) return 0;

  void *buff = jack_port_get_buffer( data->local, nframes );
  if (buff != NULL) {
    jack_midi_clear_buffer( buff );

    while ( jack_ringbuffer_read_space( data->buffSize ) > 0 ) {
      jack_ringbuffer_read( data->buffSize, (char *) &space, (size_t) sizeof(space) );
      midiData = jack_midi_event_reserve( buff, 0, space );

      jack_ringbuffer_read( data->buffMessage, (char *) midiData, (size_t) space );
    }
  }

  data->deletePortIfRequested();
  return 0;
}
#undef RTMIDI_CLASSNAME

#define RTMIDI_CLASSNAME "MidiInJack"
MidiInJack :: MidiInJack( const std::string &clientName, unsigned int queueSizeLimit ) : MidiInApi( queueSizeLimit )
{
  MidiInJack::initialize( clientName );
}

void MidiInJack :: initialize( const std::string &clientName )
{
  JackMidiData *data = new JackMidiData(clientName,this);
  apiData_ = (void *) data;
  this->clientName = clientName;
  try {
    data->init(true);
  } catch (const Error & e) {
    delete data;
    apiData_ = 0;
    throw;
  }
}

#if 0
void MidiInJack :: connect()
{
  abort();
  // this should be unnecessary
  JackMidiData *data = static_cast<JackMidiData *> (apiData_);
  if ( data->local )
    return;

  // Initialize JACK client
  if (( data->local = jack_client_open( clientName.c_str(), JackNoStartServer, NULL )) == 0) {
    error(RTMIDI_ERROR(gettext_noopt("JACK server not running?"),
		       Error::WARNING) );
    return;
  }

  jack_set_process_callback( data->client, jackProcessIn, data );
  jack_activate( data->client );
}
#endif

MidiInJack :: ~MidiInJack()
{
  JackMidiData *data = static_cast<JackMidiData *> (apiData_);
  try {
    MidiInJack::closePort();
  } catch (Error & e) {
    try {
      delete data;
    } catch (...) {
    }
    error(e);
    return;
  }

#if 0
  if ( data->client )
    jack_client_close( data->client );
#endif
  /* immediately shut down the JACK client */
  delete data;
}

void MidiInJack :: openPort( unsigned int portNumber, const std::string &portName )
{
  JackMidiData *data = static_cast<JackMidiData *> (apiData_);

  //		connect();

  // Creating new port
  if ( data->local == NULL)
    data->local = jack_port_register( *(data->seq), portName.c_str(),
				      JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0 );

  if ( data->local == NULL) {
    error(RTMIDI_ERROR(gettext_noopt("Error creating JACK port."),
		       Error::DRIVER_ERROR) );
    return;
  }

  // Connecting to the output
  std::string name = getPortName( portNumber );
  jack_connect( *(data->seq), name.c_str(), jack_port_name( data->local ) );
}

void MidiInJack :: openVirtualPort( const std::string &portName )
{
  JackMidiData *data = static_cast<JackMidiData *> (apiData_);

  //		connect();
  if ( data->local == NULL )
    data->local = jack_port_register( *(data->seq), portName.c_str(),
				      JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0 );

  if ( data->local == NULL ) {
    error(RTMIDI_ERROR(gettext_noopt("Error creating JACK virtual port."),
		       Error::DRIVER_ERROR) );
  }
}

void MidiInJack :: openPort( const PortDescriptor & p,
			     const std::string &portName )
{
  JackMidiData *data = static_cast<JackMidiData *> (apiData_);
  const JackPortDescriptor * port = dynamic_cast<const JackPortDescriptor *>(&p);

  if ( !data ) {
    error(RTMIDI_ERROR(gettext_noopt("Data has not been allocated."),
		       Error::SYSTEM_ERROR) );
    return;
  }
#if 0
  if ( connected_ ) {
    error(RTMIDI_ERROR(gettext_noopt("A valid connection already exists."),
		       Error::WARNING) );
    return;
  }
#endif
  if (!port) {
    error(RTMIDI_ERROR(gettext_noopt("JACK has been instructed to open a non-JACK MIDI port. This doesn't work."),
		       Error::INVALID_DEVICE) );
    return;
  }

  try {
    if (!data->local)
      data->openPort (JackPortIsInput,
		      portName);
    data->setRemote(*port);
    data->connectPorts(*port,data->local);
  } catch (Error & e) {
    error (e);
  }

}

Pointer<PortDescriptor> MidiInJack :: getDescriptor(bool local)
{
  JackMidiData *data = static_cast<JackMidiData *> (apiData_);
  try {
    if (local) {
      if (data && data->local) {
	return Pointer<PortDescriptor>(
				       new JackPortDescriptor(data->local,data->getClientName()));
      }
    } else {
      if (data && *data) {
	return Pointer<PortDescriptor>(
				       new JackPortDescriptor(*data,data->getClientName()));
      }
    }
  } catch (Error & e) {
    error(e);
  }
  return NULL;
}

PortList MidiInJack :: getPortList(int capabilities)
{
  JackMidiData *data = static_cast<JackMidiData *> (apiData_);
  try {
    return JackPortDescriptor::getPortList(capabilities | PortDescriptor::INPUT,
					   data->getClientName());
  } catch (Error &  e) {
    error(e);
  }
  return PortList();
}

unsigned int MidiInJack :: getPortCount()
{
  int count = 0;
  // connect();
  JackMidiData *data = static_cast<JackMidiData *> (apiData_);
  if ( !(*(data->seq)) )
    return 0;

  // List of available ports
  const char **ports = jack_get_ports( *(data->seq), NULL, JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput );

  if ( ports == NULL ) return 0;
  while ( ports[count] != NULL )
    count++;

  free( ports );

  return count;
}

std::string MidiInJack :: getPortName( unsigned int portNumber )
{
  JackMidiData *data = static_cast<JackMidiData *> (apiData_);
  std::string retStr("");

  //		connect();

  // List of available ports
  const char **ports = jack_get_ports(* (data->seq), NULL,
				      JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput );

  // Check port validity
  if ( ports == NULL ) {
    error(RTMIDI_ERROR(gettext_noopt("No ports available."),
		       Error::WARNING) );
    return retStr;
  }

  unsigned int i;
  for (i=0; i<portNumber && ports[i]; i++) {}
  if (i < portNumber || !ports[portNumber]) {
    std::ostringstream ost;
    error(RTMIDI_ERROR1(gettext_noopt("The 'portNumber' argument (%d) is invalid."),
			Error::WARNING, portNumber) );
  }
  else retStr.assign( ports[portNumber] );

  jack_free( ports );
  return retStr;
}

void MidiInJack :: closePort()
{
  JackMidiData *data = static_cast<JackMidiData *> (apiData_);
  if (!data) return;

  data->deletePort();
}

void MidiInJack :: setClientName( const std::string& )
{
  error(RTMIDI_ERROR(gettext_noopt("Setting the client name is not supported by JACK."),
		     Error::WARNING ));
}

void MidiInJack :: setPortName( const std::string &portName )
{
  JackMidiData *data = static_cast<JackMidiData *> (apiData_);
  data->rename(portName);
}
#undef RTMIDI_CLASSNAME

//*********************************************************************//
//  API: JACK
//  Class Definitions: MidiOutJack
//*********************************************************************//


#define RTMIDI_CLASSNAME "MidiOutJack"
MidiOutJack :: MidiOutJack( const std::string &clientName ) : MidiOutApi()
{
  MidiOutJack::initialize( clientName );
}

void MidiOutJack :: initialize( const std::string &clientName )
{
  JackMidiData *data = new JackMidiData(clientName);
  apiData_ = (void *) data;
  this->clientName = clientName;
  // init is the last as it may throw an exception
  try {
    data->init(false);
  } catch (const Error & e) {
    delete data;
    apiData_ = 0;
    throw;
  }
}

void MidiOutJack :: connect()
{
#if 0
  JackMidiData *data = static_cast<JackMidiData *> (apiData_);
  if ( *(data->seq) )
    return;

  // Initialize JACK client
  if (( *(data->seq) = jack_client_open( clientName.c_str(), JackNoStartServer, NULL )) == 0) {
    error(RTMIDI_ERROR(gettext_noopt("JACK server not running?"),
		       Error::WARNING) );
    return;
  }

  jack_set_process_callback( data->client, jackProcessOut, data );
  data->buffSize = jack_ringbuffer_create( JACK_RINGBUFFER_SIZE );
  data->buffMessage = jack_ringbuffer_create( JACK_RINGBUFFER_SIZE );
  jack_activate( data->client );
#endif
}

MidiOutJack :: ~MidiOutJack()
{
  JackMidiData *data = static_cast<JackMidiData *> (apiData_);
  //		closePort();
  data -> request_delete();
}

void MidiOutJack :: openPort( unsigned int portNumber, const std::string &portName )
{
  JackMidiData *data = static_cast<JackMidiData *> (apiData_);

  // connect();

  // Creating new port
  if ( data->local == NULL )
    data->local = jack_port_register( *(data->seq), portName.c_str(),
				      JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput, 0 );

  if ( data->local == NULL ) {
    error(RTMIDI_ERROR(gettext_noopt("Error creating JACK port."),
		       Error::DRIVER_ERROR) );
    return;
  }

  // Connecting to the output
  std::string name = getPortName( portNumber );
  jack_connect( *(data->seq), jack_port_name( data->local ), name.c_str() );
}

void MidiOutJack :: openVirtualPort( const std::string &portName )
{
  JackMidiData *data = static_cast<JackMidiData *> (apiData_);

  // connect();
  if ( data->local == NULL )
    data->local = jack_port_register( *(data->seq), portName.c_str(),
				      JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput, 0 );

  if ( data->local == NULL ) {
    error(RTMIDI_ERROR(gettext_noopt("Error creating JACK virtual port."),
		       Error::DRIVER_ERROR) );
  }
}

void MidiOutJack :: openPort( const PortDescriptor & p,
			      const std::string &portName )
{
  JackMidiData *data = static_cast<JackMidiData *> (apiData_);
  const JackPortDescriptor * port = dynamic_cast<const JackPortDescriptor *>(&p);

  if ( !data ) {
    error(RTMIDI_ERROR(gettext_noopt("Data has not been allocated."),
		       Error::SYSTEM_ERROR) );
    return;
  }
#if 0
  if ( connected_ ) {
    error(RTMIDI_ERROR(gettext_noopt("A valid connection already exists."),
		       Error::WARNING) );
    return;
  }
#endif
  if (!port) {
    error(RTMIDI_ERROR(gettext_noopt("JACK has been instructed to open a non-JACK MIDI port. This doesn't work."),
		       Error::INVALID_DEVICE) );
    return;
  }

  try {
    if (!data->local)
      data->openPort (JackPortIsOutput,
		      portName);
    data->setRemote(*port);
    data->connectPorts(data->local,*port);
  } catch (Error & e) {
    error(e);
  }
}

Pointer<PortDescriptor> MidiOutJack :: getDescriptor(bool local)
{
  JackMidiData *data = static_cast<JackMidiData *> (apiData_);
  try {
    if (local) {
      if (data && data->local) {
	return Pointer<PortDescriptor>(
				       new JackPortDescriptor(data->local,data->getClientName()));
      }
    } else {
      if (data && *data) {
	return Pointer<PortDescriptor>(
				       new JackPortDescriptor(*data,data->getClientName()));
      }
    }
  } catch (Error & e) {
    error(e);
  }
  return NULL;
}

PortList MidiOutJack :: getPortList(int capabilities)
{
  JackMidiData *data = static_cast<JackMidiData *> (apiData_);
  return JackPortDescriptor::getPortList(capabilities | PortDescriptor::OUTPUT,
					 data->getClientName());
}

unsigned int MidiOutJack :: getPortCount()
{
  int count = 0;
  JackMidiData *data = static_cast<JackMidiData *> (apiData_);
  // connect();
  if ( !*(data->seq) )
    return 0;

  // List of available ports
  const char **ports = jack_get_ports(* (data->seq), NULL,
				      JACK_DEFAULT_MIDI_TYPE, JackPortIsInput );

  if ( ports == NULL ) return 0;
  while ( ports[count] != NULL )
    count++;

  free( ports );

  return count;
}

std::string MidiOutJack :: getPortName( unsigned int portNumber )
{
  JackMidiData *data = static_cast<JackMidiData *> (apiData_);
  std::string retStr("");

  // connect();

  // List of available ports
  const char **ports = jack_get_ports(*(data->seq), NULL,
				      JACK_DEFAULT_MIDI_TYPE, JackPortIsInput );

  // Check port validity
  if ( ports == NULL) {
    error(RTMIDI_ERROR(gettext_noopt("No ports available."),
		       Error::WARNING) );
    return retStr;
  }

  if ( ports[portNumber] == NULL) {
    std::ostringstream ost;
    error(RTMIDI_ERROR1(gettext_noopt("The 'portNumber' argument (%d) is invalid."),
			Error::WARNING, portNumber) );
  }
  else retStr.assign( ports[portNumber] );

  free( ports );
  return retStr;
}

void MidiOutJack :: closePort()
{
#if defined(__RTMIDI_DEBUG__)
  std::cerr << "Closing Port" << std::endl;
#endif
  JackMidiData *data = static_cast<JackMidiData *> (apiData_);

  data->delayedDeletePort();
}

void MidiOutJack:: setClientName( const std::string& )
{
  error(RTMIDI_ERROR(gettext_noopt("Setting the client name is not supported by JACK."),
		     Error::WARNING ));
}

void MidiOutJack :: setPortName( const std::string &portName )
{
  JackMidiData *data = static_cast<JackMidiData *> (apiData_);
  data->rename(portName);
}

void MidiOutJack :: sendMessage( const unsigned char *message, size_t size )
{
  int nBytes = size;
  JackMidiData *data = static_cast<JackMidiData *> (apiData_);

  // Write full message to buffer
  jack_ringbuffer_write( data->buffMessage, ( const char * ) message,
                         nBytes );
  jack_ringbuffer_write( data->buffSize, ( char * ) &nBytes, sizeof( nBytes ) );
}
#undef RTMIDI_CLASSNAME
RTMIDI_NAMESPACE_END
#endif  // __UNIX_JACK__

