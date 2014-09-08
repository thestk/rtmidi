/********************* -*- C++ -*- ****************************************/
/*! \class RtMidi
  \brief An abstract base class for realtime MIDI input/output.

  This class implements some common functionality for the realtime
  MIDI input/output subclasses RtMidiIn and RtMidiOut.

  RtMidi WWW site: http://music.mcgill.ca/~gary/rtmidi/

  RtMidi: realtime MIDI i/o C++ classes
  Copyright (c) 2003-2014 Gary P. Scavone

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

/*!
  \file RtMidi.h
*/

#ifndef RTMIDI_H
#define RTMIDI_H

#define RTMIDI_VERSION "3.0.0alpha"

#ifdef __GNUC__
#define RTMIDI_DEPRECATED(func) func __attribute__ ((deprecated))
#elif defined(_MSC_VER)
#define RTMIDI_DEPRECATED(func) __declspec(deprecated) func
#else
#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#define RTMIDI_DEPRECATED(func) func
#endif

#include <exception>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <memory>
#include <stdexcept>
// the following are used in the error constructor
#include <cstdarg>
#include <cstring>
#include <cstdio>

#ifdef RTMIDI_GETTEXT
#include "gettext.h"
#endif
#define gettext_noopt(str) (str)

#define NAMESPACE_RTMIDI_START namespace rtmidi {
#define NAMESPACE_RTMIDI_END   }

NAMESPACE_RTMIDI_START


#ifdef RTMIDI_GETTEXT
const char * rtmidi_gettext(const char * s);
void init_rtmidi_gettext();
#else
#define rtmidi_gettext(arg) (arg)
#endif

//! MIDI API specifier arguments.
enum ApiType {
  UNSPECIFIED,    /*!< Search for a working compiled API. */
  MACOSX_CORE,    /*!< Macintosh OS-X Core Midi API. */
  LINUX_ALSA,     /*!< The Advanced Linux Sound Architecture API. */
  UNIX_JACK,      /*!< The JACK Low-Latency MIDI Server API. */
  WINDOWS_MM,     /*!< The Microsoft Multimedia MIDI API. */
  WINDOWS_KS,     /*!< The Microsoft Kernel Streaming MIDI API. */
  DUMMY,          /*!< A compilable but non-functional API. */
  ALL_API         /*!< Use all available APIs for port selection. */
};

//! Return the name on a MIDI API
inline std::string getApiName(ApiType type) {
  switch (type) {
  case UNSPECIFIED: return rtmidi_gettext("Automatic selection");
  case MACOSX_CORE: return rtmidi_gettext("Core MIDI");
  case LINUX_ALSA:  return rtmidi_gettext("ALSA");
  case UNIX_JACK:   return rtmidi_gettext("JACK");
  case WINDOWS_MM:  return rtmidi_gettext("Windows Multimedia");
  case WINDOWS_KS:  return rtmidi_gettext("DirectX/Kernel Streaming");
  case DUMMY:       return rtmidi_gettext("NULL device");
  case ALL_API:     return rtmidi_gettext("All available MIDI systems");
  }
  return "";
}

//! C style user callback function type definition.
/*!
  This interface type has been replaced by a MidiInterface class.

  \param timeStamp  timestamp indicating when the event has been received
  \param message    a pointer to the binary MIDI message
  \param userData   a pointer that can be set using setUserdata
  \sa MidiIn
  \sa MidiInApi
  \sa MidiInterface
  \deprecated
*/
typedef void (*MidiCallback)( double timeStamp, std::vector<unsigned char> *message, void *userData);

//! C++ style user callback interface.
/*!
  This interface class can be used to implement type safe MIDI callbacks.
  Every time a MIDI message is received  the function \ref MidiInterface::rtmidi_midi_in of the
  currently set callback object is called.
*/
struct MidiInterface {
  //! Virtual destructor to avoid unexpected behaviour.
  virtual ~MidiInterface() {}

  //! The MIDI callback function.
  /*! This function is called whenever a MIDI packet is received by a
    MIDI backend that uses the corresponding callback object.
    \param timestamp the timestamp when the MIDI message has been received
    \param message   the message itself.
  */
  virtual void rtmidi_midi_in(double timestamp, std::vector<unsigned char> *message) = 0;

  //! Delete the object if necessary.
  /*! This function allows the user to delete the Midi callback object,
    when MIDI backend drops its reference to it. By default it does nothing.
    But, callback objects are owned by the MIDI backend. These must be deleted
    after the reference to them has been dropped.

    \sa CompatibilityMidiInterface
  */
  virtual void delete_me() {}
};

//! Compatibility interface to hold a C style callback function
struct CompatibilityMidiInterface: MidiInterface {
  CompatibilityMidiInterface(MidiCallback cb, void * ud):
    callback(cb),
    userData(ud) {}
  void rtmidi_midi_in(double timestamp, std::vector<unsigned char> *message) {
    callback (timestamp, message, userData);
  }
  void delete_me() { delete this; }
  MidiCallback callback;
  void * userData;
};

/************************************************************************/
/*! \class Error
  \brief Exception handling class for RtMidi.

  The Error class is quite simple but it does allow errors to be
  "caught" by Error::Type. See the RtMidi documentation to know
  which methods can throw an Error.
*/
/************************************************************************/

#define RTMIDI_ERROR(message, type)				      \
  rtmidi::Error(message, type,					      \
		RTMIDI_CLASSNAME, __FUNCTION__,			      \
		__FILE__, __LINE__)
#define RTMIDI_ERROR1(message, type, arg1)			      \
  rtmidi::Error(message, type,					      \
		RTMIDI_CLASSNAME, __FUNCTION__,			      \
		__FILE__, __LINE__, arg1)
class Error : public std::exception
{
public:
  //! Defined Error types.
  enum Type {
    WARNING,           /*!< A non-critical error. */
    DEBUG_WARNING,     /*!< A non-critical error which might be useful for debugging. */
    UNSPECIFIED,       /*!< The default, unspecified error type. */
    NO_DEVICES_FOUND,  /*!< No devices found on system. */
    INVALID_DEVICE,    /*!< An invalid device ID was specified. */
    MEMORY_ERROR,      /*!< An error occured during memory allocation. */
    INVALID_PARAMETER, /*!< An invalid parameter was specified to a function. */
    INVALID_USE,       /*!< The function was called incorrectly. */
    DRIVER_ERROR,      /*!< A system driver error occured. */
    SYSTEM_ERROR,      /*!< A system error occured. */
    THREAD_ERROR       /*!< A thread error occured. */
  };

  //! The constructor.
  Error( const char * message,
	 Type type,
	 const char * class_name,
	 const char * function_name,
	 const char * file_name,
	 int line_number, ...) throw();

  //! The destructor.
  virtual ~Error( void ) throw() {}

  //! Prints thrown error message to stderr.
  virtual void printMessage( std::ostream &s = std::cerr ) const throw() {
    s << std::endl
      << file << ":" << line << ": in function "
      << classname << "::" << function << std::endl
      << message_ << std::endl << std::endl;
  }

  //! Returns the thrown error message type.
  virtual const Type& getType(void) const throw() { return type_; }

  //! Returns the thrown error message string.
  virtual const std::string& getMessage(void) const throw() { return message_; }

  //! Returns the thrown error message as a c-style string.
  virtual const char* what( void ) const throw() { return message_.c_str(); }

protected:
  const char * classname;
  const char * function;
  const char * file;
  int line;
  std::string message_;
  Type type_;
};

//! RtMidi error callback function prototype.
/*!
  \param type Type of error.
  \param errorText Error description.

  Note that class behaviour is undefined after a critical error (not
  a warning) is reported.
*/
typedef void (*ErrorCallback)( Error::Type type, const std::string &errorText, void * userdata );
struct ErrorInterface {
  virtual ~ErrorInterface() {}
  virtual void rtmidi_error (Error e) = 0;
  virtual void delete_me() {};
};

struct CompatibilityErrorInterface: ErrorInterface {
  CompatibilityErrorInterface(ErrorCallback cb, void * ud): callback(cb),
							    userdata(ud) {}
  void rtmidi_error(Error e) {
    callback(e.getType(),e.getMessage(),userdata);
  }

  void delete_me() { delete this; }
private:
  ErrorCallback callback;
  void * userdata;
};



#if __cplusplus < 201103L
class PortDescriptor;

template<class T>
class Pointer {
public:
  typedef T datatype;
protected:
  struct countPointer {
    int count;
    datatype * descriptor;
  };
public:
  Pointer():ptr(0) {}
  Pointer(datatype * p):ptr(new countPointer) {
    ptr->count = 1;
    ptr->descriptor = p;
  }
  Pointer(const Pointer<datatype> & other):
    ptr(other.ptr) {
    if (ptr)
      ptr->count++;
  }

  ~Pointer() {
    if (!ptr) return;
    if (!ptr->descriptor) {
      delete ptr;
      return;
    }
    if (!(--ptr->count)) {
      delete ptr->descriptor;
      delete ptr;
    }
  }

  datatype * operator -> () {
    if (!ptr) return 0;
    // this should throw an exception

    return ptr->descriptor;
  }

  datatype & operator * () {
    if (!ptr || !ptr->descriptor) {
      throw std::invalid_argument("rtmidi::Pointer: trying to dereference a NULL pointer.");
    }
    else return (*ptr->descriptor);
  }

  bool operator ! () {
    return (!ptr || !ptr->descriptor);
  }

  Pointer & operator = (const Pointer<datatype> & other) {
    if (ptr) {
      if (!(--ptr->count)) {
	delete ptr->descriptor;
	delete ptr;
      }
    }
    ptr = other.ptr;
    ptr->count++;
    return *this;
  }
protected:
  countPointer * ptr;
};
#else
template<class T>
typedef std::shared_ptr<T> Pointer;
#endif

class MidiApi;
class MidiInApi;
class MidiOutApi;

class PortDescriptor {
public:
  //! Flags for formatting a string description of the port.
  /*! These flags just mark the requirements that the string
    should fulfil. An API may return the same string for
    different requirements e.g. the same short and long
    name. */
  enum NamingType {
    SHORT_NAME =0,  /*!< A short human readable name
		      depending on the API
		      e.g. “Ensoniq AudioPCI” */
    LONG_NAME,      /*!< A complete human readable
		      name depending on the API
		      e.g. "Ensoniq AudioPCI: ES1371" */
    SESSION_PATH,   /*!< A unique description that can
		      be used to identify the port
		      during runtime. It may be a
		      cryptic string. */
    STORAGE_PATH,   /*!< A unique description that is
		      optimised for storage in
		      configuration files. This is a
		      more textual representation that
		      is more robust to small changes in
		      the surrounding environment than
		      \ref SESSION_PATH */
    NAMING_MASK = 0x0F, /*!< part of the flags
			  that is concerned with
			  naming.
			*/
    UNIQUE_NAME = 0x10, /*!< Make all names uniqe. This
			  is usually done by adding
			  numbers to the end of the
			  string \note: use #undef UNIQUE_NAME
			  on windows in case of any errors */
    INCLUDE_API = 0x20 /*!< Add a string describing the
			 API at the beginning of the
			 string. */
  };

  //! Flags describing the capabilities of a given port.
  enum PortCapabilities {
    INPUT     = 1,    /*!< Ports that can be read from. */
    OUTPUT    = 2,    /*!< Ports that can be written to. */
    INOUTPUT  = 3,    /*!< Ports that allow reading and writing (INPUT | OUTPUT) */
    UNLIMITED = 0x10  /*!< Some APIs can filter out certain ports which they consider
			not to be useful. This flags supresses this behaviour and
			selects all ports that are useable. */
  };

  //! Default constructor.
  /*!
   * Derived classes should have a constructor.
   */
  PortDescriptor() {};

  //! A virtual destructor
  /*! As we might have to destruct the object from the application code
   *  each port id must have a virtual destructor.
   */
  virtual ~PortDescriptor() {};

  //! Get the MIDI input api for the current port.
  /*! This is the only information RtMidi needs to know: Which
   *  API should handle this object. This can be used to get
   *  an API which can send data to the given port.
   *
   * \param queueSizeLimit The limit of the midi queue. This parameter is handled by
   * the constructor of the backend API.
   *
   * \return API that can use this object to connect to an input port or 0
   * if no input API can be created.
   */
  virtual MidiInApi * getInputApi(unsigned int queueSizeLimit = 100) const = 0;

  //! Get the MIDI output api for the current port.
  /*! This is the only information RtMidi needs to know: Which
   *  API should handle this object. This can be used to get
   *  an API which can receive data from the given port.
   *
   * \return API that can use this object to connect to an output port.
   */
  virtual MidiOutApi * getOutputApi() const = 0;

  //! Return the port name
  /*!
   * \param flags A description of the requirements of the returned name.
   * \return A name that is formatted according to \ref flags.
   * \sa NamingTypes
   */
  virtual std::string getName(int flags = SHORT_NAME | UNIQUE_NAME) = 0;

  //! Get capabilities
  /*! \return a capabilities flag describing the capabilities of the port.
   *  \sa PortCapabilities
   */
  virtual int getCapabilities() const = 0;
};

//! A list of port descriptors.
/*! Port descriptors are stored as shared pointers. This avoids
  unnecessary duplication of the data structure and handles automatic
  deletion if all references have been removed. */
typedef std::list<Pointer<PortDescriptor> > PortList;



// **************************************************************** //
//
// MidiInApi / MidiOutApi class declarations.
//
// Subclasses of MidiInApi and MidiOutApi contain all API- and
// OS-specific code necessary to fully implement the RtMidi API.
//
// Note that MidiInApi and MidiOutApi are abstract base classes and
// cannot be explicitly instantiated.  MidiIn and MidiOut will
// create instances of a MidiInApi or MidiOutApi subclass.
//
// **************************************************************** //

#define RTMIDI_CLASSNAME "MidiApi"
class MidiApi
{
public:

  MidiApi();
  virtual ~MidiApi();

  //! Return whether the API supports virtual ports
  /*!
    \retval true The funcion returns true if the API supports virtual ports.
    \retval false The funcion returns false if the API doesn't support virtual ports.
    \sa openVirtualPort
  */
  virtual bool hasVirtualPorts() const = 0;

  //! Pure virtal function to create a virtual port, with optional name.
  /*!
    This function creates a virtual MIDI port to which other
    software applications can connect. This type of functionality
    is currently only supported by the Macintosh OS-X, any JACK,
    and Linux ALSA APIs (the function returns an error for the other APIs).

    \param portName An optional name for the applicaction port that is
    used to connect to portId can be specified.

    \sa hasVirtualPorts
  */
  virtual void openVirtualPort( const std::string portName = std::string( "RtMidi virtual port" ) ) = 0;

  //! Pure virtual function to open a MIDI connection given by enumeration number.
  /*! \param portNumber An optional port number greater than 0
    can be specified.  Otherwise, the default or first port
    found is opened.

    \param portName An optional name for the applicaction port
    that will be generated to connect to portId can be
    specified.
  */
  virtual void openPort( unsigned int portNumber = 0, const std::string & portName = std::string( "RtMidi" ) ) = 0;

  //! Pure virtual function to open a MIDI connection given by a port descriptor.
  /*!
    \param port     A port descriptor of the port must be specified.
    \param portName An optional name for the applicaction port that is used to connect to portId can be specified.
  */
  virtual void openPort( const PortDescriptor & port, const std::string & portName = std::string( "RtMidi" ) ) = 0;

  //!  Open a MIDI connection given by a port descriptor pointer.
  /*!
    \param port     A pointer to a port descriptor of the port must be specified.
    \param portName An optional name for the applicaction port that is used to connect to portId can be specified.
    \sa openPort(const PortDescriptor &,const std::string &);
  */
  void openPort( Pointer<PortDescriptor> p, const std::string & portName = std::string( "RtMidi" ) ) {
    if (!p) {
      error(RTMIDI_ERROR( gettext_noopt("Passed NULL pointer."),
			  Error::INVALID_PARAMETER));
      return;
    }
    openPort(*p, portName);
  }

  //! Pure virtual function to return a port descriptor if the port is open
  /*! This function returns a port descriptor that can be used to open another port
    either to the connected port or – if the backend supports it – the connecting port.
    \param local The parameter local defines whether the function returns a descriptor to
    the virtual port (true) or the remote port (false). The function returns 0 if the
    port cannot be determined (e.g. if the port is not connected or the backend dosen't support it).
  */
  virtual Pointer<PortDescriptor> getDescriptor(bool local=false) = 0;

  //! Pure virtual function to return a list of all available ports of the current API.
  /*!
    \param capabilities an optional parameter that describes which
    capabilities the device typu

    \return This function returns a list of port descriptors.

    \note An input API may but need not necessarily report
    output devices which cannot be used as input if
    \ref 0 is passed as \ref capabilities parameter.
    \sa PortDescriptor::PortCapabilitiers
  */
  virtual PortList getPortList(int capabilities = 0) = 0;

  //! Pure virtual to return the number of available MIDI ports of the current API.
  /*!
    \return This function returns the number of MIDI ports of
    the selected API.

    \note Only ports are counted that can be used with the
    current API so an input API does ignore all output devices
    and vice versa.

    \sa getPortName
  */
  virtual unsigned int getPortCount() = 0;

  //! Pure virtual function to return a string identifier for the specified MIDI port number.
  /*!
    \param portNumber Number of the device to be referred to.
    \return The name of the port with the given Id is returned.
    \retval An empty string is returned if an invalid port specifier is provided.

    \note Only ports are counted that can be used with the
    current API so an input API does ignore all output devices
    and vice versa.

    \sa getPortCount()
  */
  virtual std::string getPortName( unsigned int portNumber = 0 ) = 0;

  //! Pure virtual function to close an open MIDI connection (if one exists).
  virtual void closePort( void ) = 0;

  // ! A basic error reporting function for RtMidi classes.
  //  static void error( Error::Type type, std::string &errorString );

  //! Pure virtual function to return whether a port is open or not.
  /*! \retval true if a port is open and
    \retval false if the port is not open (e.g. not opend or closed).
  */
  bool isPortOpen() const { return connected_; }

  //! Virtual function to set an error callback function to be invoked when an error has occured.
  /*!
    The callback function will be called whenever an error has occured. It is best
    to set the error callback function before opening a port.
  */
  RTMIDI_DEPRECATED(virtual void setErrorCallback( ErrorCallback errorCallback = NULL, void * userData = 0 ));

  //! Virtual function to set the error callback object
  /*!
    Everytime an error is detected or a warning is issued the function
    \r ErrorInterface::rtmidi_error of the callback object is called with an error object
    that describes the situation.

    \param callback An objact that provides an ErrorInterface.
  */
  virtual void setErrorCallback(ErrorInterface * callback);


  //! Returns the MIDI API specifier for the current instance of RtMidiIn.
  virtual ApiType getCurrentApi( void ) throw() = 0;

  //! A basic error reporting function for RtMidi classes.
  /*!  This function hanles errors end warnings that
    occur during runtime of RtMidi. If an error callback
    has been set the function calls the callback and
    returns quietly assuming the callback handled the
    case correctly.

    Otherwise it depends on the type of the error. If it
    is a warning, a message is displayed to
    std::cerr. If it is an error the object is thrown as
    an exception.

    \param e Error/Warning object describing the current
    situation.

    \throw Error
  */
  void error( Error e );

protected:
  virtual void initialize( const std::string& clientName ) = 0;

  void *apiData_;
  bool connected_;
  std::string errorString_;
  ErrorInterface * errorCallback_;
};
#undef RTMIDI_CLASSNAME

#define RTMIDI_CLASSNAME "MidiInApi"
class MidiInApi : public MidiApi
{
public:

  MidiInApi( unsigned int queueSizeLimit );
  virtual ~MidiInApi( void );
  RTMIDI_DEPRECATED(void setCallback( MidiCallback callback, void *userData = 0 ));
  void setCallback( MidiInterface * callback);
  void cancelCallback( void );
  virtual void ignoreTypes( bool midiSysex, bool midiTime, bool midiSense );
  RTMIDI_DEPRECATED(double getMessage( std::vector<unsigned char> *message ))
  {
    if (!message) {
      error( RTMIDI_ERROR(gettext_noopt("Passed NULL pointer."),
			  Error::WARNING ));
    }
    return getMessage(*message);
  }
  double getMessage( std::vector<unsigned char> &message );

  // A MIDI structure used internally by the class to store incoming
  // messages.  Each message represents one and only one MIDI message.
  struct MidiMessage {
    std::vector<unsigned char> bytes;
    double timeStamp;

    // Default constructor.
    MidiMessage()
      :bytes(0), timeStamp(0.0) {}
  };

  struct MidiQueue {
    unsigned int front;
    unsigned int back;
    unsigned int size;
    unsigned int ringSize;
    MidiMessage *ring;

    // Default constructor.
    MidiQueue()
      :front(0), back(0), size(0), ringSize(0) {}
  };

protected:
  // The RtMidiInData structure is used to pass private class data to
  // the MIDI input handling function or thread.
  MidiQueue queue;
  MidiMessage message;
  unsigned char ignoreFlags;
  bool doInput;
  bool firstMessage;
  MidiInterface * userCallback;
  bool continueSysex;
  friend struct JackBackendCallbacks;
};
#undef RTMIDI_CLASSNAME

#define RTMIDI_CLASSNAME "MidiOutApi"
class MidiOutApi : public MidiApi
{
public:

  MidiOutApi( void );
  virtual ~MidiOutApi( void );
  RTMIDI_DEPRECATED(void sendMessage( std::vector<unsigned char> *message ))
  {
    if (!message) {
      error( RTMIDI_ERROR(gettext_noopt("No data in message argument."),
			  Error::WARNING));
    }
    sendMessage(*message);
  }
  virtual void sendMessage( std::vector<unsigned char> &message ) = 0;
};
#undef RTMIDI_CLASSNAME

typedef Pointer<MidiApi> MidiApiPtr;
typedef std::list <MidiApiPtr> MidiApiList;

/*! \class Midi
  \brief A global class that implements basic backend API handling.

  This class enhances \ref MidiApi by some functionality to handle
  backend API objects. It serves as base class for the public RtMidi
  API.

  by Gary P. Scavone, 2003-2014.
*/
/**********************************************************************/
#define RTMIDI_CLASSNAME "Midi"
class Midi {
public:
  typedef rtmidi::ApiType Api;

  //! A static function to determine the current RtMidi version.
  static std::string getVersion( void ) throw();

  //! A static function to determine the available compiled MIDI APIs.
  /*!
    The values returned in the std::vector can be compared against
    the enumerated list values.  Note that there can be more than one
    API compiled for certain operating systems.

    \param apis A vector apis must be provided for the
    return value. All data in this vector will be
    deleted prior to filling in the API data.

    \param preferSystem An opitonal boolean parameter
    may be provided that tells wheter system or software
    APIs shall be prefered. Passing \c true will prefer OS provided APIs
  */
  static void getCompiledApi( std::vector<ApiType> &apis, bool preferSystem = true ) throw();

  //! A static function to determine the available compiled MIDI APIs.
  /*!
    The values returned in the std::vector can be compared against
    the enumerated list values.  Note that there can be more than one
    API compiled for certain operating systems.
  */
  static std::vector<ApiType> getCompiledApi(  ) throw() {
    std::vector<ApiType> apis;
    getCompiledApi(apis);
    return apis;
  }

  //! Returns the MIDI API specifier for the current instance of RtMidiIn.
  ApiType getCurrentApi( void ) throw()
  {
    if (rtapi_) return rtapi_->getCurrentApi();
    else return rtmidi::UNSPECIFIED;
  }

  //! Compatibilty function for older code
  virtual
  RTMIDI_DEPRECATED(void openVirtualPort( const std::string portName = std::string( "RtMidi virtual port" ) )) = 0;

  //! Pure virtual function to open a MIDI connection given by enumeration number.
  /*! \param portNumber An optional port number greater than 0
    can be specified.  Otherwise, the default or first port
    found is opened.

    \param portName An optional name for the applicaction port
    that will be generated to connect to portId can be
    specified.

    \deprecated
  */
  RTMIDI_DEPRECATED(void openPort( unsigned int portNumber = 0,
				   const std::string portName = std::string( "RtMidi" ) ))
  {
    if (rtapi_) rtapi_->openPort(portNumber,portName);
  }


  //! Pure virtual function to return a port descirptor if the port is open
  Pointer<PortDescriptor> getDescriptor(bool local=false)
  {
    if (rtapi_) return rtapi_->getDescriptor(local);
    return 0;
  }

  //! Return a list of all available ports of the current API.
  /*!
    \param capabilities an optional parameter that
    describes which capabilities the returned devices
    must support. The returned devices may have
    additional capabilities to those which have been
    requested, but not less.

    \return This function returns a list of port descriptors.

    \note Each API will request additonal
    capabilites. An output API will set always add \ref
    PortDescriptor::OUTPUT to the mask while an input
    device will always add \ref PortDescriptor::OUTPUT.

    \note An input API may but need not necessarily
    report output devices which cannot be used as input
    if \ref PortDescriptor::OUTPUT is passed as \ref
    capabilities parameter.
  */
  PortList getPortList(int capabilities = 0)
  {
    if (rtapi_) return rtapi_->getPortList(capabilities);
    if (list && !list->empty()) {
      PortList retval;
      for (MidiApiList::iterator i = list->begin();
	   i != list->end();
	   ++i) {
	PortList tmp = (*i)->getPortList(capabilities);
	retval.splice(retval.end(), tmp);
      }
      return retval;
    }
    return PortList();
  }

  //! Pure virtual to return the number of available MIDI ports of the current API.
  /*!
    \return This function returns the number of MIDI ports of
    the selected API.

    \note Only ports are counted that can be used with the
    current API so an input API does ignore all output devices
    and vice versa.

    \sa getPortName
    \deprecated
  */
  RTMIDI_DEPRECATED(unsigned int getPortCount())
  {
    if (rtapi_) return rtapi_->getPortCount();
    return 0;
  }

  //! Pure virtual function to return a string identifier for the specified MIDI port number.
  /*!
    \param portNumber Number of the device to be referred to.
    \return The name of the port with the given Id is returned.
    \retval An empty string is returned if an invalid port specifier is provided.

    \note Only ports are counted that can be used with the
    current API so an input API does ignore all output devices
    and vice versa.

    \sa getPortCount()
    \deprecated
  */
  RTMIDI_DEPRECATED(std::string getPortName( unsigned int portNumber = 0 ))
  {
    if (rtapi_) return rtapi_->getPortName(portNumber);
    return "";
  }

  //! Close an open MIDI connection (if one exists).
  void closePort( void )
  {
    if (rtapi_) rtapi_->closePort();
  }

  // ! A basic error reporting function for RtMidi classes.
  //  static void error( Error::Type type, std::string &errorString );

  //! Pure virtual function to return whether a port is open or not.
  /*! \retval true if a port is open and
    \retval false if the port is not open (e.g. not opend or closed).
  */
  bool isPortOpen( void ) const
  {
    if (rtapi_) return rtapi_->isPortOpen();
    return false;
  }

  //! Pure virtual function to set an error callback function to be invoked when an error has occured.
  /*!
    The callback function will be called whenever an error has occured. It is best
    to set the error callback function before opening a port.
  */
  RTMIDI_DEPRECATED(void setErrorCallback( ErrorCallback errorCallback = NULL, void * userData = 0 ))
  {
    if (rtapi_) rtapi_->setErrorCallback(errorCallback, userData);
  }

  //! Pure virtual function to set an error callback function to be invoked when an error has occured.
  /*!
    The callback function will be called whenever an error has occured. It is best
    to set the error callback function before opening a port.
  */
  void setErrorCallback( ErrorInterface * callback)
  {
    if (rtapi_) rtapi_->setErrorCallback(callback);
  }

  //! A basic error reporting function for RtMidi classes.
  void error( Error e );
protected:
  MidiApi *rtapi_;
  MidiApiList * list;
  bool preferSystem;
  std::string clientName;

  Midi(MidiApiList * l,
       bool pfsystem,
       const std::string & name):rtapi_(0),
				 list(l),
				 preferSystem(pfsystem),
				 clientName(name) {}
  virtual ~Midi()
  {
    if (rtapi_) {
      delete rtapi_;
      rtapi_ = 0;
    }
  }
};
#undef RTMIDI_CLASSNAME

/**********************************************************************/
/*! \class MidiIn
  \brief A realtime MIDI input class.

  This class provides a common, platform-independent API for
  realtime MIDI input.  It allows access to a single MIDI input
  port.  Incoming MIDI messages are either saved to a queue for
  retrieval using the getMessage() function or immediately passed to
  a user-specified callback function.  Create multiple instances of
  this class to connect to more than one MIDI device at the same
  time.  With the OS-X and Linux ALSA MIDI APIs, it is also possible
  to open a virtual input port to which other MIDI software clients
  can connect.

  by Gary P. Scavone, 2003-2014.
*/
/**********************************************************************/

// **************************************************************** //
//
// MidiIn and MidiOut class declarations.
//
// MidiIn / MidiOut are "controllers" used to select an available
// MIDI input or output interface.  They present common APIs for the
// user to call but all functionality is implemented by the classes
// MidiInApi, MidiOutApi and their subclasses.  MidiIn and MidiOut
// each create an instance of a MidiInApi or MidiOutApi subclass based
// on the user's API choice.  If no choice is made, they attempt to
// make a "logical" API selection.
//
// **************************************************************** //

#define RTMIDI_CLASSNAME "MidiIn"
class MidiIn : public Midi
{
public:


  //! Default constructor that allows an optional api, client name and queue size.
  /*!
    An exception will be thrown if a MIDI system initialization
    error occurs.  The queue size defines the maximum number of
    messages that can be held in the MIDI queue (when not using a
    callback function).  If the queue size limit is reached,
    incoming messages will be ignored.

    If no API argument is specified and multiple API support has been
    compiled, the default order of use is JACK, ALSA (Linux) and CORE,
    JACK (OS-X).

    \param api        An optional API id can be specified.
    \param clientName An optional Client name can be specified. This
    will be used to group the ports that are created
    by the application.
    \param queueSizeLimit An optional size of the MIDI input queue can be specified.

    \param pfsystem An optional boolean parameter can be
    provided to indicate the API preferences of the user
    code. If RtMidi is requested to autoselect a backend
    this parameter tells which backend should be tried
    first. If it is \c true the backend will prefer OS
    provieded APIs (WinMM, ALSA, Core MIDI) over other
    APIs (JACK).  If \c false, the order will be vice
    versa.
  */
  MidiIn( ApiType api=rtmidi::UNSPECIFIED,
	  const std::string clientName = std::string( "RtMidi Input Client"),
	  unsigned int queueSizeLimit = 100,
	  bool pfsystem = true);

  //! If a MIDI connection is still open, it will be closed by the destructor.
  ~MidiIn ( void ) throw();

  using Midi::openPort;

  //! Open a MIDI connection given by a port descriptor.
  /*!
    \param port     A port descriptor of the port must be specified.
    \param portName An optional name for the applicaction port that is used to connect to portId can be specified.
  */
  void openPort( const PortDescriptor & port,
		 const std::string & portName = std::string( "RtMidi" ) )
  {
    if (!rtapi_) rtapi_ = port.getInputApi();
    if (rtapi_) rtapi_->openPort(port,portName);
  }


  //!  Open a MIDI connection given by a port descriptor pointer.
  /*!
    \param port     A pointer to a port descriptor of the port must be specified.
    \param portName An optional name for the applicaction port that is used to connect to portId can be specified.
    \sa openPort(const PortDescriptor &,const std::string &);
  */
  void openPort( Pointer<PortDescriptor> p,
		 const std::string & portName = std::string( "RtMidi" ) ) {
    if (!p) {
      error(RTMIDI_ERROR(gettext_noopt("A NULL pointer has been passed as port descriptor"),
			 Error::INVALID_PARAMETER));
      return;
    }
    openPort(*p, portName);
  }

  //! Function to create a virtual port, with optional name.
  /*!
    This function creates a virtual MIDI port to which other
    software applications can connect. This type of functionality
    is currently only supported by the Macintosh OS-X, any JACK,
    and Linux ALSA APIs (the function returns an error for the other APIs).

    \param portName An optional name for the application port that is
    used to connect to portId can be specified.
  */
  void openVirtualPort( const std::string portName = std::string( "RtMidi virtual input port" ) )
  {
    if (!rtapi_ && list && !list->empty()) {
      Pointer<MidiApi> api = list->front();

      std::vector< ApiType > apis;
      getCompiledApi( apis );
      for (size_t i = 0 ; i < apis.size() ; i++) {
	openMidiApi( apis[0] );
	if (rtapi_ && rtapi_->hasVirtualPorts()) break;
      }
    }

    if (rtapi_) rtapi_->openVirtualPort(portName);
    else {
      error(RTMIDI_ERROR(gettext_noopt("No valid MIDI system has been selected."),
			 Error::INVALID_DEVICE));
    }
  }


  //! Pure virtual function to return a port descirptor if the port is open
  Pointer<PortDescriptor> getDescriptor(bool local=false)
  {
    if (rtapi_) return rtapi_->getDescriptor(local);
    return 0;
  }

  //! Set a callback function to be invoked for incoming MIDI messages.
  /*!
    The callback function will be called whenever an incoming MIDI
    message is received.  While not absolutely necessary, it is best
    to set the callback function before opening a MIDI port to avoid
    leaving some messages in the queue.

    \param callback A callback function must be given.
    \param userData Opitionally, a pointer to additional data can be
    passed to the callback function whenever it is called.
  */
  RTMIDI_DEPRECATED(void setCallback( MidiCallback callback, void *userData = 0 ))
  {
    if (rtapi_)
      static_cast<MidiInApi*>(rtapi_)->setCallback(callback,userData);
  }

  //! Set a callback function to be invoked for incoming MIDI messages.
  /*!
    The callback function will be called whenever an incoming MIDI
    message is received.  While not absolutely necessary, it is best
    to set the callback function before opening a MIDI port to avoid
    leaving some messages in the queue.

    \param callback A callback function must be given.
    \param userData Opitionally, a pointer to additional data can be
    passed to the callback function whenever it is called.
  */
  void setCallback( MidiInterface * callback )
  {
    if (rtapi_)
      static_cast<MidiInApi*>(rtapi_)->setCallback(callback);
  }

  //! Cancel use of the current callback function (if one exists).
  /*!
    Subsequent incoming MIDI messages will be written to the queue
    and can be retrieved with the \e getMessage function.
  */
  void cancelCallback()
  {
    if (rtapi_)
      static_cast<MidiInApi*>(rtapi_)->cancelCallback();
  }

  //! Specify whether certain MIDI message types should be queued or ignored during input.
  /*!
    By default, MIDI timing and active sensing messages are ignored
    during message input because of their relative high data rates.
    MIDI sysex messages are ignored by default as well.  Variable
    values of "true" imply that the respective message type will be
    ignored.
  */
  void ignoreTypes( bool midiSysex = true,
		    bool midiTime = true,
		    bool midiSense = true )
  {
    if (rtapi_)
      static_cast<MidiInApi*>(rtapi_)->ignoreTypes(midiSysex, midiTime, midiSense);
  }

  //! Fill the user-provided vector with the data bytes for the next available MIDI message in the input queue and return the event delta-time in seconds.
  /*!
    This function returns immediately whether a new message is
    available or not.  A valid message is indicated by a non-zero
    vector size.  An exception is thrown if an error occurs during
    message retrieval or an input connection was not previously
    established.
  */
  double getMessage( std::vector<unsigned char> &message )
  {
    if (rtapi_)
      return static_cast<MidiInApi*>(rtapi_)->getMessage(message);
    error( RTMIDI_ERROR(gettext_noopt("Could not find any valid MIDI system."),
			Error::WARNING));
    return 0.0;
  }

  //! Fill the user-provided vector with the data bytes for the next available MIDI message in the input queue and return the event delta-time in seconds.
  /*!
    This function returns immediately whether a new message is
    available or not.  A valid message is indicated by a non-zero
    vector size.  An exception is thrown if an error occurs during
    message retrieval or an input connection was not previously
    established.

    \deprecated
  */
  RTMIDI_DEPRECATED(double getMessage( std::vector<unsigned char> *message ))
  {
    if (!message) {
      error( RTMIDI_ERROR(gettext_noopt("Passed NULL pointer."),
			  Error::WARNING));
    }
    if (rtapi_)
      return static_cast<MidiInApi*>(rtapi_)->getMessage(*message);
    error( RTMIDI_ERROR(gettext_noopt("No valid MIDI system has been found."),
			Error::WARNING));
    return 0.0;
  }

protected:
  static MidiApiList queryApis;
  int queueSizeLimit;
  void openMidiApi( ApiType api );

};
#undef RTMIDI_CLASSNAME

/**********************************************************************/
/*! \class MidiOut
  \brief A realtime MIDI output class.

  This class provides a common, platform-independent API for MIDI
  output.  It allows one to probe available MIDI output ports, to
  connect to one such port, and to send MIDI bytes immediately over
  the connection.  Create multiple instances of this class to
  connect to more than one MIDI device at the same time.  With the
  OS-X and Linux ALSA MIDI APIs, it is also possible to open a
  virtual port to which other MIDI software clients can connect.

  by Gary P. Scavone, 2003-2014.
*/
/**********************************************************************/

#define RTMIDI_CLASSNAME "MidiOut"
class MidiOut : public Midi
{
public:

  //! Default constructor that allows an optional client name.
  /*!
    An exception will be thrown if a MIDI system initialization error occurs.

    If no API argument is specified and multiple API support has been
    compiled, the default order of use is JACK, ALSA (Linux) and CORE,
    JACK (OS-X).

    \param api        An optional API id can be specified.
    \param clientName An optional Client name can be specified. This
    will be used to group the ports that are created
    by the application.
    \param queueSizeLimit An optional size of the MIDI input queue can be specified.

    \param pfsystem An optional boolean parameter can be
    provided to indicate the API preferences of the user
    code. If RtMidi is requested to autoselect a backend
    this parameter tells which backend should be tried
    first. If it is \c true the backend will prefer OS
    provieded APIs (WinMM, ALSA, Core MIDI) over other
    APIs (JACK).  If \c false, the order will be vice
    versa.
  */
  MidiOut( ApiType api=rtmidi::UNSPECIFIED,
	   const std::string clientName = std::string( "RtMidi Output Client"),
	   bool pfsystem = true);

  //! The destructor closes any open MIDI connections.
  ~MidiOut( void ) throw();

  using Midi::openPort;

  //! Open a MIDI connection given by a port descriptor.
  /*!
    \param port     A port descriptor of the port must be specified.
    \param portName An optional name for the applicaction port that is used to connect to portId can be specified.
  */
  void openPort( const PortDescriptor & port,
		 const std::string & portName = std::string( "RtMidi" ) )
  {
    if (!rtapi_) rtapi_ = port.getOutputApi();
    if (rtapi_) rtapi_->openPort(port,portName);
  }

  //!  Open a MIDI connection given by a port descriptor pointer.
  /*!
    \param port     A pointer to a port descriptor of the port must be specified.
    \param portName An optional name for the applicaction port that is used to connect to portId can be specified.
    \sa openPort(const PortDescriptor &,const std::string &);
  */
  void openPort( Pointer<PortDescriptor> p,
		 const std::string & portName = std::string( "RtMidi" ) ) {
    if (!p) {
      error(RTMIDI_ERROR(gettext_noopt("Passed NULL pointer."),
			 Error::INVALID_PARAMETER));
      return;
    }
    openPort(*p, portName);
  }


  //! Function to create a virtual port, with optional name.
  /*!
    This function creates a virtual MIDI port to which other
    software applications can connect. This type of functionality
    is currently only supported by the Macintosh OS-X, any JACK,
    and Linux ALSA APIs (the function returns an error for the other APIs).

    \param portName An optional name for the applicaction port that is
    used to connect to portId can be specified.
  */
  void openVirtualPort( const std::string portName = std::string( "RtMidi virtual output port" ) )
  {
    if (!rtapi_ && list && !list->empty()) {
      Pointer<MidiApi> api = list->front();

      std::vector< ApiType > apis;
      getCompiledApi( apis );
      for (size_t i = 0 ; i < apis.size() ; i++) {
	openMidiApi( apis[0] );
	if (rtapi_ && rtapi_->hasVirtualPorts()) break;
      }
    }

    if (rtapi_) rtapi_->openVirtualPort(portName);
    else {
      error(RTMIDI_ERROR(gettext_noopt("No valid MIDI system has been selected."),
			 Error::INVALID_DEVICE));
    }
  }



  //! Immediately send a single message out an open MIDI output port.
  /*!
    An exception is thrown if an error occurs during output or an
    output connection was not previously established.

    \deprecated
  */
  RTMIDI_DEPRECATED(void sendMessage( std::vector<unsigned char> *message ))
  {
    if (!message) {
      error( RTMIDI_ERROR(gettext_noopt("No data in MIDI message."),
			  Error::WARNING));
    }
    if (rtapi_)
      static_cast<MidiOutApi *>(rtapi_)->sendMessage(*message);
    else
      error( RTMIDI_ERROR(gettext_noopt("No valid MIDI system has been selected."),
			  Error::WARNING));
  }


  //! Immediately send a single message out an open MIDI output port.
  /*!
    An exception is thrown if an error occurs during output or an
    output connection was not previously established.
  */
  void sendMessage( std::vector<unsigned char> &message ) {
    if (rtapi_)
      static_cast<MidiOutApi *>(rtapi_)->sendMessage(message);
    error( RTMIDI_ERROR(gettext_noopt("No valid MIDI system has been selected."),
			Error::WARNING));
  }
protected:
  static MidiApiList queryApis;
  void openMidiApi( ApiType api );
};
#undef RTMIDI_CLASSNAME


// **************************************************************** //
//
// MidiInApi and MidiOutApi subclass prototypes.
//
// **************************************************************** //


#if !defined(__LINUX_ALSA__) && !defined(__UNIX_JACK__) && !defined(__MACOSX_CORE__) && !defined(__WINDOWS_MM__)
#define __RTMIDI_DUMMY__
#endif

#if defined(__MACOSX_CORE__)

class MidiInCore: public MidiInApi
{
public:
  MidiInCore( const std::string clientName, unsigned int queueSizeLimit );
  ~MidiInCore( void );
  ApiType getCurrentApi( void ) throw() { return MACOSX_CORE; };
  bool hasVirtualPorts() const { return true; }
  void openPort( unsigned int portNumber, const std::string & portName );
  void openVirtualPort( const std::string portName );
  void openPort( const PortDescriptor & port, const std::string & portName);
  Pointer<PortDescriptor> getDescriptor(bool local=false);
  PortList getPortList(int capabilities);
  void closePort( void );
  unsigned int getPortCount( void );
  std::string getPortName( unsigned int portNumber );

protected:
  void initialize( const std::string& clientName );
};

class MidiOutCore: public MidiOutApi
{
public:
  MidiOutCore( const std::string clientName );
  ~MidiOutCore( void );
  ApiType getCurrentApi( void ) throw() { return MACOSX_CORE; };
  bool hasVirtualPorts() const { return true; }
  void openPort( unsigned int portNumber, const std::string & portName );
  void openVirtualPort( const std::string portName );
  void openPort( const PortDescriptor & port, const std::string & portName);
  Pointer<PortDescriptor> getDescriptor(bool local=false);
  PortList getPortList(int capabilities);
  void closePort( void );
  unsigned int getPortCount( void );
  std::string getPortName( unsigned int portNumber );
  void sendMessage( std::vector<unsigned char> &message );

protected:
  void initialize( const std::string& clientName );
};

#endif

#if defined(__UNIX_JACK__)

class MidiInJack: public MidiInApi
{
public:
  MidiInJack( const std::string clientName, unsigned int queueSizeLimit );
  ~MidiInJack( void );
  ApiType getCurrentApi( void ) throw() { return UNIX_JACK; };
  bool hasVirtualPorts() const { return true; }
  void openPort( unsigned int portNumber, const std::string & portName );
  void openVirtualPort( const std::string portName );
  void openPort( const PortDescriptor & port, const std::string & portName);
  Pointer<PortDescriptor> getDescriptor(bool local=false);
  PortList getPortList(int capabilities);
  void closePort( void );
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
  MidiOutJack( const std::string clientName );
  ~MidiOutJack( void );
  ApiType getCurrentApi( void ) throw() { return UNIX_JACK; };
  bool hasVirtualPorts() const { return true; }
  void openPort( unsigned int portNumber, const std::string & portName );
  void openVirtualPort( const std::string portName );
  void openPort( const PortDescriptor & port, const std::string & portName);
  Pointer<PortDescriptor> getDescriptor(bool local=false);
  PortList getPortList(int capabilities);
  void closePort( void );
  unsigned int getPortCount( void );
  std::string getPortName( unsigned int portNumber );
  void sendMessage( std::vector<unsigned char> &message );

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
  MidiInAlsa( const std::string clientName, unsigned int queueSizeLimit );
  ~MidiInAlsa( void );
  ApiType getCurrentApi( void ) throw() { return LINUX_ALSA; };
  bool hasVirtualPorts() const { return true; }
  void openPort( unsigned int portNumber, const std::string & portName );
  void openVirtualPort( const std::string portName );
  void openPort( const PortDescriptor & port, const std::string & portName);
  Pointer<PortDescriptor> getDescriptor(bool local=false);
  PortList getPortList(int capabilities);
  void closePort( void );
  unsigned int getPortCount( void );
  std::string getPortName( unsigned int portNumber );

protected:
  static void * alsaMidiHandler( void *ptr ) throw();
  void initialize( const std::string& clientName );
  friend class AlsaMidiData;
};

class MidiOutAlsa: public MidiOutApi
{
public:
  MidiOutAlsa( const std::string clientName );
  ~MidiOutAlsa( void );
  ApiType getCurrentApi( void ) throw() { return LINUX_ALSA; };
  bool hasVirtualPorts() const { return true; }
  void openPort( unsigned int portNumber, const std::string & portName );
  void openVirtualPort( const std::string portName );
  void openPort( const PortDescriptor & port, const std::string & portName);
  Pointer<PortDescriptor> getDescriptor(bool local=false);
  PortList getPortList(int capabilities);
  void closePort( void );
  unsigned int getPortCount( void );
  std::string getPortName( unsigned int portNumber );
  void sendMessage( std::vector<unsigned char> &message );

protected:
  void initialize( const std::string& clientName );
};

#endif

#if defined(__WINDOWS_MM__)

class MidiInWinMM: public MidiInApi
{
public:
  MidiInWinMM( const std::string clientName, unsigned int queueSizeLimit );
  ~MidiInWinMM( void );
  ApiType getCurrentApi( void ) throw() { return WINDOWS_MM; };
  bool hasVirtualPorts() const { return false; }
  void openPort( unsigned int portNumber, const std::string & portName );
  void openVirtualPort( const std::string portName );
  void openPort( const PortDescriptor & port, const std::string & portName);
  Pointer<PortDescriptor> getDescriptor(bool local=false);
  PortList getPortList(int capabilities);
  void closePort( void );
  unsigned int getPortCount( void );
  std::string getPortName( unsigned int portNumber );

protected:
  void initialize( const std::string& clientName );
  friend struct WinMMCallbacks;

};

class MidiOutWinMM: public MidiOutApi
{
public:
  MidiOutWinMM( const std::string clientName );
  ~MidiOutWinMM( void );
  ApiType getCurrentApi( void ) throw() { return WINDOWS_MM; };
  bool hasVirtualPorts() const { return false; }
  void openPort( unsigned int portNumber, const std::string & portName );
  void openVirtualPort( const std::string portName );
  void openPort( const PortDescriptor & port, const std::string & portName);
  Pointer<PortDescriptor> getDescriptor(bool local=false);
  PortList getPortList(int capabilities);
  void closePort( void );
  unsigned int getPortCount( void );
  std::string getPortName( unsigned int portNumber );
  void sendMessage( std::vector<unsigned char> &message );

protected:
  void initialize( const std::string& clientName );
};

#endif

#if defined(__RTMIDI_DUMMY__)

class MidiInDummy: public MidiInApi
{
public:
  MidiInDummy( const std::string /*clientName*/, unsigned int queueSizeLimit )
    : MidiInApi( queueSizeLimit ) {
    error( RTMIDI_ERROR(_("MidiInDummy: This class provides no functionality."),
			Error::WARNING) );
  }
  ApiType getCurrentApi( void ) { return RTMIDI_DUMMY; }
  bool hasVirtualPorts() const { return false; }
  void openPort( unsigned int /*portNumber*/, const & std::string /*portName*/ ) {}
  void openVirtualPort( const std::string /*portName*/ ) {}
  void openPort( const PortDescriptor & port, const & std::string portName) {}
  Pointer<PortDescriptor> getDescriptor(bool local=false) { return 0; }
  PortList getPortList(int capabilities) { return PortList(); }
  void closePort( void ) {}
  unsigned int getPortCount( void ) { return 0; }
  std::string getPortName( unsigned int portNumber ) { return ""; }

protected:
  void initialize( const std::string& /*clientName*/ ) {}
};

class MidiOutDummy: public MidiOutApi
{
public:
  MidiOutDummy( const std::string /*clientName*/ ) {
    error( RTMIDI_ERROR(_("MidiInDummy: This class provides no functionality."),
			Error::WARNING) );
  }
  ApiType getCurrentApi( void ) { return RTMIDI_DUMMY; }
  bool hasVirtualPorts() const { return false; }
  void openPort( unsigned int /*portNumber*/, const & std::string /*portName*/ ) {}
  void openVirtualPort( const std::string /*portName*/ ) {}
  void openPort( const PortDescriptor & port, const & std::string portName) {}
  Pointer<PortDescriptor> getDescriptor(bool local=false) { return 0; }
  PortList getPortList(int capabilities) { return PortList(); }
  void closePort( void ) {}
  unsigned int getPortCount( void ) { return 0; }
  std::string getPortName( unsigned int /*portNumber*/ ) { return ""; }
  void sendMessage( std::vector<unsigned char> & /*message*/ ) {}

protected:
  void initialize( const std::string& /*clientName*/ ) {}
};

#endif

NAMESPACE_RTMIDI_END

class   RtMidi: public  rtmidi::Midi {
public:
	enum Api {
		UNSPECIFIED  = rtmidi::UNSPECIFIED,
		MACOSX_CORE  = rtmidi::MACOSX_CORE,
		LINUX_ALSA   = rtmidi::LINUX_ALSA,
		UNIX_JACK    = rtmidi::UNIX_JACK,
		WINDOWS_MM   = rtmidi::WINDOWS_MM,
		RTMIDI_DUMMY = rtmidi::DUMMY
	};
};

class RtMidiIn: public rtmidi::MidiIn {
public:
	RTMIDI_DEPRECATED(RtMidiIn( RtMidi::Api api = RtMidi::UNSPECIFIED,
				     const std::string clientName = std::string( "RtMidi Input Client"))):
	MidiIn((rtmidi::ApiType)api,
		clientName) {}
};
class RtMidiOut: public rtmidi::MidiOut {
public:
	RTMIDI_DEPRECATED(RtMidiOut( RtMidi::Api api = RtMidi::UNSPECIFIED,
				   const std::string clientName = std::string( "RtMidi Output Client"))):
	MidiOut((rtmidi::ApiType)api,
		clientName) {}
};
typedef rtmidi::Error   RtMidiError;
#endif
