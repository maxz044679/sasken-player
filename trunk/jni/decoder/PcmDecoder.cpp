#include "PcmDecoder.h"
#include <android/log.h>

#define DEBUG_TAG "NDK_Pcm_Dec"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, DEBUG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, DEBUG_TAG, __VA_ARGS__)

#define HEADER_SIZE 40

PcmDecoder::PcmDecoder()
	: m_pcm(NULL)
{
}

PcmDecoder::~PcmDecoder()
{

	// we close on the destructor
	close();
}

unsigned long PcmDecoder::value(char *ptr, size_t num) {
	unsigned long value = 0;
	int i;
	for (i=num-1; i>=0; i--) {
		value = value<< 8 | ((long)ptr[i] & 0xFF);
	}
	return value;
}

bool PcmDecoder::pcm_open()
{
	char buffer[HEADER_SIZE];

	// obtain file size:
	fseek (m_pcm , 0 , SEEK_END);
	m_length = (ftell (m_pcm) - HEADER_SIZE) >> 1;
	if (m_length < 0) {
		LOGE("file too small to be a valid wav file");
		return false;
	}

	// rewind the to the beginning
	fseek (m_pcm , 0L , SEEK_SET);

	// read the buffer
	fread(buffer, sizeof(char), HEADER_SIZE, m_pcm);

	// Verify the Chunk descriptor
	if (memcmp(buffer, "RIFF", 4) != 0) {
		buffer[4] = 0;
		LOGE("Wrong chunk descriptor [%s]", buffer);
		return false;
	}

	// Check the file format
	if (memcmp(buffer + 8, "WAVE", 4) != 0) {
		buffer[12] = 0;
		LOGE("Wrong file format [%s]", buffer+8);
		return false;
	}

	// Check the fmt subchunk
	if (memcmp(buffer+12, "fmt ", 4) != 0) {
		buffer[16] = 0;
		LOGE("Wrong fmt subchunk [%s]", buffer+8);
		return false;
	}

	// Check the data subchunk
	if (memcmp(buffer+36, "data", 4) != 0) {
		LOGE("Wrong data subchunk [%c%c%c%c]", buffer[36], buffer[37], buffer[38], buffer[39]);
		return false;
	}
	
	// the subchunk size should be 16
	if (value(buffer + 16,4) != 16) {
		buffer[16] = 0;
		LOGE("invalid subchunk1Size [%ld]", value(buffer + 16,4));
		return false;
	}

	// the audio format should be 1
	if (value(buffer + 20, 2) != 1) {
		buffer[16] = 0;
		LOGE("invalid Audio Format [%ld]", value(buffer + 20, 2));
		return false;
	}

	// the bytes per sample should be 16
	if (value(buffer + 34, 2) != 16) {
		buffer[16] = 0;
		LOGE("invalid BitsPerSample [%ld]", value(buffer + 34, 2));
		return false;
	}

	// Read the NumChannels
	m_channels = value(buffer + 22, 2);
	if (m_channels < 1 || m_channels > 2) {
		LOGE("invalid number of channels [%d]", m_channels);
		return false;
	}

	// Read the SampleRate
	m_rate = value(buffer + 24, 4);
	if (m_rate < 8000 || m_rate > 48000) {
		LOGE("invalid sample rate [%d]", m_rate);
		return false;
	}

	// The Block Align should be twice the number of channels 
	if (value(buffer + 32, 2) != (unsigned long)m_channels*2) {
		buffer[16] = 0;
		LOGE("invalid Block Align [%ld]", value(buffer + 32, 2));
		return false;
	}

	// looks like a valid pcm header
	return true;
}

bool PcmDecoder::open(char *file)
{
	// log the name
	LOGI("Open [%s]", file);

	m_pcm = fopen(file, "rb" );

	if( m_pcm == NULL )
	{
		return 0;
	}

	if(!pcm_open())
	{
		close();
		return 0;
	}

	return 1;
}

int PcmDecoder::read()
{
	return fread(m_data, sizeof(char), m_size, m_pcm);
}

void PcmDecoder::close()
{
	if (m_pcm) {
		fclose (m_pcm);
		m_pcm = NULL;
	}
}

void PcmDecoder::rewind()
{
	fseek (m_pcm , (long)HEADER_SIZE , SEEK_SET);
}

long PcmDecoder::position()
{
	long pos;
	pos = (ftell(m_pcm) - HEADER_SIZE) >> 1;
	return pos;
}

long PcmDecoder::skipSamples(long samples)
{
	return fseek (m_pcm , samples * 2 , SEEK_CUR );
}
