/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef _DECODER_H_
#define _DECODER_H_

#include <string.h>
#include <time.h>
#include <android/log.h>
#include <pthread.h>
#include "decoder/AudioTrack.h"

#define SASKEN_NOP 0 // interface test message
#define SASKEN_PREPARED 1
#define SASKEN_PLAYBACK_COMPLETE 2

class decoder 
{
public:
	decoder();
	~decoder();
	void start(jobject weak_this);
	void pause();
	void stop();
	virtual void setBuffer(int size) {m_size = size;}
	virtual jbyte* getData(){ return m_data;}
	virtual bool open(char *file) {return false;}
	virtual int read() {return 0;}
	virtual void close() {}
	virtual int rate() {return m_rate;}
	virtual int channels() {return m_channels;}
	virtual void seek(int msec) {}
	virtual long getLength() {return m_length;}
	virtual bool isPlaying() {return m_running;}
	virtual bool isPaused() {return m_pause;}
	virtual void rewind() {}
	virtual long position() {return 0;}
	virtual long skipSamples(long samples) {return 0;}
	static void *ThreadWrapper(void *me);
	void threadEntry();

protected:
	// decoder stuff
	jbyte   *m_data;
	size_t m_size;
	int    m_channels;
	int    m_rate;
	long   m_length;

	// thread
	volatile bool m_running;
	pthread_t m_thread;

	// playback control
	volatile bool m_pause;
	bool      m_playback;
	pthread_mutex_t m_pause_mutex;
	pthread_cond_t  m_pause_cond;
	jobject m_decoder;
};

#endif
