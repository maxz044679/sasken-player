#include "decoder.h"
#include "vorbis/ivorbisfile.h"

#ifndef _VORBISDECODER_H_
#define _VORBISDECODER_H_

class VorbisDecoder: public decoder
{
public:
	VorbisDecoder();
	~VorbisDecoder();
	
	bool open(char *file);
	void close();
	int read();
//	void rewind();
//	long position();
//	long skipSamples(long numSamples);
	
private:

	OggVorbis_File* m_ogg;
	int m_bitstream;
	
};

#endif /* _VORBISDECODER_H_ */
