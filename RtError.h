/************************************************************************/
/*! \class RtError
    \brief Exception handling class for RtAudio & RtMidi.

    The RtError class is quite simple but it does allow errors to be
    "caught" by RtError::Type. See the RtAudio and RtMidi
    documentation to know which methods can "throw" an RtError.

*/
/************************************************************************/

#ifndef RTERROR_H
#define RTERROR_H

#include <iostream>
#include <string>

class RtError
{
public:
  //! Defined RtError types.
  enum Type {
    WARNING,
    DEBUG_WARNING,
    UNSPECIFIED,
    NO_DEVICES_FOUND,
    INVALID_DEVICE,
    INVALID_STREAM,
    MEMORY_ERROR,
    INVALID_PARAMETER,
    DRIVER_ERROR,
    SYSTEM_ERROR,
    THREAD_ERROR
  };

protected:
  std::string message_;
  Type type_;

public:
  //! The constructor.
  RtError(const std::string& message, Type type = RtError::UNSPECIFIED) : message_(message), type_(type) {}

  //! The destructor.
  virtual ~RtError(void) {}

  //! Prints "thrown" error message to stdout.
  virtual void printMessage(void) { std::cout << '\n' << message_ << "\n\n"; }

  //! Returns the "thrown" error message type.
  virtual const Type& getType(void) { return type_; }

  //! Returns the "thrown" error message string.
  virtual const std::string& getMessage(void) { return message_; }
};

#endif
