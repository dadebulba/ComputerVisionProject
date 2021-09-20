#ifndef CODECCONTEXT_H_
#define CODECCONTEXT_H_

#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>

extern "C" {
	#include <libavformat/avformat.h>
}

#include "Frame.h"
#include "Packet.h"
#include "Codec.h"

namespace ffpp
{

class CodecContext
{
	protected:
		boost::shared_ptr<AVCodecContext> avp;
		
		void OpenDecoder();
		void OpenEncoder();
		static void Close(AVCodecContext* _avp);
//		int avcodec_decode_video	(AVCodecContext * avctx, AVFrame * picture, int * 	got_picture_ptr, uint8_t * 	buf, int buf_size);
		
	public:
		CodecContext(AVCodecContext *avp);

		boost::optional<Frame> DecodeFrame(Packet& pkt);
		boost::optional<Packet> EncodeFrame(const Frame& frame);

		AVCodecContext* get();
		const AVCodecContext* get() const;
		
		int GetWidth() const;
		int GetHeight() const;
		int GetChannels() const;
		enum PixelFormat GetPixelFormat() const;
        int GetBitrate() const;
		
		void SetWidth(int width);
		void SetHeight(int height);
		void SetChannels(int channels);
		void SetPixelFormat(enum PixelFormat pix_fmt);
        void SetBitrate(int bitrate);
		
		Codec GetDecoder() const;
		Codec GetEncoder() const;
};

}

#endif /*CODECCONTEXT_H_*/
