# Example iOS app

A minimal SwiftUI app demonstrating RtMidi on iOS. Lists MIDI ports, sends
Note On/Off, SysEx, and an Identity Request, and shows received MIDI in a
simple monitor with basic Identity Reply decoding.

`RtMidiExample/RtMidi.cpp` and `RtMidi.h` are symlinks to the files at the
root of the repository.

Generate the Xcode project with [XcodeGen](https://github.com/yonaskolb/XcodeGen):

```sh
xcodegen generate
open RtMidiExample.xcodeproj
```
