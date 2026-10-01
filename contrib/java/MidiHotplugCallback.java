package com.yellowlab.rtmidi;

import android.media.midi.MidiDeviceInfo;
import android.media.midi.MidiManager;

/**
 * This class must be included in the Android app when it uses
 * RtMidi::createHotplug() with the AMIDI backend. MidiManager.DeviceCallback
 * is an abstract class, so the C++ code cannot provide it through JNI alone.
 */
public class MidiHotplugCallback extends MidiManager.DeviceCallback {
    private long nativeId;

    public MidiHotplugCallback(long id) {
        nativeId = id;
    }

    @Override
    public void onDeviceAdded(MidiDeviceInfo device) {
        devicesChanged(nativeId);
    }

    @Override
    public void onDeviceRemoved(MidiDeviceInfo device) {
        devicesChanged(nativeId);
    }

    private native static void devicesChanged(long id);
}
