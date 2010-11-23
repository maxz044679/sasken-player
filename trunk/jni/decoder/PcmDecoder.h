#include "decoder.h"
#include <stdio.h>

#ifndef _PCMDECODER_H_
#define _PCMDECODER_H_

class PcmDecoder: public decoder
{
public:
	PcmDecoder();
	~PcmDecoder();
	bool open(char *file);
	void close();
	int read();
	void rewind();
	long position();
	long skipSamples(long samples);
private:
	unsigned long value(char *ptr, size_t num);
	bool pcm_open();
	FILE* m_pcm;
};

#endif /* _PCMDECODER_H_ */
