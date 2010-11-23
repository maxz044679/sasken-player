/**
* Copyright 2010 Mario Zechner (contact@badlogicgames.com)
* 
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
* 
*   http://www.apache.org/licenses/LICENSE-2.0
* 
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#include "VorbisDecoder.h"
#include "vorbis/ogg.h"
#include "vorbis/ivorbiscodec.h"
#include "vorbis/ivorbisfile.h"
#include <stdlib.h>
#include <stdio.h>
#include <android/log.h>

#define DEBUG_TAG "NDK_Vorbis_Dec"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, DEBUG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, DEBUG_TAG, __VA_ARGS__)


VorbisDecoder::VorbisDecoder()
             : m_ogg(NULL),
               m_bitstream(0)
{

}


VorbisDecoder::~VorbisDecoder()
{
	close();
}


bool VorbisDecoder::open(char *fileString)
{
	OggVorbis_File* ogg = (OggVorbis_File *) malloc(sizeof(OggVorbis_File));
	FILE* file = fopen(fileString, "rb" );

    LOGI("Open [%s]", fileString);

	if( file == 0 )
	{
		free (ogg);
		return false;
	}

	if( ov_open( file, ogg, NULL, 0 ) != 0 )
	{
        LOGE("Error opening file");
		fclose( file );
		free (ogg);
		return false;
	}

	vorbis_info *info = ov_info( ogg, -1 );
	int channels = info->channels;
	int rate = info->rate;
	long length = (long)(ov_time_total(ogg, -1 )*rate);

	m_ogg = ogg;
	m_channels = channels;
	m_rate = rate;
	m_length = length;

	return true;
}

int VorbisDecoder::read()
{
	char* samples = (char*)m_data;
	int toRead = m_size;
	int readed = 0;

	while( readed != toRead )
	{
		int ret = ov_read( m_ogg, samples + readed, toRead - readed, &m_bitstream );
		if( ret == OV_HOLE )
			continue;
		if( ret == OV_EBADLINK || ret == OV_EINVAL || ret == 0 )
			return readed;
		readed+=ret;
	}

	return readed;
}

/*
static char buffer[10000];

int VorbisDecoder_skipSamples(long handle, int numSamples)
{
	OggFile* file = (OggFile*)handle;
	int toRead = 2 * numSamples;
	int read = 0;

	while( read != toRead )
	{
		int ret = ov_read( file->ogg, buffer, (toRead - read)>10000?10000:(toRead-read), &file->bitstream );
		if( ret == OV_HOLE )
			continue;
		if( ret == OV_EBADLINK || ret == OV_EINVAL || ret == 0 )
			return read / 2;
		read+=ret;
	}

	return read / 2;
}
*/

void VorbisDecoder::close()
{
	if (m_ogg)
	{
		ov_clear(m_ogg);
		free(m_ogg);
		m_ogg = NULL;
	}
}
