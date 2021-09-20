#ifndef OUTPUTFORMAT_H_
#define OUTPUTFORMAT_H_

#ifdef __cplusplus
extern "C" {
	#include <libavformat/avformat.h>
}
#endif

namespace ffpp
{

class OutputFormat
{
	protected:
		const AVOutputFormat *avp;
	
	public:
		OutputFormat(const AVOutputFormat *avp = 0);
		OutputFormat(const char* name, const char* filename);

		const char* GetName() const;

		const AVOutputFormat* get() const;
};
AVOutputFormat *guess_format(const char *short_name, const char *filename, const char *mime_type);

}

#endif /*OUTPUTFORMAT_H_*/
