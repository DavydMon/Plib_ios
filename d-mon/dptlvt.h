


#ifndef dptlvtH
#define dptlvtH

#include "dmain.h"

#include "dptlv.h"

#ifdef LINUX
namespace dmon {
#endif

template <dword _size>
class tDPrimitveTLV : public DPrimitveTLV
{
	private:

		byte data[_size];

	public:


		tDPrimitveTLV():DPrimitveTLV(data,_size){};
		tDPrimitveTLV(byte *value,dword value_len):DPrimitveTLV(data,_size,value,value_len){};
		tDPrimitveTLV(DPrimitveTLV& value):DPrimitveTLV(data,_size,value){};
};

typedef tDPrimitveTLV <64>   tDPrimitveTLV64;
typedef tDPrimitveTLV <128>  tDPrimitveTLV128;
typedef tDPrimitveTLV <256>  tDPrimitveTLV256;
typedef tDPrimitveTLV <512>  tDPrimitveTLV512;
typedef tDPrimitveTLV <1024> tDPrimitveTLV1024;
typedef tDPrimitveTLV <2048> tDPrimitveTLV2048;
typedef tDPrimitveTLV <4096> tDPrimitveTLV4096;
typedef tDPrimitveTLV <8192> tDPrimitveTLV8192;

#ifdef LINUX
}
#endif


#endif
