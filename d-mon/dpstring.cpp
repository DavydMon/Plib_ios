/*
   $Id:$
   $Change:$
   $DateTime:$
   $Author:$
*/


#include <stdlib.h>

#include "dmain.h"
#include "dsymbconv.h"
#include "dpstring.h"

#ifdef LINUX
namespace dmon {
#endif


DPString::DPString(char *_str,dword _size)
{
    str    = _str;
    size   = _size;
    str[0] = 0;
	len    = 0;


}

DPString::DPString(char *_str,dword _size,dword _len)
{
    str    = _str;
    size   = _size;

    if( _len >= size)
    {
        str[0] = 0;
        len = 0;
    }
    else
    {
        len = _len;
        str[len] = 0;
    }


}

DPString::DPString(char *_str,dword _size,char symbol)
{
    str    = _str;
    size   = _size;
    str[0] = symbol;
    str[1] = 0;
	len    = 1;


}

DPString::DPString(char *_str,dword _size,char* string)
{
	size_t _len;

    str  = _str;
    size = _size;

	if(!string)
    {
    	str[0] = 0;
		len = 0;
    }
	else
    {
    	_len = strlen(string);
    	if( _len >= size)
        {
    		str[0] = 0;
			len = 0;
        }
    	else
		{
			memcpy(str,string,_len);
			len = (dword)_len;
    		str[len] = 0;
        }
    }


}

DPString::DPString(char *_str,dword _size,char* string,dword _len)
{
    str  = _str;
    size = _size;

	if(!string)
    {
    	str[0] = 0;
		len = 0;
    }
	else
    {
    	if( _len >= size)
        {
    		str[0] = 0;
			len = 0;
        }
    	else
		{

			    memcpy(str,string,_len);
			len = _len;
    		str[len] = 0;
        }
    }


}

DPString::DPString(char *_str,dword _size,DPString& string)
{
    str  = _str;
    size = _size;

	if( string.len >= size)
    {
    	str[0] = 0;
        len = 0;
    }
    else
    {
    	memcpy(str,string.str,string.len);
        len = string.len;
        str[len] = 0;
    }


}

dword DPString::Size()
{
	return size;
}

dword DPString::Length()
{
	return len;
}

dword DPString::Len()
{
	return len;
}

bool DPString::SetLength(dword length)
{
	if(length > len)
    	return false;

    len = length;
    str[len] = 0;



	return true;
}

bool DPString::SetLen(dword length)
{
	if(length > len)
    	return false;

    len = length;
    str[len] = 0;



	return true;
}

void DPString::Clear()
{
    str[0] = 0;
	len = 0;


}

char* DPString::c_str()
{
	return str;
}

bool DPString::Set(char character)
{
    if(!size)
    	return false;

    str[0] = character;
    str[1] = 0;
	len = 1;



	return true;
}

bool DPString::Set(const char *string)
{
	size_t _len;

	if(!string)
		return false;

    _len = strlen(string);
    if( _len >= size)
    	return false;

	memcpy(str,string,_len);
	len = (dword)_len;
    str[len] = 0;



	return true;
}

bool DPString::Set(const char *string,dword _len)
{
	if(!string)
		return false;
    if( _len >= size)
    	return false;

	memcpy(str,string,_len);
	len = _len;
    str[len] = 0;



	return true;
}

bool DPString::Set(DPString& string)
{
    if( string.len >= size)
    	return false;

	memcpy(str,string.str,string.len);
	len = string.len;
    str[len] = 0;



	return true;
}

bool DPString::Add(const char *string)
{
	size_t _len;

	if(!string)
		return false;

    _len = strlen(string);
    if( (len + _len) >= size)
    	return false;

	memcpy(str+len,string,_len);
	len += (dword)_len;
    str[len] = 0;



	return true;
}

bool DPString::Add(char character)
{
    if( (len + 1) >= size)
    	return false;

    str[len] = character;
	len += 1;
    str[len] = 0;



	return true;
}

bool DPString::Add(const char *string,dword _len)
{
	if(!string)
		return false;
    if( (len + _len) >= size)
    	return false;

	memcpy(str+len,string,_len);
	len += _len;
    str[len] = 0;



	return true;
}

bool DPString::Add(DPString& string)
{
    if( (len + string.len) >= size)
    	return false;

	memcpy(str + len,string.str,string.len);
	len += string.len;
    str[len] = 0;



	return true;
}




bool DPString::AddW(short *string,dword _len)
{
    unsigned short symbol;

	if(!string)
		return false;
    if( (len + _len) >= size)
    	return false;

    for(dword i=0;i<_len;i++)
    {
        symbol = string[i];
        if(symbol < 0x80)
            str[len] = (unsigned char)symbol;
        else
        {
            if( (symbol >= 0x400) && (symbol <= 0x45f) )
                str[len] = dtable_UCS2_win1251[symbol - 0x400];
            else
                str[len] = '?';
        }
		len++;
    }
    str[len] = 0;



	return true;
}

bool DPString::AddWSwap(short *string,dword _len)
{
    unsigned short symbol;
    unsigned short b1,b2;

	if(!string)
		return false;
    if( (len + _len) >= size)
    	return false;

	for(dword i=0;i<_len;i++)
    {
        symbol = string[i];

        b1 = symbol >> 8;
        b2 = symbol << 8;

        symbol = b1 | b2;

        if(symbol < 0x80)
            str[len] = (unsigned char)symbol;
        else
        {
            if( (symbol >= 0x400) && (symbol <= 0x45f) )
                str[len] = dtable_UCS2_win1251[symbol - 0x400];
            else
                str[len] = '?';
        }
		len++;
	}
	str[len] = 0;



	return true;
}

bool DPString::AddWAsUTF8(short *string,dword _len)
{
	unsigned short symbol;
	byte buff[16];
	dword l;

	if(!string)
		return false;



	for(dword i=0;i<_len;i++)
	{
		symbol = string[i];
		l = dconvert_UCS2_to_UTF8(buff,sizeof(buff),&symbol,2);
		if( (len + l) >= size)
			return false;
		memcpy(str + len,buff,l);
		len += l;
	}
	str[len] = 0;

	

	return true;
}

bool DPString::AddWAsUTF8b(short *string,dword _len)
{
	unsigned short symbol;
	byte buff[16];
	dword l;

	if(!string)
		return false;



	for(dword i=0;i<_len;i++)
	{
		symbol = string[i];
		l = dconvert_UCS2_to_UTF8_swap(buff,sizeof(buff),&symbol,2);
		if( (len + l) >= size)
			return false;
		memcpy(str + len,buff,l);
		len += l;
	}
	str[len] = 0;

	

	return true;
}

bool DPString::Get(char *string)
{
	if(!string)
		return false;

	memcpy(string,str,len+1);

	return true;
}

bool DPString::Get(char *string,dword max_len)
{
	if(!string)
		return false;
    if(max_len < (len + 1) )
    	return false;

	memcpy(string,str,len+1);

	return true;
}

bool DPString::Cmp(char *string)
{
	size_t _len;

	if(!string)
		return false;

    _len = strlen(string);
    if(len != _len)
    	return false;

	return memcmp(str,string,len) == 0;
}

bool DPString::Cmp(char *string,dword _len)
{
	if(!string)
		return false;
    if(len != _len)
    	return false;

	return memcmp(str,string,len) == 0;
}

bool DPString::Cmp(DPString& string)
{
    if(len != string.len)
    	return false;

	return memcmp(str,string.str,len) == 0;
}


char DPString::operator [](dword index)
{
	if(!str || (index >= len))
		return 0;
	
	return str[index];
}

DPString::operator char*()
{
	return str;
}

DPString& DPString::operator =(char character)
{
    if(size)
	{
		str[0] = character;
		str[1] = 0;
		len = 1;


	}

	return *this;
};

#ifndef LINUX
DPString& DPString::operator =(char* string)
{
	size_t _len;

	if(!string)
    {
    	str[0] = 0;
        len = 0;
    }
    else
	{
    	_len = strlen(string);
    	if( _len >= size)
        {
        	str[0] = 0;
            len = 0;
        }
        else
        {
			memcpy(str,string,_len);
			len = (dword)_len;
    		str[len] = 0;
        }
    }


	return *this;
};

DPString& DPString::operator =(DPString& string)
{
	if( string.len >= size)
    {
    	str[0] = 0;
        len = 0;
    }
    else
    {
    	memcpy(str,string.str,string.len);
        len = string.len;
        str[len] = 0;
    }



	return *this;
};
#endif

/*
DPString DPString::operator +(char* string)
{
	DPString str_temp(str,len);

    str_temp.Add(string);

	return str_temp;
}


DPString DPString::operator +(DPString& string)
{
	DPString str_temp(str,len);

    str_temp.Add(string);

	return str_temp;
}
*/

DPString& DPString::operator +=(char character)
{
    if( (len + 1) >= size)
    {
        str[0] = 0;
        len = 0;

    }
    else
    {
    	str[len] = character;
		len += 1;
    	str[len] = 0;

    }

	return *this;
}

DPString& DPString::operator +=(char* string)
{
	size_t _len;

	if(!string)
    {
    	str[0] = 0;
        len = 0;

    }
    else
    {
    	_len = strlen(string);
    	if( (len + _len) >= size)
        {
        	str[0] = 0;
            len = 0;

        }
        else
        {
			memcpy(str+len,string,_len);
			len += (dword)_len;
    		str[len] = 0;

        }
    }
 
	return *this;
}

DPString& DPString::operator +=(DPString& string)
{
	if( (len + string.len) >= size)
    {
    	str[0] = 0;
        len = 0;

    }
    else
    {
    	memcpy(str+len,string.str,string.len);
        len += string.len;
        str[len] = 0;

    }

	return *this;
}


bool DPString::operator ==(char* string)
{
	size_t _len;

	if(!string)
    	return false;

    _len = strlen(string);
    if(len != _len)
    	return false;

	return memcmp(str,string,len) == 0;
}

bool DPString::operator ==(DPString& string)
{
    if(len != string.len)
    	return false;

	return memcmp(str,string.str,len) == 0;
}

bool DPString::operator !=(char* string)
{
	size_t _len;

	if(!string)
    	return false;

    _len = strlen(string);
    if(len != _len)
    	return false;

	return memcmp(str,string,len) != 0;
}

bool DPString::operator !=(DPString& string)
{
    if(len != string.len)
    	return false;

	return memcmp(str,string.str,len) != 0;
}


bool DPString::ChangeChar(dword index,char character)
{
	if(index >= len)
		return false;

	str[index] = character;
	
	return true;
}

bool DPString::AddDec(dword value)
{
	byte l;

	l = ddtoafex(value,str+len,size-len+1);
    len += l;




	return (l != 0);
}

bool DPString::AddDecEx(dword value,byte count,byte symbol,byte end_space_count)
{
	byte l;

	l = ddtoaex(value,str+len,size-len+1,count,symbol,end_space_count);
    len += l;


	return (l != 0);
}

bool DPString::AddQDec(qword value)
{
	byte l;

	l = dqtoafex(value,str+len,size-len+1);
    len += l;




	return (l != 0);
}

bool DPString::AddQDecEx(dword value,byte count,byte symbol,byte end_space_count)
{
	byte l;

	l = dqtoaex(value,str+len,size-len+1,count,symbol,end_space_count);
    len += l;


	return (l != 0);
}

bool DPString::AddLong(sdword value)
{
	byte l;

	l = dltoafex(value,str+len,size-len+1);
    len += l;


	return (l != 0);
}

bool DPString::AddLongEx(sdword value,byte end_space_count)
{
	byte l;

	l = dltoaex(value,str+len,size-len+1,end_space_count);
    len += l;


	return (l != 0);
}

bool DPString::AddSQDec(sqword value)
{
	byte l;

	l = dsqtoafex(value,str+len,size-len+1);
    len += l;


	return (l != 0);
}

bool DPString::AddSQDecEx(sqword value,byte end_space_count)
{
	byte l;

	l = dsqtoaex(value,str+len,size-len+1,end_space_count);
    len += l;


	return (l != 0);
}

bool DPString::AddDouble(double value,byte dl)
{
    dword old_len;
    qword  qv;
    double tq;
    dword  td = 1;




    old_len = len;

    qv = (qword)value;

    if(!AddQDec(qv))
	{

        return false;
	}

    if(!dl)
	{

        return true;
	}

    tq = value - double(qv);
    for(byte i = 0;i<dl;i++)
        td *= 10;
    tq *= td;

    qv = (qword)tq;
    if(!qv)
	{

        return true;
	}

	if( (len + 1) > size)
    {
        len = old_len;

    	return false;
    }

    str[len] = '.';
    len += 1;
    str[len] = 0;

    if(!AddQDec(qv))
    {
		len = old_len;

		return false;
    }



	return true;
}

bool DPString::AddHex(dword value,byte count)
{
	byte l;

	l = ddtoahexfex(value,str+len,size-len+1,count);
    len += l;


	return (l != 0);
}

bool DPString::AddHexEx(dword value,byte count,byte symbol,byte end_space_count)
{
	byte l;

	l = ddtoahexex(value,str+len,size-len+1,count,symbol,end_space_count);
    len += l;


	return (l != 0);
}

bool DPString::AddQHex(qword value,byte count)
{
	byte l;

	l = dqtoahexfex(value,str+len,size-len+1,count);
    len += l;


	return (l != 0);
}

bool DPString::AddQHexEx(qword value,byte count,byte symbol,byte end_space_count)
{
	byte l;

	l = dqtoahexex(value,str+len,size-len+1,count,symbol,end_space_count);
    len += l;


	return (l != 0);
}

bool DPString::AddHexByte(byte value)
{
	if( (len+2) > size)
    	return false;

	ddtoahex_byte(value,str+len);
    len += 2;


	return true;
}

bool DPString::AddHexWord(word value)
{
	if( (len+4) > size)
    	return false;

	ddtoahex_word(value,str+len);
    len += 4;


	return true;
}

bool DPString::AddHexDword(dword value)
{
	if( (len + 8) > size)
    	return false;

	ddtoahex_dword(value,str+len);
    len += 8;


	return true;
}


bool DPString::AddHexQword(qword value)
{
	if( (len + 16) > size)
    	return false;

	dqtoahex_qword(value,str+len);
    len += 16;


	return true;
}


bool DPString::AddIP(dword ip)
{

	bool result = false;
	byte l;



	while(true)
	{
		l = ddtoafex(((byte*)&ip)[3],str+len,size-len+1);
		if(!l)
    		break;
		len += l;
		if( len >= size)
    		break;
		str[len++] = '.';

		l = ddtoafex(((byte*)&ip)[2],str+len,size-len+1);
		if(!l)
    		break;
		len += l;
		if( len >= size)
    		break;
		str[len++] = '.';

		l = ddtoafex(((byte*)&ip)[1],str+len,size-len+1);
		if(!l)
    		break;
		len += l;
		if( len >= size)
    		break;
		str[len++] = '.';

		l = ddtoafex(((byte*)&ip)[0],str+len,size-len+1);
		if(!l)
    		break;
		len += l;

		result = true;
		break;
	}



	return result;
}


bool DPString::AddIPSwap(dword ip)
{

	bool result = false;
	byte l;



	while(true)
	{
		l = ddtoafex(((byte*)&ip)[0],str+len,size-len+1);
		if(!l)
    		break;
		len += l;
		if( len >= size)
    		break;
		str[len++] = '.';

		l = ddtoafex(((byte*)&ip)[1],str+len,size-len+1);
		if(!l)
    		break;
		len += l;
		if( len >= size)
    		break;
		str[len++] = '.';

		l = ddtoafex(((byte*)&ip)[2],str+len,size-len+1);
		if(!l)
    		break;
		len += l;
		if( len >= size)
    		break;
		str[len++] = '.';

		l = ddtoafex(((byte*)&ip)[3],str+len,size-len+1);
		if(!l)
    		break;
		len += l;

		result = true;
		break;
	}



	return result;
}


bool DPString::AddIPPort(dword ip,word port)
{

	bool result = false;
	byte l;



	while(true)
	{
		if(!AddIP(ip))
     		break;

		if( len >= size)
     		break;
		str[len++] = ':';

		l = ddtoafex(port,str+len,size-len+1);
		if(!l)
     		break;
		len += l;

		result = true;
		break;
	}



	return result;
}


bool DPString::AddIPPortSwap(dword ip,word port)
{

	bool result = false;
	byte l;



	while(true)
	{
	   if(!AddIPSwap(ip))
    		break;

		if( len >= size)
    		break;
		str[len++] = ':';

		l = ddtoafex(port,str+len,size-len+1);
		if(!l)
    		break;
		len += l;

		result = true;
		break;
	}



	return result;
}


bool DPString::AddSpace(dword count)
{
	if( (len + count) >= size)
	{

    	return false;
	}

    for(dword i=0;i<count;i++)
		str[len++] = ' ';

    str[len] = 0;



	return true;
}

bool DPString::AddSpaceAlignLen(dword _size)
{
	if(_size <= len)
		return true;

	return AddSpace(_size - len);
}

bool DPString::AddSpaceTab(dword tab)
{
	dword count;

	if(tab < 2)
		return true;
	if(!len)
		return true;

	count = tab - (len % tab);

	return AddSpace(count);
}

void DPString::Trim()
{
	TrimRight();
    TrimLeft();
}

void DPString::TrimLeft()
{
	dword i;

    if(!len)
    	return;

	for(i=0;i<len;i++)
    {
    	if((byte)str[i] > 0x20)
        	break;
    }

    len -= i;

    memmove(str,str + i,len + 1);



}

void DPString::TrimRight()
{
	dword i;

    if(!len)
    	return;

	for(i=len-1;i!=0xffffffff;i--)
    {
    	if((byte)str[i] > 0x20)
        	break;
    }

    len = i + 1;

    str[len] = 0;
}


void DPString::TrimPath()
{
	dword i;

    if(!len)
    	return;

	for(i=len-1;i!=0xffffffff;i--)
    {
		if((str[i] == PATH_SEPARATOR_WIN) || (str[i] == PATH_SEPARATOR_NIX))
        	break;
    }

    len = i + 1;

    str[len] = 0;
}

void DPString::TrimQuotes(bool trim_space_before,bool trim_space_after)
{
	if(trim_space_before)
		Trim();

	if(len < 2)
		return;

	if( (str[0] != '"') || (str[len-1] != '"') )
		return;

	len -= 2;
	memmove(str,str + 1,len);
	str[len] = 0;

	if(trim_space_after)
		Trim();
}

bool DPString::TrimFileExt()
{
	dword i;
	bool f = false;

	if(!len)
		return false;

	for(i=len-1;i!=0xffffffff;i--)
	{
		if(str[i] == '.')
		{
			f = true;
			break;
		}
	}

	if(!f)
		return false;

	if(i == 0)
		len = 0;
	else
		len = i - 1;

	str[len] = 0;

	return true;
}


void DPString::UpperCase()
{
	byte s;

	if(!len)
		return;

	for(dword i=0;i<len;i++)
	{
		s = str[i];
		if( ((s >= 'a') && (s <= 'z')) || (s >= 0xe0) || (s == 0xb8) )
		{
			s -= 32;
			str[i] = s;
		}
	}

}

void DPString::LowerCase()
{
	byte s;

	if(!len)
		return;

	for(dword i=0;i<len;i++)
	{
		s = str[i];
		if( ((s >= 'A') && (s <= 'Z')) || (s >= 0xc0) || (s == 0xa8) )
		{
			s += 32;
			str[i] = s;
		}
	}

}
#if defined(ddateH)

bool DPString::AddDate(dword year,byte mon,byte day,char separator)
{
	byte l;

	l = ddatetostrxex(year,mon,day,str+len,size-len+1,separator);
    len += l;


	return (l != 0);
}

bool DPString::AddDate(ddate *date,char separator)
{
	byte l;

	l = ddatetostrxex(date,str+len,size-len+1,separator);
    len += l;


	return (l != 0);
}

bool DPString::AddDate2(dword year,byte mon,byte day,char separator)
{
	byte l;

	l = ddatetostrx2ex(year,mon,day,str+len,size-len+1,separator);
    len += l;


	return (l != 0);
}

bool DPString::AddDate2(ddate *date,char separator)
{
	byte l;

	l = ddatetostrx2ex(date,str+len,size-len+1,separator);
    len += l;


	return (l != 0);
}

bool DPString::AddDate_yymmdd(dword year,byte mon,byte day,char separator)
{
	byte l;

	l = ddatetostrxex_yymmdd(year,mon,day,str+len,size-len+1,separator);
    len += l;


	return (l != 0);
}

bool DPString::AddDate_yymmdd(ddate *date,char separator)
{
	byte l;

	l = ddatetostrxex_yymmdd(date,str+len,size-len+1,separator);
    len += l;


	return (l != 0);
}


bool DPString::AddTime(dword hour,dword min,dword sec,char separator)
{
	byte l;

	l = dtimetostrxex(hour,min,sec,str+len,size-len+1,separator);
    len += l;


	return (l != 0);
}

bool DPString::AddTime(dtime *time,char separator)
{
	byte l;

	l = dtimetostrxex(time,str+len,size-len+1,separator);
    len += l;


	return (l != 0);
}

bool DPString::AddSecAsTime(dword sec,char separator)
{
	dtime time;
	byte l;

	dsectotime(sec,&time);
	l = dtimetostrxex(&time,str+len,size-len+1,separator);
    len += l;


	return (l != 0);
}
#endif

bool DPString::AddPathDelimiter()
{
	if(len)
	{
		if((str[len - 1] == PATH_SEPARATOR_WIN) || (str[len - 1] == PATH_SEPARATOR_NIX))
		{
			str[len - 1] = PATH_SEPARATOR;

			return true;
		}
	}
	
	if( (len + 1) >= size)
	{

    	return false;
	}

	str[len] = PATH_SEPARATOR;
	len += 1;
    str[len] = 0;


	return true;
}


bool DPString::AddStrEnd()
{
	if( (len + 2) >= size)
	{

    	return false;
	}

    str[len++] = 0x0d;
    str[len++] = 0x0a;
    str[len] = 0;


	return true;
}


bool DPString::AddDumpByte(byte *dump,dword _len,char separator)
{

	
	if(!dump)
    	return false;

	if(!separator)
    {
    	if(((_len * 2) + len) > size)
		{

        	return false;
		}
    }
    else
	{

    	if(((_len * 3) + len) > size)
		{

        	return false;
		}
    }



    for(dword i=0;i<_len;i++)
    {
        if(separator && i)
        	str[len++] = separator;

		ddtoahex_byte(dump[i],str+len);
        len += 2;

    }
    str[len] = 0;



	return true;
}

bool DPString::AddDumpByteA(byte *dump,dword _len,char separator)
{
	if(!dump)
    	return false;

	if(!separator)
    {
    	if(((_len * 2) + len) > size)
        	return false;
    }
    else
	{

    	if(((_len * 3) + len) > size)
        	return false;
    }

    for(dword i=0;i<_len;i++)
    {
        if(separator && i)
        	str[len++] = separator;

		ddtoahex_byte_a(dump[i],str+len);
        len += 2;
    }
    str[len] = 0;

	return true;
}

bool DPString::AddDumpByte0x(byte *dump,dword _len,char separator,byte space)
{
	if(!dump)
    	return false;

	if(!separator)
    {
    	if(((_len * 4) + len + space) > size)
        	return false;
    }
    else
	{
    	if(((_len * 5) + len + space) > size)
        	return false;
    }

    for(dword i=0;i<_len;i++)
    {
        if(separator && i)
		{
        	str[len++] = separator;
			for(dword i=0;i<space;i++)
         		str[len++] = ' ';
		}

		str[len++] = '0';
       	str[len++] = 'x';
		ddtoahex_byte(dump[i],str+len);
        len += 2;

   }
    str[len] = 0;

	return true;
}

bool DPString::AddDumpByteA0x(byte *dump,dword _len,char separator,byte space)
{
	if(!dump)
    	return false;

	if(!separator)
    {
    	if(((_len * 4) + len + space) > size)
        	return false;
    }
    else
	{
    	if(((_len * 5) + len + space) > size)
        	return false;
    }

    for(dword i=0;i<_len;i++)
    {
        if(separator && i)
		{
        	str[len++] = separator;
			for(dword i=0;i<space;i++)
         		str[len++] = ' ';
		}

       	str[len++] = '0';
       	str[len++] = 'x';
		ddtoahex_byte(dump[i],str+len);
        len += 2;
   }
    str[len] = 0;

	return true;
}


char* DPString::AddAsToken(char *string,dword _len,dword *_dl,bool next_token)
{
	char *p;
	dword l,dl;

	p = dnspace_fixlen(string,_len,&l);
	if(!p)
		return 0;
	if(l > _len)
		return 0;

	Add(string,l);

	_len -= l;
	dl = l;

	if(next_token)
	{
		p = dispace_fixlen(p,_len,false,&l);
		if(!p)
			return 0;
		if(l > _len)
			return 0;

		dl += l;
	}

	if(_dl)
		*_dl = dl;

	return p;
}

char* DPString::AddAsToken(DPString *string,dword *_dl,bool next_token)
{
	dword dl;

	if(!string)
 		return 0;

	if(!AddAsToken(string->c_str(),string->Len(),&dl,next_token))
		return 0;

	if(next_token)
		string->DeleteLeft(dl);

	if(_dl)
		*_dl = dl;

	return string->c_str();
}


char* DPString::AddAsTokenDelimiter(char *string,dword _len,char delimiter,bool space,bool cs,dword *_dl,bool next_token)
{
	char *p;
	dword l,dl;

	p = dnsymbol_fixlen(string,_len,delimiter,space,cs,&l);
	if(!p)
		return 0;
	if(l > _len)
		return 0;

	Add(string,l);

	_len -= l;
	dl = l;

	if(_len)
	{
		if(*p == delimiter)
		{
			p++;
			_len--;
			dl++;
		}

		if(next_token)
		{
			p = dispace_fixlen(p,_len,false,&l);
			if(!p)
				return 0;
			if(l > _len)
				return 0;

			dl += l;
		}
	}

	if(_dl)
		*_dl = dl;

	return p;
}

char* DPString::AddAsTokenDelimiter(DPString *string,char delimiter,bool space,bool cs,dword *_dl,bool next_token)
{
	dword dl;

	if(!string)
 		return 0;

	if(!AddAsTokenDelimiter(string->c_str(),string->Len(),delimiter,space,cs,&dl,next_token))
		return 0;

	if(next_token)
		string->DeleteLeft(dl);

	if(_dl)
		*_dl = dl;

	return string->c_str();
}

char* DPString::AddStrLine(char *string,dword _len,dword *_dl)
{
	dword dl = 0;
	char s = 0;
	char *p;

	p = string;

	while(_len)
	{
		s = *p;

		if(!s || (s == '\r') || (s == '\n'))
			break;

		p++;
		_len--;
		dl++;
	}

	if(dl)
	{
		if(!Add(string,dl))
			return 0;
	}

	if(s == '\r')
	{
		p++;
		_len--;
		dl++;
		while(_len)
		{
			s = *p;

			if(!s || (s == '\n'))
				break;

			p++;
			_len--;
			dl++;
		}
	}
	if(s == '\n')
		dl++;

	if(_dl)
	{
		p++;
		*_dl = dl;
	}

	return p;
}

char* DPString::AddStrLine(DPString *string,dword *_dl,bool next_line)
{
	dword dl;

	if(!string)
 		return 0;

	if(!AddStrLine(string->c_str(),string->Len(),&dl))
		return 0;

	if(next_line)
		string->DeleteLeft(dl);

	if(_dl)
		*_dl = dl;

	return string->c_str();
}


dword DPString::FindStr(char *string,dword _len)
{
	dword s;

	if(!str || !string || !len)
		return 0xffffffff;

    if(_len > len)
		return 0xffffffff;

	s = len - _len + 1;

	for(dword i=0;i<s;i++)
	{
		if(memcmp(str + i,string,_len) == 0)
			return i;
	}

	return 0xffffffff;
}

dword DPString::FindStr(DPString *string)
{
	if(!string)
		return 0xffffffff;

	return FindStr(string->c_str(),string->Len());
}


void DPString::DeleteLeft(dword value)
{
	if(!size || !len)
		return;
	if(value > len)
		value = len;

	len -= value;
	memmove(str,str + value,len + 1);
}

bool DPString::nexttoken(char *p,dword _len,dword &dl)
{
	dword l;

	p = dispace_fixlen(p,_len,false,&l);
	if(!p)
		return false;
	if(l > _len)
		return false;

	dl += l;

	DeleteLeft(dl);

	return true;
}


bool DPString::_AddRnd(dword _len,char *srnd,byte srnd_len)
{
	if(!srnd)
		return false;

	if( (len + _len) >= size)
		return false;

	for(dword i=0;i<srnd_len;i++)
	{
		byte r = rand() % srnd_len;
		str[len++] = srnd[r];
	}

	str[len]  = 0;

	return true;
}

bool DPString::AddRnd_AaNum(dword _len)
{
	char chars[] = "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

	return _AddRnd(_len,_strl(chars));
}

bool DPString::AddRnd_ANum(dword _len)
{
	char chars[] = "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	return _AddRnd(_len,_strl(chars));
}

bool DPString::AddRnd_aNum(dword _len)
{
	char chars[] = "1234567890abcdefghijklmnopqrstuvwxyz";

	return _AddRnd(_len,_strl(chars));
}



char* DPString::GetDwordEx(dword *data,dword *_dl,bool next_token)
{
	char *p;
	dword dl;
	dword _len;

	p = dgetdwordex_fixlen(str,len,data,&dl);
	if(!p)
		return 0;
	if(dl > len)
		return 0;

	if(next_token)
	{
		_len = len - dl;









		if(!nexttoken(p,_len,dl))
			return 0;
	}

	if(_dl)
		*_dl = dl;

	return str;
}

char* DPString::GetByteEx(byte *data,dword *_dl,bool next_token)
{
	dword _data;

	if(!data)
		return 0;

	if(!GetDwordEx(&_data,_dl,next_token))
		return 0;
	if(_data > 255)
		return 0;

	*data = (byte)_data;

	return str;
}

char* DPString::GetWordEx(word *data,dword *_dl,bool next_token)
{
	dword _data;

	if(!data)
		return 0;

	if(!GetDwordEx(&_data,_dl,next_token))
		return 0;
	if(_data > 65535)
		return 0;

	*data = (word)_data;

	return str;
}

char* DPString::GetQwordEx(qword *data,dword *_dl,bool next_token)
{
	char *p;
	dword dl;
	dword _len;

	p = dgetqwordex_fixlen(str,len,data,&dl);
	if(!p)
		return 0;
	if(dl > len)
		return 0;

	if(next_token)
	{
		_len = len - dl;









		if(!nexttoken(p,_len,dl))
			return 0;
	}

	if(_dl)
		*_dl = dl;

	return str;
}


char* DPString::GetIP(dword *ip,dword *_dl,bool next_token)
{
	char *p;
	dword dl;
	dword _len;

	p = dstrtoip_fixlen(str,len,ip,&dl);
	if(!p)
		return 0;
	if(dl > len)
		return 0;

	if(next_token)
	{
		_len = len - dl;
		if(!nexttoken(p,_len,dl))
			return 0;
	}

	if(_dl)
		*_dl = dl;

	return str;
}

char* DPString::GetIPSwap(dword *ip,dword *_dl,bool next_token)
{
	char *p;
	dword dl;
	dword _len;

	p = dstrtoip_swap_fixlen(str,len,ip,&dl);
	if(!p)
		return 0;
	if(dl > len)
		return 0;

	if(next_token)
	{
		_len = len - dl;
		if(!nexttoken(p,_len,dl))
			return 0;
	}

	if(_dl)
		*_dl = dl;

	return str;
}

char* DPString::GetIPPort(dword *ip,word *port,dword *_dl,bool next_token)
{
	char *p;
	dword dl;
	dword _len;

	p = dstrtoipport_fixlen(str,len,ip,port,&dl);
	if(!p)
		return 0;
	if(dl > len)
		return 0;

	if(next_token)
	{
		_len = len - dl;
		if(!nexttoken(p,_len,dl))
			return 0;
	}

	if(_dl)
		*_dl = dl;

	return str;
}

char* DPString::GetIPPortSwap(dword *ip,word *port,dword *_dl,bool next_token)
{
	char *p;
	dword dl;
	dword _len;

	p = dstrtoipport_swap_fixlen(str,len,ip,port,&dl);
	if(!p)
		return 0;
	if(dl > len)
		return 0;

	if(next_token)
	{
		_len = len - dl;
		if(!nexttoken(p,_len,dl))
			return 0;
	}

	if(_dl)
		*_dl = dl;

	return str;
}




#ifdef LINUX
}
#endif
