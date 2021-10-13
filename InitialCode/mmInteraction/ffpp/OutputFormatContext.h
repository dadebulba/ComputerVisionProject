#ifndef OUTPUTFORMATCONTEXT_H_
#define OUTPUTFORMATCONTEXT_H_

#include "FormatContext.h"
#include "Packet.h"
#include "Codec.h"

#ifdef __cplusplus
extern "C" {
	#include <libavformat/avformat.h>
}
#endif

namespace ffpp
{

class OutputFormatContext: public FormatContext
{
	protected:
		bool header_written;
		
		static void Destruct(AVFormatContext* _avp);
		Stream AddStream(enum AVMediaType type);
		Stream AddStream(enum AVMediaType type, enum CodecID codec_id);
	
	public:
	    OutputFormatContext(const char* filename, const OutputFormat& format);
	    
		OutputFormat GetFormat() const;

		Stream AddVideoStream();
		Stream AddVideoStream(const Codec& codec);
		
		void WriteHeader();
		void WritePacket(const Packet& pkt);
};
//AVFormatContext *av_alloc_format_context(void);
}

#endif /*OUTPUTFORMATCONTEXT_H_*/
