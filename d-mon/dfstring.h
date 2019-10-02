


#ifndef dfstringH
#define dfstringH

#include <string.h>
#include "dmain.h"
#include "dstr.h"


#include "dpstring.h"

#ifdef LINUX
#include <cstring>
#include <algorithm>

namespace dmon {
#endif



#define DFixStringMaxLen 0xfffffffe

template <dword _size>
class DFixString : public DPString
{
	private:

		char	str_[_size+1];

	public:

#ifndef LINUX
		DFixString& operator = (char* string);



#else
		DFixString& operator = (char* string)
		{
			size_t _len;

			if(!string)
			{
				str_[0] = 0;
				len = 0;
			}
			else
			{
				_len = strlen(string);
				if( _len > _size)
				{
					str_[0] = 0;
					len = 0;
				}
				else
				{
					memcpy(str_, string, _len);
					len = (dword)_len;
					str_[len] = 0;
				}
			}

			return *this;
		}


















#endif
		DFixString& operator = (DPString& string)
		{
			dword l = string.Len();
			if( l > _size)
			{
				str_[0] = 0;
				len = 0;
			}
			else
			{
				memcpy(str_, string, l);
				len = l;
				str_[len] = 0;
			}

			return *this;
 		}
		DFixString& operator = (DFixString& other)
		{
			dword l = other.Len();
			if( l > _size)
			{
				str_[0] = 0;
				len = 0;
			}
			else
			{
				memcpy(str_, other, l);
				len = l;
				str_[len] = 0;
			}

			return *this;
		}

		DFixString():DPString(str_, _size + 1){};
		DFixString(char symbol):DPString(str_, _size + 1, symbol){};
		DFixString(char *string):DPString(str_, _size + 1, string){};
		DFixString(char *string, dword _len):DPString(str_, _size + 1, string, _len){};
		DFixString(DPString& string):DPString(str_, _size + 1, string){};
};

#ifndef LINUX
template <dword size>
DFixString<size>& DFixString<size>::operator =(char* string)
{
    DPString::operator = (string);
	return *this;
};



















#endif

template <char Type[]>
class DFixCString : public DPString
{
	public:


		DFixCString(char *s):DPString(s,(dword)sizeof(Type),(dword)sizeof(Type)-1){};
};

typedef DFixString<15>    DFixString15;
typedef DFixString<16>    DFixString16;
typedef DFixString<31>    DFixString31;
typedef DFixString<32>    DFixString32;
typedef DFixString<63>    DFixString63;
typedef DFixString<64>    DFixString64;
typedef DFixString<127>   DFixString127;
typedef DFixString<128>   DFixString128;
typedef DFixString<256>   DFixString256;
typedef DFixString<255>   DFixString255;
typedef DFixString<511>   DFixString511;
typedef DFixString<512>   DFixString512;
typedef DFixString<1023>  DFixString1023;
typedef DFixString<1024>  DFixString1024;
typedef DFixString<2047>  DFixString2047;
typedef DFixString<2048>  DFixString2048;
typedef DFixString<4095>  DFixString4095;
typedef DFixString<4096>  DFixString4096;
typedef DFixString<8191>  DFixString8191;
typedef DFixString<8192>  DFixString8192;
typedef DFixString<16383> DFixString16383;
typedef DFixString<16384> DFixString16384;
typedef DFixString<32767> DFixString32767;
typedef DFixString<32768> DFixString32768;
typedef DFixString<65535> DFixString65535;
typedef DFixString<65536> DFixString65536;

#ifdef LINUX
}
#endif

#endif

