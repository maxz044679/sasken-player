#include "Mpg123Decoder.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <android/log.h>

#define DEBUG_TAG "NDK_MP3_Dec"

Mpg123Decoder::~Mpg123Decoder()
{

}

void Mpg123Decoder::cleanup(mpg123_handle* handle)
{
	mpg123_close(handle);
	mpg123_delete(handle);
	mpg123_exit();
}

bool Mpg123Decoder::open(char *file)
{
	mpg123_handle *mh = NULL;
	int  channels = 0, encoding = 0;
	long rate = 0;
	int  err  = MPG123_OK;

    __android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Open [%s]", file);

	err = mpg123_init();
	if( err != MPG123_OK || (mh = mpg123_new(NULL, &err)) == NULL
			|| mpg123_open(mh, file) != MPG123_OK
			|| mpg123_getformat(mh, &rate, &channels, &encoding) != MPG123_OK )
	{
	    __android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Trouble with mpg123: %s\n",
				mh==NULL ? mpg123_plain_strerror(err) : mpg123_strerror(mh) );
		cleanup(mh);
		return false;
	}

	if(encoding != MPG123_ENC_SIGNED_16)
	{ /* Signed 16 is the default output format anyways; it would actually by only different if we forced it.
	              So this check is here just for this explanation. */
	    __android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "invalid BitsPerSample");
		cleanup(mh);
		return false;
	}
	/* Ensure that this output format will not change (it could, when we allow it). */
	mpg123_format_none(mh);
	mpg123_format(mh, rate, channels, encoding);

	m_buffer_size = mpg123_outblock( mh );
	m_buffer = (unsigned char*)malloc(m_buffer_size);

	m_handle = mh;
	m_channels = channels;
	m_rate = rate;

	int length = mpg123_length( mh );
	if( length == MPG123_ERR )
		m_length = 0;
	else
		m_length = length / rate;

	// return success
	return true;
}

int Mpg123Decoder::readBuffer()
{
	size_t done = 0;
	int err = mpg123_read( m_handle, m_buffer, m_buffer_size, &done );

	m_leftSamples = done / 2;
	m_offset = 0;

	if( err != MPG123_OK )
		return 0;
	else
		return done;
}

int Mpg123Decoder::read()
{
	short* target = (short*)m_data;
	int numSamples = m_size / 2;    

	int idx = 0;
	while( idx != numSamples )
	{
		if( m_leftSamples > 0 )
		{
			short* src = ((short*)m_buffer) + m_offset;
			for( ; idx < numSamples && m_offset < (int)m_buffer_size / 2; m_leftSamples--, m_offset++, target++, src++, idx++ )
			{
				*target = *src;
			}
		}
		else
		{
			int result = readBuffer();
			if( result == 0 )
				return 0;
		}

	}

	if( idx > numSamples )
		return 0;

	return idx;
}

long Mpg123Decoder::skipSamples(long numSamples)
{

	long idx = 0;
	while( idx != numSamples )
	{
		if( m_leftSamples > 0 )
		{
			for( ; idx < numSamples && m_offset < (int)m_buffer_size / 2; m_leftSamples--, m_offset++, idx++ );
		}
		else
		{
			int result = readBuffer();
			if( result == 0 )
				return 0;
		}

	}

	if( idx > numSamples )
		return 0;

	return idx;
}

void Mpg123Decoder::close()
{
	free(m_buffer);
	cleanup(m_handle);
}

