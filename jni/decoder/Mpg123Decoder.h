#include "decoder.h"
#include "mpg123/mpg123.h"


#ifndef _MPG123DECODER_H_
#define _MPG123DECODER_H_

class Mpg123Decoder: public decoder
{
public:
	~Mpg123Decoder();
	bool open(char *file);
	void close();
	int read();
//	void rewind();
//	long position();
	long skipSamples(long numSamples);

private:
	int readBuffer();
	void cleanup(mpg123_handle* handle);
	mpg123_handle* m_handle;
	int m_leftSamples;
	int m_offset;
	size_t m_buffer_size;
	unsigned char* m_buffer;
};

#endif /* _MPG123DECODER_H_ */
