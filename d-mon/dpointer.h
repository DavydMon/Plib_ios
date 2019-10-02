#ifndef	dpointerH
#define	dpointerH

#include "dmain.h"

#ifdef LINUX
namespace dmon {
#endif

template <typename Type,typename Len>
struct DPointerT
{
	byte *msg;
	Len len;
	inline void Clear()
	{
		msg = 0;
		len = 0;
	}
	inline void Set(byte *_msg,Len _len)
	{
		msg = _msg;
		len = _len;
	}
	bool GetParam(Type param_name,byte *_msg,dword _len)
	{
		Type pn;
		Len  pl;
		while(_len)
		{
			if(_len < (sizeof(Type) + sizeof(Len)))
				break;
			
			pn = *(Type*)_msg;
			_msg += sizeof(Type);
			_len -= sizeof(Type);
			
			pl = *(Len*)_msg;
			_msg += sizeof(Len);
			_len -= sizeof(Len);
			if(_len < (dword)pl)
				break;

			if(pn == param_name)
			{
				msg = _msg;
				len = pl;
				return true;
			}

			_msg += pl;
			_len -= pl;
		}

		return false;
	}
	bool Exist()
	{
		return (msg != 0) && (len != 0);
	}
};

template <typename Type,typename Len>
struct DPointerZeroT : DPointerT <Type,Len>
{
	DPointerZeroT()
	{
		DPointerT <Type,Len>::Clear();
	}
};

typedef DPointerT     <byte,byte>   DPointerByte;
typedef DPointerZeroT <byte,byte>   DPointerZByte;
typedef DPointerT     <word,word>   DPointerWord;
typedef DPointerZeroT <word,word>   DPointerZWord;
typedef DPointerT     <dword,dword> DPointerDword;
typedef DPointerZeroT <dword,dword> DPointerZDword;
typedef DPointerT     <qword,qword> DPointerQword;
typedef DPointerZeroT <qword,qword> DPointerZQword;

#ifdef LINUX
}
#endif



#endif

