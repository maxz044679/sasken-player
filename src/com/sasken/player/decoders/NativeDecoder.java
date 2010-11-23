package com.sasken.player.decoders;

import android.util.Log;

public class NativeDecoder {
	
	final private String mName;

	public NativeDecoder(String name)
	{
		mName = name;
		Log.i(this.toString(),"Load Library " + name);
		System.loadLibrary(mName);
	}
	
	public native boolean Init(String file);
	
	public native int GetSampleRateInHz();
	
	public native int GetNumberOfChannels();
	
	public native int Decode(byte[] buffer);
	
	public native void Release();
	
	public native void seekTo(int msec);
	
}
