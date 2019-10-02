#ifndef dttablepointH
#define dttablepointH

#include <string.h>

#include "dmain.h"
#include "dpointer.h"


#ifdef LINUX
namespace dmon {
#endif

template <class Type,class Len,dword _size>
class tDTablePointer
{
	private:

	public:

		DPointerT <Type,Len> data[_size];
        dword count;

		void Clear()
		{
            count = 0;
            memset(data,0,sizeof(DPointerT <Type,Len>) * _size);
		}

		bool Parse(byte *msg,dword len)
		{
			Type pn;
			Len  pl;

			if(!msg)
				return false;


			while(len >= (sizeof(Type) + sizeof(Len)))
			{


				
				pn = *(Type*)msg;
				msg += sizeof(Type);
				len -= sizeof(Type);
				
				pl = *(Len*)msg;
				msg += sizeof(Len);
				len -= sizeof(Len);
				if(len < (dword)pl)
					break;

				if(pn < _size)
				{
					data[pn].msg = msg;
					data[pn].len = pl;
				}

				msg += pl;
				len -= pl;
			}

			return true;
		}

		tDTablePointer()
		{
			Clear();
		};

};

template <_dword _size>
class tDTablePointerByte : public tDTablePointer <byte,byte,_size>
{
};

template <_dword _size>
class tDTablePointerWord : public tDTablePointer <word,word,_size>
{
};

template <_dword _size>
class tDTablePointerDword : public tDTablePointer <dword,dword,_size>
{
};


template <_dword _size>
class tDTablePointerTWordLDword : public tDTablePointer <word,dword,_size>
{
};

#define DTP_PARAMETER(d,n) d.data[(dword)n].msg,d.data[(dword)n].len
#define DTP_PARAMETERB(d,n) d.data[(dword)n].msg,(byte)d.data[(dword)n].len
#define DTP_PPARAMETER(n) p.data[(dword)n].msg,p.data[(dword)n].len
#define DTP_PPARAMETERB(n) p.data[(dword)n].msg,(byte)p.data[(dword)n].len


#ifdef LINUX
}
#endif




#endif
