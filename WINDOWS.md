# Building and Using RtMidi on Windows

## Requirements

Before building, ensure the following are installed via the **Visual Studio Installer**:

| Component | Notes |
|---|---|
| **Visual Studio 2019 or newer** | Visual Studio 2019 or newer will work |
| **Desktop development with C++** | Core workload required for the build |
| **Windows SDK 22000 or newer** | Required; select version 10.0.22000.0 or later in the Visual Studio Installer |

---


## Building RtMidi

The Visual Studio solution is located at:

```
tests/RtMidi.sln
```

1. Open `tests/RtMidi.sln` in Visual Studio.
2. Select the desired configuration and platform from the toolbar:
   - **Release | x64** (recommended for production use)
   - **Debug | x64** (recommended during development)
3. Build the solution (**Build → Build Solution** or <kbd>Ctrl+Shift+B</kbd>).

After a successful build, the compiled static library will be at:

| Configuration | Output path |
|---|---|
| Release \| x64 | `tests/x64/Release/rtmidilib.lib` |
| Debug \| x64 | `tests/x64/Debug/rtmidilib.lib` |

The RtMidi library build is now complete.

---

## Using RtMidi in Your Own Project

Building RtMidi and linking it into your own project are two separate steps. Once you have the library built, you need two files from this repository to use RtMidi in a personal C++ project:

| File | Purpose |
|---|---|
| `RtMidi.h` | Header — add its containing directory as an include path |
| `tests/x64/Release/rtmidilib.lib` or `tests/x64/Debug/rtmidilib.lib` | Static library matching your chosen configuration — link it into your project |

### Visual Studio Project Configuration

Open your project's **Property Pages** (**Project → Properties**) and set the following:

| Property Page | Setting | Value |
|---|---|---|
| **C/C++ → General** | Additional Include Directories | Path to the directory containing `RtMidi.h` (the repository root) |
| **Linker → General** | Additional Library Directories | `tests/x64/$(Configuration)` |
| **Linker → Input** | Additional Dependencies | `rtmidilib.lib; winmm.lib; windowsapp.lib` |

> [!IMPORTANT]
> - `$(Configuration)` is a Visual Studio macro that resolves automatically to `Debug` or `Release` to match the active build configuration.
> - Your consuming application must target **x64** to match the library architecture.
> - The application and the library must use the **same configuration**. Linking a Debug application against the Release `rtmidilib.lib` (or vice versa) will cause an **LNK2038** runtime-library mismatch error.

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
