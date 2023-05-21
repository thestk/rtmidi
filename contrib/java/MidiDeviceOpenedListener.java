package com.yellowlab.rtmidi;

import android.media.midi.MidiDevice;
import android.media.midi.MidiManager;

public class MidiDeviceOpenedListener implements MidiManager.OnDeviceOpenedListener {
    private long nativeId;
    private boolean isOutput;

    public MidiDeviceOpenedListener(long id, boolean output) {
        nativeId = id;
        isOutput = output;
    }

    @Override
    public void onDeviceOpened(MidiDevice midiDevice) {
        midiDeviceOpened(midiDevice, nativeId, isOutput);
    }

    private native static void midiDeviceOpened(MidiDevice midiDevice, long id, boolean isOutput);
}
