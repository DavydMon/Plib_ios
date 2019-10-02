/*
   $Id: 
   $Change: 134223 $
   $DateTime: 2012/05/10 18:43:02 $
   $Author: DavidMon $
*/


#ifndef dptlvH
#define dptlvH

#include "dmain.h"

#ifdef LINUX
namespace dmon {
#endif

enum DPrimitveTLVState{DPrimitveTLVState_empty,DPrimitveTLVState_ok,DPrimitveTLVState_error};

#pragma pack(push,mystruct,1)
struct DwTwLV
{
	word tag;
	word len;
	void Clear()
	{
		tag = 0xffff;
		len = 0;
	}
};
#pragma pack(pop,mystruct)

class DPrimitveTLV
{
	protected:

		byte *p;
		dword l;
		dword size;
		dword Size();

		DPrimitveTLVState state;
		bool error;

		template <class Type, class Len>
		bool TAddTLV(Type type, void *value, Len len)
		{
			if (!p)
				return false;
			if (len && !value)
				return false;
			if ((l + sizeof(Type) + sizeof(Len) + len) > size)
				return false;

			*(Type*)(p + l) = type;
			l += sizeof(Type);
			*(Len*)(p + l) = len;
			l += sizeof(Len);

			if (len)
			{
				memcpy(p + l, value, len);
				l += len;
			}

			return true;
		}


		bool _AddTLVV(byte type,void *value1,byte len1,void *value2,byte len2);
		bool _AddTLVVV(byte type,void *value1,byte len1,void *value2,byte len2,void *value3,byte len3);



		bool _AddSwTwLV(word type,byte status,void *value,word len);
		bool _AddwTwLV_AsCString(word type,void *value,word len);
		bool _AddSwTwLV_AsCString(word type,byte status,void *value,word len);

	public:

		bool  Init(byte *data,dword max_size);
		bool  Init(byte *data,dword max_size,dword shift);
		void  Clear();
		byte* GetP();
		byte* GetPS();
		byte* GetPTLV();
		dword Len();
		dword EmptySize();
		DPrimitveTLVState GetState();
		bool GetError();

		operator byte*();

		bool SetPrimitve     (byte  primitve);
        bool SetPrimitveWord (word primitve);
		bool SetPrimitveDword(dword  primitve);
		byte* SetHeader(dword len);
		bool Seek(dword len);
		bool GetPrimitve(byte *primitve);
        bool AddByte (byte  value);
        bool AddWord (word value);
		bool AddDword(dword  value);
		bool AddBin  (void *value,dword len);
		bool AddTLV  (byte  type,void *value,byte  len);
		bool AddTLVV (byte type,void *value1,byte len1,void *value2,byte len2);
		bool AddTLVVV(byte type,void *value1,byte len1,void *value2,byte len2,void *value3,byte len3);

		bool AddTLV_byte (byte type,byte value);
		bool AddTLV_dword(byte type,dword value);

		bool AddTwLV(byte type,void *value,word len);
		bool AddwTwLV(word type,void *value,word len);
		bool AddSwTwLV(word type,byte status,void *value,word len);
		bool AddwTwLV_AsCString(word type,void *value,word len);
		bool AddSwTwLV_AsCString(word type,byte status,void *value,word len);

		bool AddwTdLV(word type, void *value, dword len);
		bool AdddTdLV(dword type, void *value, dword len);

		DPrimitveTLV();
		DPrimitveTLV(byte *data,dword max_size);
		DPrimitveTLV(byte *data,dword max_size,byte *value,dword value_len);
		DPrimitveTLV(byte *data,dword max_size,DPrimitveTLV& value);
};

#ifdef LINUX
}
#endif


#endif
