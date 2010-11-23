#include "decoder.h"
#include "../Visualizers/VU/VUVisualizer.h"
#include "../Visualizers/VI20Band/VI20BandVisualizer.h"

#define DEBUG_TAG "NDK_Decoder"
#define LOG(...) __android_log_print(ANDROID_LOG_INFO, DEBUG_TAG, __VA_ARGS__)

// Constructor
decoder::decoder()
		: m_data(NULL),
		  m_size(0),
		  m_running(false),
		  m_pause(false),
		  m_playback(false)
{
	m_pause_mutex = PTHREAD_MUTEX_INITIALIZER;
	m_pause_cond = PTHREAD_COND_INITIALIZER;
}

// Destructor
decoder::~decoder()
{

	// stop the decoding
	stop();

	// close
	close();
}

void decoder::pause()
{
	// if we are no longer running, we cannot do pause
	if (m_running)
	{
		pthread_mutex_lock( &m_pause_mutex );
		// check pause
		if (!m_pause)
		{
			// Send Pause
			m_pause = true;
		}

		pthread_mutex_unlock( &m_pause_mutex );
	}

}

void decoder::start(jobject decoder)
{
	if (!m_running) {
		m_decoder = decoder;
		// if not running start the thread
		m_running = true;
		pthread_create(&m_thread, NULL, ThreadWrapper, this);
	} else {
		// if already running check for pause
		pthread_mutex_lock( &m_pause_mutex );
		if (m_pause) {
			// resume the playback
			pthread_cond_signal( &m_pause_cond );
		}
		pthread_mutex_unlock( &m_pause_mutex );
	}
}

void decoder::stop()
{
	// check that we are running
	if (m_running)
	{
		// stop the playback
		m_running = 0;

		// if we are on pause, resume the pause
		pthread_mutex_lock( &m_pause_mutex );
		if (m_pause)
		{
			// resume the playback
			pthread_cond_signal( &m_pause_cond );
		}
		pthread_mutex_unlock( &m_pause_mutex );

		// wait for the thread to finish
		pthread_join(m_thread, NULL);
	}
}

// static
void *decoder::ThreadWrapper(void *me) {
	static_cast<decoder *>(me)->threadEntry();
	return NULL;
}

void decoder::threadEntry()
{
	size_t decoded;
	bool playbackCompleted = false;
	jint *ng;

	// 5 miliseconds
	struct timespec sleepTime;
	sleepTime.tv_sec = 0;
	sleepTime.tv_nsec = 5 * 1000 * 1000;

	LOG("ThreadEntry");

	// Attach the current thread to the Java VM
	JNIEnv* env;
	javaVM->AttachCurrentThread(&env, NULL);

	// create a new scope for local references
	env->PushLocalFrame(2);

	// Obtain the channel configuration
	int channelConfig = m_channels == 1 ? CHANNEL_CONFIGURATION_MONO : CHANNEL_CONFIGURATION_STEREO;

	// calculate the buffer size required
	m_size = env->CallStaticIntMethod(cAudioTrack, mGetMinBufferSize, m_rate, channelConfig, ENCODING_PCM_16BIT);

	// if we already have a buffer, release the buffer
	if (m_data) {
		free (m_data);
	}

	// allocate a new buffer
	m_data = (jbyte *)malloc(m_size);

	// create a new track object
	jobject track = env->NewObject(cAudioTrack, mAudioTrack, STREAM_MUSIC, m_rate, channelConfig,
		ENCODING_PCM_16BIT, m_size, MODE_STREAM);

	// start the playback
	env->CallNonvirtualVoidMethod(track, cAudioTrack, mPlay);

	// create an array on the java space for the data
	jarray buffer = env->NewByteArray(m_size);

	// Create an EQ instance
	jobject eqobj = 0;
	jclass eqclass = 0;

	if (!eqobj) {
		eqobj = env->GetStaticObjectField(cSaskenDecoder, mEq);
		//eqobj = env->NewGlobalRef(eqobj);
		if (!eqobj) {
			LOG("Out Memory, File: %s, Line: %d", __FILE__, __LINE__);
		}
	}

	eqclass = env->GetObjectClass(eqobj);
	jboolean isEQEnabled = env->GetStaticBooleanField(eqclass, mEnable);
	jboolean wasChanged = env->GetStaticBooleanField(eqclass, mChange);
	jint EQid = env->GetStaticBooleanField(eqclass, mEqID);

	if (isEQEnabled) {
		SaskenEqualizer_init((jint)channels(), EQid, rate());
		LOG("srate is: %d, Channels: %d, EQid: %d", rate(), channels(), EQid);
	}
	//env->DeleteLocalRef(eqclass);
	LOG("EQ is enabled?: %s", isEQEnabled ? "true" : "false");

	while (m_running) {

		pthread_mutex_lock( &m_pause_mutex );
		if (m_pause) {
			// Pause detected, we should pause the playback
			env->CallNonvirtualVoidMethod(track, cAudioTrack, mPause);

			// wait for the signal to continue
			pthread_cond_wait( &m_pause_cond, &m_pause_mutex);

			// we are no longer on pause
			m_pause = false;

			// check if we are going to still running
			if (!m_running) {
				LOG("Stopped after pause");
				pthread_mutex_unlock( &m_pause_mutex );
				break;
			}

			// resume the playback
			env->CallNonvirtualVoidMethod(track, cAudioTrack, mPlay);
		}
		pthread_mutex_unlock( &m_pause_mutex );

		// decode another block
		decoded = read();

		// if we decoded until the end of file
		if (decoded == 0)
		{
			// check until we reach the end of the playback
			while (m_running) {
				int play_state  = env->CallNonvirtualIntMethod(track, cAudioTrack, mGetPlayState);

				if (play_state == PLAYSTATE_PLAYING) {
					playbackCompleted = true;
					break;
				}

				LOG("Waiting for End of Playback");

				// sleeping a bit before asking again if the playback has ended
				nanosleep(&sleepTime, NULL);
			}

			// we are no longer running
			m_running = false;
			break;
		}

		/**
		 * To avoid deadlocks, we should get no critical regions first, before
		 * enter into a critical region(s).
		 */
		wasChanged = env->GetStaticBooleanField(eqclass, mChange);
		void* pBuffer = env->GetPrimitiveArrayCritical(buffer, NULL);
		if (pBuffer) {
			if (isEQEnabled) {
				SaskenEqualizer_process(m_data, decoded);
			}
			//================================================
			VU_process_Buffer(env, eqobj, m_data, decoded);
			Vis_process_Buffer();
			//================================================
			memcpy(pBuffer, m_data, decoded);
			env->ReleasePrimitiveArrayCritical(buffer, pBuffer, 0);
			env->CallNonvirtualIntMethod(track, cAudioTrack, mWrite, buffer, 0, decoded);
			if (wasChanged) {
				jobject myobj = env->CallObjectMethod(eqobj, mGetG);
				jintArray arr = (jintArray)myobj;
				ng = env->GetIntArrayElements(arr, 0);
				SaskenEqualizer_configure(ng);
				env->ReleaseIntArrayElements(arr, ng, 0);
				env->DeleteLocalRef(myobj);
				env->SetStaticBooleanField(eqclass, mChange, 0);
			}
		} else {
			LOG("Failed to get pointer to array bytes");
		}
	}

	LOG("Done playing");

	// stop the track
	env->CallNonvirtualVoidMethod(track, cAudioTrack, mStop);

	// release the resource
	env->CallNonvirtualVoidMethod(track, cAudioTrack, mRelease);

	if (m_decoder)
	{
		if (playbackCompleted)
		{
			LOG("Making the callback");
			env->CallNonvirtualVoidMethod(m_decoder, cSaskenDecoder, mCallBack, (jint)SASKEN_PLAYBACK_COMPLETE);
		}

		// delete the global reference
		env->DeleteGlobalRef(m_decoder);
	}

	// delete the global reference
	if (m_decoder)
		// release the local scope
		env->PopLocalFrame(NULL);

	// detach the thread from the jvm
	javaVM->DetachCurrentThread();

	// rewind the file to the beginning
	rewind();

	// Release the buffer
	if (m_data) {
		free (m_data);
		m_data = NULL;
	}
}
