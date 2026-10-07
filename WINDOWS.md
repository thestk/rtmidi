# Building and Using RtMidi on Windows

## Requirements

Before building, ensure the following are installed via the **Visual Studio Installer**:

| Component | Notes |
|---|---|
| **Visual Studio 2019 or newer** | Visual Studio 2019 or newer will work |
| **Desktop development with C++** | Core workload required for the build |
| **Windows 11 SDK** | Select the latest available version |
| **MSVC v142 build tools** | Required even when using a newer Visual Studio version |

---


## Building RtMidi

The Visual Studio solution is located at:

```
tests/RtMidi.sln
```

1. Open `tests/RtMidi.sln` in Visual Studio.
2. Set the configuration to **Release** and the platform to **x64**.
3. Build the solution (**Build → Build Solution** or <kbd>Ctrl+Shift+B</kbd>).

After a successful build, the compiled static library will be at:

```
tests/x64/Release/rtmidilib.lib
```

The RtMidi library build is now complete.

---

## Using RtMidi in Your Own Project

Building RtMidi and linking it into your own project are two separate steps. Once you have the library built, you need two files from this repository to use RtMidi in a personal C++ project:

| File | Purpose |
|---|---|
| `RtMidi.h` | Header — add its containing directory as an include path |
| `tests/x64/Release/rtmidilib.lib` | Static library — link it into your project |

### Visual Studio Project Configuration

Open your project's **Property Pages** (**Project → Properties**) and set the following:

| Property Page | Setting | Value |
|---|---|---|
| **C/C++ → General** | Additional Include Directories | Path to the directory containing `RtMidi.h` (the repository root) |
| **Linker → General** | Additional Library Directories | Path to the directory containing `rtmidilib.lib` (e.g. `tests/x64/Release/`) |
| **Linker → Input** | Additional Dependencies | `rtmidilib.lib; winmm.lib; windowsapp.lib` |

> [!IMPORTANT]
> Make sure the **Configuration** and **Platform** dropdowns in Property Pages match the build you intend to run (e.g. **Release / x64**).

---

## Example Usage

The following minimal program verifies that your project can include and link against RtMidi successfully:

```cpp
#include <iostream>
#include "RtMidi.h"

using namespace std;

int main()
{
    RtMidiOut midiout;
    cout << "MIDI output ports: " << midiout.getPortCount() << endl;
    return 0;
}
```

Build this in your project. If it compiles and links without errors, RtMidi is correctly set up.
