package com.sasken.player.decoders;

import java.io.IOException;

import android.os.Handler;
import android.os.Message;
import android.util.Log;

import com.sasken.player.equalizers.SaskenEqualizer;

public class SaskenDecoder {

	private static final int SASKEN_NOP = 0; // interface test message
	private static final int SASKEN_PREPARED = 1;
	private static final int SASKEN_PLAYBACK_COMPLETE = 2;
	private static final String LOG_TAG = "SaskenDecoder";
	private static SaskenEqualizer equalizer = null;

	/**
	 * Interface definition for a callback to be invoked when playback of a
	 * media source has completed.
	 */
	public interface OnPlaybackCompletedListener {
		/**
		 * Called when the end of a media source is reached during playback.
		 * 
		 * @param mp
		 *            the MediaPlayer that reached the end of the file
		 */
		void onPlaybackCompleted(SaskenDecoder sd);
	}

	// Load the native libraries
	static {
		System.loadLibrary("decoder");
	}

	public SaskenDecoder(SaskenEqualizer e) {
		mEventHandler = new EventHandler(this);
		equalizer = e;
	}

	private long mFile = 0;
	private static OnPlaybackCompletedListener mListener = null;
	private final EventHandler mEventHandler;

	private class EventHandler extends Handler {
		private final SaskenDecoder mSaskenDecoder;

		public EventHandler(SaskenDecoder sd) {
			mSaskenDecoder = sd;
		}

		@Override
		public void handleMessage(Message msg) {

			switch (msg.what) {
			case SASKEN_PREPARED:
				Log.i(LOG_TAG, "SaskenDecoder prepared");
				// if (mOnPreparedListener != null)
				// mOnPreparedListener.onPrepared(mMediaPlayer);
				return;

			case SASKEN_PLAYBACK_COMPLETE:
				Log.i(LOG_TAG, "Playback complete");
				if (mListener != null)
					mListener.onPlaybackCompleted(mSaskenDecoder);
				return;

			case SASKEN_NOP: // interface test message - ignore
				Log.i(LOG_TAG, "Interface test message");
				break;

			default:
				Log.e(LOG_TAG, "Unknown message type " + msg.what);
				return;
			}
		}
	}

	// private FileOutputStream fos;

	public boolean Init(String fileName) throws IOException {

		mFile = OpenFile(fileName);

		// fos = new FileOutputStream (new File ("/sdcard/data.bin"));

		return (mFile == 0) ? false : true;

	}

	public void setOnCompletitionListener(OnPlaybackCompletedListener listener) {
		mListener = listener;
	}

	public long GetId() {
		return mFile;
	}

	public void Start() {
		Log.i("SaskenDecoder", "Starting...");
		Start(mFile);
	}

	public void Pause() {
		Pause(mFile);
	}

	public void Stop() {
		Stop(mFile);
	}

	public void callBack(int what) {
		Log.i("SaskenDecoder", "Callback reached");
		if (mEventHandler != null) {
			Message m = mEventHandler.obtainMessage(what);
			mEventHandler.sendMessage(m);
		}
	}

	public int Decode(byte[] buffer) throws IOException {
		int len = ReadSamples(mFile, buffer);
		// fos.write(buffer, 0, len);
		return len;
	}

	public int GetSampleRateInHz() {
		return GetRate(mFile);
	}

	public int GetNumberOfChannels() {
		return GetNumChannels(mFile);
	}

	public void Release() throws IOException {
		CloseFile(mFile);
		// fos.close();JNIExampleInterface
	}

	public void seekTo(int msec) {
		seekTo(mFile, msec);
	}

	public boolean SetBufferSize(int size) {
		return SetBufferSize(mFile, size);
	}

	public boolean isPlaying() {
		return isPlaying(mFile);
	}

	public boolean isPaused() {
		return isPaused(mFile);
	}

	public static native int CheckFile(String file);

	private native boolean isPlaying(long file);

	private native boolean isPaused(long file);

	private native long OpenFile(String file);

	private native int ReadSamples(long file, byte[] buffer);

	private native int GetRate(long file);

	private native int GetNumChannels(long file);

	private native void CloseFile(long file);

	private native void seekTo(long file, int msec);

	private native boolean SetBufferSize(long file, int size);

	private native void Start(long file);

	private native void Pause(long file);

	private native void Stop(long file);
}
