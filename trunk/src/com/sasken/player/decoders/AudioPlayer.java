package com.sasken.player.decoders;

import java.io.IOException;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.util.Log;

import com.sasken.player.equalizers.SaskenEqualizer;
import com.sasken.player.visualizations.SaskenVisualizer;

public class AudioPlayer {

	private final static String LOG_TAG = "AudioPlayer"; 
	
	public enum PlayerState {
		Idle,
		Initialized,
		Prepared,
		Started,
		Stopped,
		Paused,
		PlaybackCompleted
	}

	private AudioTrack mTrack;
	private SaskenVisualizer myVU;
	private SaskenVisualizer myVis;
	protected int mBufferSize;
	protected SaskenDecoder mDecoder;
	private String mFile;
	protected PlayerState mState;
	protected SaskenEqualizer mEqualizer;
	private AudioThread mThread;
	
	public static class AudioPlayerException extends Exception {
		private static final long serialVersionUID = -2911994926926588084L;
	}

	private class AudioThread extends Thread {
		@Override
		public void run() {
			byte buffer[] = new byte[mBufferSize];
			int samplesReaded = 0;
			int samplesPlayed = 0;
			int samplesWritten = 0;
			int len = 0;
			mTrack.play();
			try {

				while(true) {

					synchronized (this) {
						while (mState == PlayerState.Paused) {
							wait();
						}
					}

					if (mState == PlayerState.Stopped) {
						break;
					}

					if (samplesReaded == samplesPlayed) {
						samplesReaded = mDecoder.Decode(buffer);
						samplesPlayed = 0;

						// if there are no more samples we are done
						if (samplesReaded == 0) {
							mState = PlayerState.PlaybackCompleted;
							// set the position to the beginning
							mDecoder.seekTo(0);
							break;
						}

						if (mEqualizer != null) {
							//mEqualizer.process(buffer);
						}
					}

					len = samplesReaded - samplesPlayed;
					samplesWritten = mTrack.write(buffer, samplesPlayed, len);
					if(myVU.isVisualizationOn()){
						myVU.processBuffer(buffer);
						//myVU.postInvalidate();
						//myVU.draw();
					}
					if(myVis != null){
						if(myVis.isVisualizationOn()){
							myVis.processBuffer(buffer);
							//myVis.draw();
						}
					}
					if (samplesWritten < 0) {
						Log.w(LOG_TAG, String.format(
								"AudioTrack.write(%dB) returned error %d",
								len, samplesWritten));
					} else if (samplesWritten != len) {
						Log.w(LOG_TAG, String.format(
								"Short audio write.      Wrote %dB, not %dB",
								samplesWritten, len));
						// Buffer is full, so yield cpu for a while
						Thread.sleep(100);
					}
					samplesPlayed += samplesWritten;
				}
				Log.i(LOG_TAG, "Finish the thread cycle");
			} catch (IOException e) {
				e.printStackTrace();
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
	}

	public AudioPlayer() {
		// No Audio file selected
		mFile = "";

		// we are on idle state by default
		mState = PlayerState.Idle;
		
		// no equalizer by default
		mEqualizer = null;
	}

	/*
	 * Sets the data source (file-path) to use.
	 *
	 * Parameters:
	 * path - the path of the file of the stream you want to play
	 *
	 * Throws:
	 * IllegalStateException - if it is called in an invalid state
	 * IOException
	 */
	public void setDataSource (String file) throws IOException {
		
		// Only on idle state we can set the data source
		if (mState != PlayerState.Idle) {
			throw new IllegalStateException();
		}

		// store the path
		mFile = file;
		
		// move to the next state
		mState = PlayerState.Initialized;
	}

	/*
	 * Prepares the player for playback. 
	 * After setting the datasource you need to call prepare() which blocks 
	 * until AudioPlayer is ready for playback.
	 * 
	 * Throws:
	 * IllegalStateException - if it is called in an invalid state
	 * IOException
	 */
	public void  prepare () throws IOException, AudioPlayerException {

		switch (mState) {
			case Initialized:

				// create a new decoder
				//mDecoder = new SaskenDecoder();

				// if we cannot decode the file, it's over
				if (!mDecoder.Init(mFile)) {
					Log.e(LOG_TAG, "Error intializing the Sasken Decoder.");
					throw new AudioPlayerException();
				}

				// query the recommended buffer size
				mBufferSize = AudioTrack.getMinBufferSize(
					mDecoder.GetSampleRateInHz(), 
					(mDecoder.GetNumberOfChannels() == 1) ? AudioFormat.CHANNEL_CONFIGURATION_MONO: AudioFormat.CHANNEL_CONFIGURATION_STEREO, 
					AudioFormat.ENCODING_PCM_16BIT
				);

				// use at least 4 times the size recomended
				mBufferSize *=4;

				Log.i(LOG_TAG, "BufferSize = " + mBufferSize);
				Log.i(LOG_TAG, "SampleRate = " + mDecoder.GetSampleRateInHz());
				Log.i(LOG_TAG, "Channels   = " + mDecoder.GetNumberOfChannels());
				mDecoder.SetBufferSize(mBufferSize);

				// initialize the audioTrack
				mTrack = new AudioTrack(AudioManager.STREAM_MUSIC,
					mDecoder.GetSampleRateInHz(),
					(mDecoder.GetNumberOfChannels() == 1) ? AudioFormat.CHANNEL_CONFIGURATION_MONO : AudioFormat.CHANNEL_CONFIGURATION_STEREO,
					AudioFormat.ENCODING_PCM_16BIT,
					mBufferSize, 
					AudioTrack.MODE_STREAM
				);

				if (mTrack.getState() != AudioTrack.STATE_INITIALIZED) {
					Log.e(LOG_TAG, "Error intializing Android audio system.");
					throw new AudioPlayerException();
				}

				// if there is an equalizer then initialize it
				if (mEqualizer != null) {
					//mEqualizer.init(mDecoder.GetNumberOfChannels(), mEqualizer.getF(),
					//	mDecoder.GetSampleRateInHz());
				}

				// move to the next state
				mState = PlayerState.Prepared;

				break;

			// If we were stopped, we will start from the beginning
			case Stopped:
				mDecoder.seekTo(0);

				// move to the next state
				mState = PlayerState.Prepared;
				break;
			default:
				throw new IllegalStateException();
		}
	}

	/*
	 * Starts or resumes playback. 
	 * If playback had previously been paused, playback will continue from where it was paused. 
	 * If playback had been stopped, or never started before, playback will start at the beginning.
	 * 
	 * Throws:
	 * IllegalStateException - if it is called in an invalid state
	 */
	public void  start  () {
		switch (mState) {
			case Prepared:
			case PlaybackCompleted:
				mThread = new AudioThread();
				mThread.start();
				mState = PlayerState.Started;
				break;
			case Paused:
				mState = PlayerState.Started;
				mTrack.play();
				synchronized (mThread) {
					mThread.notify();
				}
				break;
			default:
				throw new IllegalStateException();
		}
	}

	/*
	 * Pauses playback. Call start() to resume.
	 * 
	 * Throws:
	 * IllegalStateException - if the internal player engine has not been initialized. 
	 */
	public void  pause () {
		
		switch (mState) {
			case Started:
				mState = PlayerState.Paused;
				mTrack.pause();
				break;
			default:
				throw new IllegalStateException();
		}
	}

	
	/*
	 * Stops playback after playback has been stopped or paused.
	 * 
	 * Throws:
	 * IllegalStateException - if the internal player engine has not been initialized. 
	 */
	public void stop () {

		switch (mState) {
			case Paused:
				mTrack.flush();
				mTrack.stop();
				mState = PlayerState.Stopped;
				synchronized (mThread) {
					mThread.notify();
				}
				break;
			case Started:
				mTrack.stop();
				mState = PlayerState.Stopped;
				break;
			case Stopped:
				break;
			default:
				throw new IllegalStateException();
		}
	}

	/*
	 * Returns the current state of the AudioPlayer
	 * 
	 * Throws:
	 * IllegalStateException - if the internal player engine has not been initialized. 
	 */
	public PlayerState getState() {
		return mState;
	}

	public void setEqualizer(SaskenEqualizer eq) {

		// we set the new equalizer
		mEqualizer = eq;

		if (mEqualizer != null) {
			// do initialization
		}
	}
	
	public void setVUVisualizer(SaskenVisualizer visualizer){
		myVU = visualizer;
	}
	
	public void setVisualizer(SaskenVisualizer visualizer){
		myVis = visualizer;
	}
	
	public void unsetVisualizer(){
		myVis = null;
	}
}
