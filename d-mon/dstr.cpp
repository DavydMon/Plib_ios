/*
   $Id:$
   $Change:$
   $DateTime:$
   $Author:$
*/



#include <string.h>


#include "dmain.h"
#include "dstr.h"

#ifdef LINUX
namespace dmon {
#endif

const char dlib_hex_symbol_table[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

const char *dlib_word_hex_symbols_table[256] =
{
	"00","01","02","03","04","05","06","07","08","09","0A","0B","0C","0D","0E","0F",
	"10","11","12","13","14","15","16","17","18","19","1A","1B","1C","1D","1E","1F",
	"20","21","22","23","24","25","26","27","28","29","2A","2B","2C","2D","2E","2F",
	"30","31","32","33","34","35","36","37","38","39","3A","3B","3C","3D","3E","3F",
	"40","41","42","43","44","45","46","47","48","49","4A","4B","4C","4D","4E","4F",
	"50","51","52","53","54","55","56","57","58","59","5A","5B","5C","5D","5E","5F",
	"60","61","62","63","64","65","66","67","68","69","6A","6B","6C","6D","6E","6F",
	"70","71","72","73","74","75","76","77","78","79","7A","7B","7C","7D","7E","7F",
	"80","81","82","83","84","85","86","87","88","89","8A","8B","8C","8D","8E","8F",
	"90","91","92","93","94","95","96","97","98","99","9A","9B","9C","9D","9E","9F",
	"A0","A1","A2","A3","A4","A5","A6","A7","A8","A9","AA","AB","AC","AD","AE","AF",
	"B0","B1","B2","B3","B4","B5","B6","B7","B8","B9","BA","BB","BC","BD","BE","BF",
	"C0","C1","C2","C3","C4","C5","C6","C7","C8","C9","CA","CB","CC","CD","CE","CF",
	"D0","D1","D2","D3","D4","D5","D6","D7","D8","D9","DA","DB","DC","DD","DE","DF",
	"E0","E1","E2","E3","E4","E5","E6","E7","E8","E9","EA","EB","EC","ED","EE","EF",
	"F0","F1","F2","F3","F4","F5","F6","F7","F8","F9","FA","FB","FC","FD","FE","FF"
};

const char *dlib_word_hex_symbols_table_a[256] =
{
	"00","01","02","03","04","05","06","07","08","09","0a","0b","0c","0d","0e","0f",
	"10","11","12","13","14","15","16","17","18","19","1a","1b","1c","1d","1e","1f",
	"20","21","22","23","24","25","26","27","28","29","2a","2b","2c","2d","2e","2f",
	"30","31","32","33","34","35","36","37","38","39","3a","3b","3c","3d","3e","3f",
	"40","41","42","43","44","45","46","47","48","49","4a","4b","4c","4d","4e","4f",
	"50","51","52","53","54","55","56","57","58","59","5a","5b","5c","5d","5e","5f",
	"60","61","62","63","64","65","66","67","68","69","6a","6b","6c","6d","6e","6f",
	"70","71","72","73","74","75","76","77","78","79","7a","7b","7c","7d","7e","7f",
	"80","81","82","83","84","85","86","87","88","89","8a","8b","8c","8d","8e","8f",
	"90","91","92","93","94","95","96","97","98","99","9a","9b","9c","9d","9e","9f",
	"a0","a1","a2","a3","a4","a5","a6","a7","a8","a9","aa","ab","ac","ad","ae","af",
	"b0","b1","b2","b3","b4","b5","b6","b7","b8","b9","ba","bb","bc","bd","be","bf",
	"c0","c1","c2","c3","c4","c5","c6","c7","c8","c9","ca","cb","cc","cd","ce","cf",
	"d0","d1","d2","d3","d4","d5","d6","d7","d8","d9","da","db","dc","dd","de","df",
	"e0","e1","e2","e3","e4","e5","e6","e7","e8","e9","ea","eb","ec","ed","ee","ef",
	"f0","f1","f2","f3","f4","f5","f6","f7","f8","f9","fa","fb","fc","fd","fe","ff"
};

byte dlib_hex_table[256] =
{
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff 
};




char* dispace(char *str,bool enter)
{
	if(!str)
		return 0;
	while(*str)
	{
		if(!(*str == 0x20 || *str == 0x09))
		{
			if(!enter)
				break;
			else
				if(!((*str == 0x0d) || (*str == 0x0a)))
					break;
		}
		str++;
	}
	return str;
}


char* dispace_fixlen(char *str,dword str_len,bool enter,dword *dl)
{
	dword l = 0;

	if(!str)
		return 0;
	for(dword i=0;i<str_len;i++)
	{
		if(!(*str == 0x20 || *str == 0x09))
		{
			if(!enter)
				break;
			else
				if(!((*str == 0x0d) || (*str == 0x0a)))
					break;
		}
		str++;
		l++;
	}

	if(dl)
		*dl = l;

	return str;
}




char* dnspace(char *str)
{
	if(!str)
		return 0;
	while(*str > 0x20)
		str++;
	return str;
}

char* dnspace_fixlen(char *str,dword str_len,dword *dl)
{
	dword l = 0;

	if(!str)
		return 0;
	for(dword i=0;i<str_len;i++)
	{

		if(*str <= 0x20)
			break;
		str++;
		l++;
	}

	if(dl)
		*dl = l;
	
	return str;
}

char* dnsymbol_fixlen(char *str,dword str_len,char symbol,bool space,bool cs,dword *dl)
{




	dword l = 0;
	char s;

	if(!str)
		return 0;
	for(dword i=0;i<str_len;i++)
	{
		s = *str;
		if(s == symbol)
			break;

		if(space && (s == ' '))
			break;

		if(cs && (*str < 0x20))
			break;
		str++;
		l++;
	}

	if(dl)
		*dl = l;
	
	return str;
}


inline char* dnspace_i(char *str)
{
	if(!str)
		return 0;
	while(*str > 0x20)
		str++;
	return str;
}


char* dstr_trim(char *str,bool enter)
{
	dword i,len;
	char *s;

	if(!str)
		return 0;

	str = (char*)dispace(str,enter);
	if(!str)
		return 0;

	len = dstrlen(str);
	if(len)
	{
		s = str + len - 1;
		for(i=0;i<len;i++)
		{
			if(!((*s == 0x20) || (*s == 0x09)))
			{
				if(!enter)
					break;
				else
					if(!((*s == 0x0d) || (*s == 0x0a)))
						break;
			}
			s--;
		}
		*(s+1) = 0;
	}

	return str;
}



char* dend_line_fixlen(char *str,dword str_len,dword *dl)
{
	dword l = 0;

	if(!str)
		return 0;
	for(dword i=0;i<str_len;i++)
	{
		if((*str == 0x0d) || (*str == 0x0a))
		{
			break;
		}
		str++;
		l++;
	}

	if(dl)
		*dl = l;

	return str;
}


char* dnext_line_fixlen(char *str,dword str_len,dword *dl)
{
	dword l = 0;

	if(!str)
		return 0;
	for(dword i=0;i<str_len;i++)
	{
		if(*str == 0x0a)
		{
			str++;
			l++;
			break;
		}
		str++;
		l++;
	}

	if(dl)
		*dl = l;

	return str;
}




char* diabc(char *str)
{
	byte t;
	if(!str)
		return 0;
	while(*str)
	{
		t = *str;
		if(t < 0x30)
			break;
		if((t > 0x39) && (t < 0x41))
			break;
		if((t > 0x5a) && (t < 0x5f))
			break;
		if((t == 0x60) || (t > 0x7a))
			break;
		str++;
	}
	return str;
}


void dstrcpy(char *str1,char *str2)
{
	if(!str1 || !str2)
		return;
	while(*str2)
	{
		*str1 = *str2;
		str1++;
		str2++;
	}
	*str1 = 0;
}


void dstrncpy(char *str1,char *str2,dword maxlen)
{
	if(!str1 || !str2 || !maxlen)
		return;
	while(*str2)
	{
		if(!maxlen)
			break;
		maxlen--;
		*str1 = *str2;
		str1++;
		str2++;
	}
	*str1 = 0;
}


void dstrcat(char *str1,char *str2)
{
	if(!str1 || !str2)
		return;
	while(*str1)
		str1++;
	while(*str2)
	{
		*str1 = *str2;
		str1++;
		str2++;
	}
	*str1 = 0;
}


dword dstrlen(char *str)
{
	dword len = 0;
	if(!str)
		return 0;
	while(*str)
	{
		len++;
		str++;
	}
	return len;
}



dword datod(char *str)
{
	byte symbol;
	dword ret=0;

	if(!str)
		return 0;
	
	while(*str)
	{
		symbol = *str;


		symbol -= 0x30;
        if(symbol > 9)
        	return ret;
		if(ret > 0x19999999)
			return 0;
		ret = (ret * 10);
		ret = (ret + symbol);
		str++;
	}
	return ret;
}


dword daton_array(byte *data,dword maxlen,char *str,dword len)
{
	byte symbol;
	dword ret = 0;

	if(!data || !str)
		return 0;

	for(dword i=0;i<len;i++)
	{
		symbol = str[i];
		symbol -= 0x30;
        if(symbol > 9)
        	return ret;

        if(!maxlen)
        	return 0;

		data[i] = symbol;
        ret++;
        maxlen--;
	}

	return ret;
}


/*
byte ddtoa(dword number,char *str,byte count,byte symbol,bool space)
{
	char bcd[5],dest[16];
	char *p_bcd,*p_dest,*p_ret;
	byte c,i,ret;

	p_bcd = bcd;
	p_dest = (dest + 1);
	dest[0] = 48;
	if(space)
	{
		dest[11] = 32;
		dest[12] = 0;
        ret = 12;
	}
	else
	{
		dest[11] = 0;
        ret = 11;
    }
	for(i=0;i<5;i++)
		bcd[i] = 0;

	c = count;
	if(count == 0)
		c = 1;
	else
		if(count > 10)
			c = 10;
	c--;

	__asm
	{
		push	eax
		push	ebx
		push	ecx
		push	edx

		mov		eax,number
		mov		ecx,32

next:	push	ecx
		mov		ecx,5
		mov		ebx,p_bcd

j0:		mov		dl,[ebx]
		and		dl,0fh
		cmp		dl,5
		jc		j1
		add		[byte ptr ebx],3
j1:		mov		dl,[ebx]
		and		dl,0f0h
		cmp		dl,80
		jc		j2
		add		[byte ptr ebx],48
j2:		inc		ebx
		loop	j0

		shl		eax,1
		mov		ecx,5
		dec		ebx
j3:		rcl		byte ptr [ebx],byte ptr 1
		dec		ebx
		loop	j3

		pop		ecx
		loop	next

		mov		ecx,5
		mov		ebx,p_bcd
		mov		edx,p_dest
next2:	mov		al,[ebx]
		and		al,0f0h
		ror		al,4
		add		al,48
		mov		[edx],al
		inc		edx
		mov		al,[ebx]
		and		al,0fh
		add		al,48
		mov		[edx],al
		inc		edx
		inc		ebx
		loop	next2

		pop		edx
		pop		ecx
		pop		ebx
		pop		eax
	}

	p_dest = (p_dest + 9 - c);
	p_ret = p_dest;
	for(i=0;i<c;i++)
	{
		if(*p_dest != 48)
			break;
		*p_dest = symbol;
		p_dest++;
	}

	if(symbol == 0)
		p_ret = p_dest;

    ret += (byte)(dest - p_ret);


	memcpy(str,p_ret,ret);
    str[ret] = 0;

    return ret;
}
*/


byte ddtoa(dword value,char *str,byte count,byte symbol,byte end_space_count)
{
    char buf[12];
	char *p;
    dword temp;
    byte d,i,c,ret;



	if(!str)
    	return 0;

	p = buf + 11;
    c = 0;

	do
	{
        temp = (value / 10);
		d = (unsigned) (value - (temp * 10));
		value = temp;

		*p-- = (char) (d + '0');
        c++;
	} while (value > 0);
	p++;

    ret = c;

    if(symbol)
    {
    	if(count > c)
        {
            ret = count;
        	count = (count - c);
    		for(i=0;i<count;i++)
    			*str++ = symbol;
        }
    }

    for(i=0;i<c;i++)
    	*str++ = *p++;

    if(end_space_count)
	{
		for(i=0;i<end_space_count;i++)
    		*str++ = ' ';
        ret += end_space_count;
    }

    *str = 0;

    return ret;
}


byte dqtoa(qword value,char *str,byte count,byte symbol,byte end_space_count)
{
    char buf[22];
	char *p;
    qword temp;
    byte d,i,c,ret;

	if(!str)
    	return 0;

	p = buf + 21;
    c = 0;

	do
	{
        temp = (value / 10);
		d = (unsigned) (value - (temp * 10));
		value = temp;

		*p-- = (char) (d + '0');
        c++;
	} while (value > 0);
	p++;

    ret = c;

    if(symbol)
    {
    	if(count > c)
        {
            ret = count;
        	count = (count - c);
    		for(i=0;i<count;i++)
    			*str++ = symbol;
        }
    }

    for(i=0;i<c;i++)
    	*str++ = *p++;

    if(end_space_count)
	{
		for(i=0;i<end_space_count;i++)
    		*str++ = ' ';
        ret += end_space_count;
    }

    *str = 0;

    return ret;
}


byte ddtoaf(dword value,char *str)
{
    char buf[12];
	char *p;
    dword temp;
    byte d,i,c,ret;

	if(!str)
    	return 0;

	p = buf + 11;
    c = 0;

	do
	{
        temp = (value / 10);
		d = (unsigned) (value - (temp * 10));
		value = temp;

		*p-- = (char) (d + '0');
        c++;
	} while (value > 0);
	p++;

    ret = c;

    for(i=0;i<c;i++)
    	*str++ = *p++;

    *str = 0;

    return ret;
}


byte dqtoaf(qword value,char *str)
{
    char buf[22];
	char *p;
    qword temp;
    byte d,i,c,ret;

	if(!str)
    	return 0;

	p = buf + 21;
    c = 0;

	do
	{
        temp = (value / 10);
		d = (unsigned) (value - (temp * 10));
		value = temp;

		*p-- = (char) (d + '0');
        c++;
	} while (value > 0);
	p++;

    ret = c;

    for(i=0;i<c;i++)
    	*str++ = *p++;

    *str = 0;

    return ret;
}


byte ddtoaex(dword value,char *str,dword str_max_size,byte count,byte symbol,byte end_space_count)
{
    char buf[12];
	char *p,*s;
    dword temp;
    byte d,i,c,ret;



	if(!str || !str_max_size)
    	return 0;
    s = str;

	p = buf + 11;
    c = 0;

	do
	{
        temp = (value / 10);
		d = (unsigned) (value - (temp * 10));
		value = temp;

		*p-- = (char) (d + '0');
        c++;
	} while (value > 0);
	p++;

    ret = c;

    if(symbol)
    {
    	if(count > c)
        {
            ret = count;
        	count = (count - c);
            if((dword)count > str_max_size)
            {
                *s = 0;
            	return 0;
            }
    		for(i=0;i<count;i++)
    			*str++ = symbol;
            str_max_size -= count;
        }
    }

    if((dword)c > str_max_size)
    {
        *s = 0;
    	return 0;
    }
    for(i=0;i<c;i++)
    	*str++ = *p++;
    str_max_size -= c;

    if(end_space_count)
	{
    	if((dword)end_space_count > str_max_size)
        {
        	*s = 0;
    		return 0;
        }
		for(i=0;i<end_space_count;i++)
    		*str++ = ' ';
        str_max_size -= end_space_count;
        ret += end_space_count;
    }

    if(!str_max_size)
    {
      	*s = 0;
    	return 0;
    }

    *str = 0;

    return ret;
}


byte dqtoaex(qword value,char *str,dword str_max_size,byte count,byte symbol,byte end_space_count)
{
    char buf[22];
	char *p,*s;
    qword temp;
    byte d,i,c,ret;

	if(!str || !str_max_size)
    	return 0;
    s = str;

	p = buf + 21;
    c = 0;

	do
	{
        temp = (value / 10);
		d = (unsigned) (value - (temp * 10));
		value = temp;

		*p-- = (char) (d + '0');
        c++;
	} while (value > 0);
	p++;

    ret = c;

    if(symbol)
    {
    	if(count > c)
        {
            ret = count;
        	count = (count - c);
            if((dword)count > str_max_size)
            {
                *s = 0;
            	return 0;
            }
    		for(i=0;i<count;i++)
    			*str++ = symbol;
            str_max_size -= count;
        }
    }

    if((dword)c > str_max_size)
    {
        *s = 0;
    	return 0;
    }
    for(i=0;i<c;i++)
    	*str++ = *p++;
    str_max_size -= c;

    if(end_space_count)
	{
    	if((dword)end_space_count > str_max_size)
        {
        	*s = 0;
    		return 0;
        }
		for(i=0;i<end_space_count;i++)
    		*str++ = ' ';
        str_max_size -= end_space_count;
        ret += end_space_count;
    }

    if(!str_max_size)
    {
      	*s = 0;
    	return 0;
    }

    *str = 0;

    return ret;
}


byte ddtoafex(dword value,char *str,dword str_max_size)
{
    char buf[12];
	char *p;
    dword temp;
    byte d,i,c,ret;

	if(!str || !str_max_size)
    	return 0;

	p = buf + 11;
    c = 0;

	do
	{
        temp = (value / 10);
		d = (unsigned) (value - (temp * 10));
		value = temp;

		*p-- = (char) (d + '0');
        c++;
	} while (value > 0);
	p++;

    ret = c;

    if((dword)(c+1) > str_max_size)
    {
        *str = 0;
    	return 0;
    }

    for(i=0;i<c;i++)
    	*str++ = *p++;
 

    *str = 0;

    return ret;
}


byte dqtoafex(qword value,char *str,dword str_max_size)
{
    char buf[22];
	char *p;
    qword temp;
    byte d,i,c,ret;

	if(!str || !str_max_size)
    	return 0;

	p = buf + 21;
    c = 0;

	do
	{
        temp = (value / 10);
		d = (unsigned) (value - (temp * 10));
		value = temp;

		*p-- = (char) (d + '0');
        c++;
	} while (value > 0);
	p++;

    ret = c;

    if((dword)(c+1) > str_max_size)
    {
        *str = 0;
    	return 0;
    }

    for(i=0;i<c;i++)
    	*str++ = *p++;
 

    *str = 0;

    return ret;
}


byte dltoa(sdword value,char *str,byte end_space_count)
{
	byte ret;

	if(!str)
    	return 0;

    if(value >= 0)
    	return ddtoa((dword)value,str,10,0,end_space_count);

    ret = ddtoa((dword)(-(sdword)value),str+1,10,0,end_space_count);
    if(!ret)
    	return 0;

    *str = '-';

    return (ret + 1);
}


byte dsqtoa(sqword value,char *str,byte end_space_count)
{
	byte ret;

	if(!str)
    	return 0;

    if(value >= 0)
    	return dqtoa((qword)value,str,20,0,end_space_count);

    ret = dqtoa((qword)(-(sqword)value),str+1,20,0,end_space_count);
    if(!ret)
    	return 0;

    *str = '-';

    return (ret + 1);
}


byte dltoaf(sdword value,char *str)
{
	byte ret;

	if(!str)
    	return 0;

    if(value >= 0)
    	return ddtoaf((dword)value,str);

    ret = ddtoaf((dword)(-(sdword)value),str+1);
    if(!ret)
    	return 0;

    *str = '-';

    return (ret + 1);
}


byte dqstoaf(sqword value,char *str)
{
	byte ret;

	if(!str)
    	return 0;

    if(value >= 0)
    	return dqtoaf((qword)value,str);

    ret = dqtoaf((qword)(-(sqword)value),str+1);
    if(!ret)
    	return 0;

    *str = '-';

    return (ret + 1);
}


byte dltoaex(sdword value,char *str,dword str_max_size,byte end_space_count)
{
	byte ret;

	if(!str)
    	return 0;

    if(value >= 0)
    	return ddtoaex((dword)value,str,str_max_size,10,0,end_space_count);

    if(str_max_size < 2)
    	return 0;

    ret = ddtoaex((dword)(-(sdword)value),str+1,str_max_size-1,10,0,end_space_count);
    if(!ret || ((dword)ret >= str_max_size))
	{
        *str = 0;
    	return 0;
    }

    *str = '-';

    return (ret + 1);
}


byte dsqtoaex(sqword value,char *str,dword str_max_size,byte end_space_count)
{
	byte ret;

	if(!str)
    	return 0;

    if(value >= 0)
    	return dqtoaex((qword)value,str,str_max_size,20,0,end_space_count);

    if(str_max_size < 2)
    	return 0;

    ret = dqtoaex((qword)(-(sqword)value),str+1,str_max_size-1,20,0,end_space_count);
    if(!ret || ((dword)ret >= str_max_size))
	{
        *str = 0;
    	return 0;
    }

    *str = '-';

    return (ret + 1);
}


byte dltoafex(sdword value,char *str,dword str_max_size)
{
	byte ret;

	if(!str)
    	return 0;

    if(value >= 0)
    	return ddtoafex((dword)value,str,str_max_size);

    if(str_max_size < 2)
    	return 0;

    ret = ddtoafex((dword)(-(sdword)value),str+1,str_max_size-1);
    if(!ret || ((dword)ret >= str_max_size))
	{
        *str = 0;
    	return 0;
    }

    *str = '-';

    return (ret + 1);
}


byte dsqtoafex(sqword value,char *str,dword str_max_size)
{
	byte ret;

	if(!str)
    	return 0;

    if(value >= 0)
    	return dqtoafex((qword)value,str,str_max_size);

    if(str_max_size < 2)
    	return 0;

    ret = dqtoafex((qword)(-(sqword)value),str+1,str_max_size-1);
    if(!ret || ((dword)ret >= str_max_size))
	{
        *str = 0;
    	return 0;
    }

    *str = '-';

    return (ret + 1);
}


byte dgetsymbolhex(byte symbol)
{
	return dlib_hex_table[symbol];
}


dword datodhex(char *str)
{
	dword ret = 0;
	byte symbol,count=0;
	if(!str)
		return 0;
	while(*str)
	{
		if(count >= 8)
			return 0;


        symbol = dlib_hex_table[(byte)*str];
		if(symbol == 0xff)
			return ret;
		ret = ret << 4;
		ret = ret | symbol;
		count++;
		str++;
	}
	return ret;
}


dword datodhexstr(char *str,byte *array,dword max)
{
	dword count=0;
	byte  value=0,v_count=0,symbol;
	if(!str || !array)
		return 0;

	while(*str)
	{

        symbol = dlib_hex_table[(byte)*str];
		if(symbol == 0xff)
		{
			if(v_count != 0)
				*array = value;
			return count;
		}
		value = value << 4;
		value = value | symbol;
		v_count++;
		if(v_count > 1)
		{
			*array = value;
			array++;
			value = v_count = 0;
		}
		count++;
		if((max != 0) && (count >= max))
			return count;
		str++;
	}
	return count;
}


byte ddtoahex(dword value,char *str,byte count,byte symbol,byte end_space_count)
{
    char buf[16];
	char *p;
	byte temp,c,ret;
	int i;

	if(!str)
		return 0 ;

	if(count > 8)
		count = 8;

	p = buf + sizeof(buf) - 1;
    c = 0;

	do
	{
		temp = (byte)(value & 0x0f);
		value = value >> 4;



		*p-- = dlib_hex_symbol_table[temp];
        c++;
	} while (value > 0);

	p++;

    if(c > count)
    {
        p -= (count - c);
    	c = count;
    }
    ret = c;

    if(symbol)
    {
    	if(count > c)
        {
            ret = count;
        	count = (count - c);
    		for(i=0;i<count;i++)
    			*str++ = symbol;
        }
    }

    for(i=0;i<c;i++)
    	*str++ = *p++;

    if(end_space_count)
	{
		for(i=0;i<end_space_count;i++)
    		*str++ = ' ';
        ret += end_space_count;
    }

    *str = 0;

    return ret;
}


byte ddtoahexf(dword value,char *str,byte count)
{
    char buf[16];
	char *p;
	byte temp,c,ret;
	int i;

	if(!str)
		return 0;

	if(count > 8)
		count = 8;

	p = buf + sizeof(buf) - 1;
    c = 0;

	do
	{
		temp = (byte)(value & 0x0f);
		value = value >> 4;
		*p-- = dlib_hex_symbol_table[temp];
        c++;
	} while (value > 0);

	p++;

    if(c > count)
    {
        p -= (count - c);
    	c = count;
    }
    ret = c;

    if(count > c)
    {
    	ret = count;
        count = (count - c);
        for(i=0;i<count;i++)
        	*str++ = '0';
    }

    for(i=0;i<c;i++)
    	*str++ = *p++;

    *str = 0;

    return ret;
}


byte dqtoahexf(qword value,char *str,byte count)
{
    char buf[24];
	char *p;
	byte temp,c,ret;
	int i;

	if(!str)
		return 0;

	if(count > 16)
		count = 16;

	p = buf + sizeof(buf) - 1;
    c = 0;

	do
	{
		temp = (byte)(value & 0x0f);
		value = value >> 4;
		*p-- = dlib_hex_symbol_table[temp];
        c++;
	} while (value > 0);

	p++;

    if(c > count)
    {
        p -= (count - c);
    	c = count;
    }
    ret = c;

    if(count > c)
    {
    	ret = count;
        count = (count - c);
        for(i=0;i<count;i++)
        	*str++ = '0';
    }

    for(i=0;i<c;i++)
    	*str++ = *p++;

    *str = 0;

    return ret;
}


byte ddtoahexex(dword value,char *str,dword str_max_size,byte count,byte symbol,byte end_space_count)
{
    char buf[12];
	char *p,*s;
	byte temp,c,ret;
	int i;

	if(!str || !str_max_size)
		return 0 ;
    s = str;

	if(count > 8)
		count = 8;

	p = buf + sizeof(buf) - 1;
    c = 0;

	do
	{
		temp = (byte)(value & 0x0f);
		value = value >> 4;
		*p-- = dlib_hex_symbol_table[temp];
        c++;
	} while (value > 0);

	p++;

    if(c > count)
    {
        p -= (count - c);
    	c = count;
    }
    ret = c;

    if(symbol)
    {
    	if(count > c)
        {
            ret = count;
        	count = (count - c);
            if((dword)count > str_max_size)
            {
            	*s = 0;
                return 0;
            }
    		for(i=0;i<count;i++)
    			*str++ = symbol;
            str_max_size -= count;
        }
    }

    if((dword)c > str_max_size)
    {
    	*s = 0;
        return 0;
    }
    for(i=0;i<c;i++)
    	*str++ = *p++;
    str_max_size -= c;

    if(end_space_count)
	{
    	if((dword)end_space_count > str_max_size)
    	{
    		*s = 0;
        	return 0;
    	}
		for(i=0;i<end_space_count;i++)
    		*str++ = ' ';
        str_max_size -= end_space_count;
        ret += end_space_count;
    }

    if(!str_max_size)
    {
    	*s = 0;
        return 0;
    }

    *str = 0;

    return ret;
}


byte dqtoahexex(qword value,char *str,dword str_max_size,byte count,byte symbol,byte end_space_count)
{
    char buf[24];
	char *p,*s;
	byte temp,c,ret;
	int i;

	if(!str || !str_max_size)
		return 0 ;
    s = str;

	if(count > 16)
		count = 16;

	p = buf + sizeof(buf) - 1;
    c = 0;

	do
	{
		temp = (byte)(value & 0x0f);
		value = value >> 4;
		*p-- = dlib_hex_symbol_table[temp];
        c++;
	} while (value > 0);

	p++;

    if(c > count)
    {
        p -= (count - c);
    	c = count;
    }
    ret = c;

    if(symbol)
    {
    	if(count > c)
        {
            ret = count;
        	count = (count - c);
            if((dword)count > str_max_size)
            {
            	*s = 0;
                return 0;
            }
    		for(i=0;i<count;i++)
    			*str++ = symbol;
            str_max_size -= count;
        }
    }

    if((dword)c > str_max_size)
    {
    	*s = 0;
        return 0;
    }
    for(i=0;i<c;i++)
    	*str++ = *p++;
    str_max_size -= c;

    if(end_space_count)
	{
    	if((dword)end_space_count > str_max_size)
    	{
    		*s = 0;
        	return 0;
    	}
		for(i=0;i<end_space_count;i++)
    		*str++ = ' ';
        str_max_size -= end_space_count;
        ret += end_space_count;
    }

    if(!str_max_size)
    {
    	*s = 0;
        return 0;
    }

    *str = 0;

    return ret;
}


byte ddtoahexfex(dword value,char *str,dword str_max_size,byte count)
{
    char buf[16];
	char *p,*s;
	byte temp,c,ret;
	int i;

	if(!str || !str_max_size)
		return 0;
    s = str;

	if(count > 8)
		count = 8;

	p = buf + sizeof(buf) - 1;
    c = 0;

	do
	{
		temp = (byte)(value & 0x0f);
		value = value >> 4;
		*p-- = dlib_hex_symbol_table[temp];
        c++;
	} while (value > 0);

	p++;

    if(c > count)
    {
        p -= (count - c);
    	c = count;
    }
    ret = c;

    if(count > c)
    {
    	ret = count;
        count = (count - c);
        if((dword)count > str_max_size)
        {
        	*s = 0;
            return 0;
        }
        for(i=0;i<count;i++)
        	*str++ = '0';
        str_max_size -= count;
    }

    if((dword)(c+1) > str_max_size)
    {
    	*s = 0;
        return 0;
    }
    for(i=0;i<c;i++)
    	*str++ = *p++;


    *str = 0;

    return ret;
}


byte dqtoahexfex(qword value,char *str,dword str_max_size,byte count)
{
    char buf[24];
	char *p,*s;
	byte temp,c,ret;
	int i;

	if(!str || !str_max_size)
		return 0;
    s = str;

	if(count > 16)
		count = 16;

	p = buf + sizeof(buf) - 1;
    c = 0;

	do
	{
		temp = (byte)(value & 0x0f);
		value = value >> 4;
		*p-- = dlib_hex_symbol_table[temp];
        c++;
	} while (value > 0);

	p++;

    if(c > count)
    {
        p -= (count - c);
    	c = count;
    }
    ret = c;

    if(count > c)
    {
    	ret = count;
        count = (count - c);
        if((dword)count > str_max_size)
        {
        	*s = 0;
            return 0;
        }
        for(i=0;i<count;i++)
        	*str++ = '0';
        str_max_size -= count;
    }

    if((dword)(c+1) > str_max_size)
    {
    	*s = 0;
        return 0;
    }
    for(i=0;i<c;i++)
    	*str++ = *p++;


    *str = 0;

    return ret;
}


void ddtoahex_byte(byte value,char *str)
{
	if(!str)
		return;

    *(word*)str = *(word*)dlib_word_hex_symbols_table[value];
    str[2] = 0;
}


void ddtoahex_byte_a(byte value,char *str)
{
	if(!str)
		return;

    *(word*)str = *(word*)dlib_word_hex_symbols_table_a[value];
    str[2] = 0;
}


void ddtoahex_word(word value,char *str)
{
	if(!str)
		return;

    *(word*)str     = *(word*)dlib_word_hex_symbols_table[(byte)(value >> 8)];
    *(word*)(str+2) = *(word*)dlib_word_hex_symbols_table[(byte)value];
    str[4] = 0;
}


void ddtoahex_word_a(word value,char *str)
{
	if(!str)
		return;

    *(word*)str     = *(word*)dlib_word_hex_symbols_table_a[(byte)(value >> 8)];
    *(word*)(str+2) = *(word*)dlib_word_hex_symbols_table_a[(byte)value];
    str[4] = 0;
}


void ddtoahex_dword(dword value,char *str)
{
	if(!str)
		return;

    *(word*)str     = *(word*)dlib_word_hex_symbols_table[(byte)(value >> 24)];
    *(word*)(str+2) = *(word*)dlib_word_hex_symbols_table[(byte)(value >> 16)];
    *(word*)(str+4) = *(word*)dlib_word_hex_symbols_table[(byte)(value >> 8)];
    *(word*)(str+6) = *(word*)dlib_word_hex_symbols_table[(byte)value];
    str[8] = 0;
}


void ddtoahex_dword_a(dword value,char *str)
{
	if(!str)
		return;

    *(word*)str     = *(word*)dlib_word_hex_symbols_table_a[(byte)(value >> 24)];
    *(word*)(str+2) = *(word*)dlib_word_hex_symbols_table_a[(byte)(value >> 16)];
    *(word*)(str+4) = *(word*)dlib_word_hex_symbols_table_a[(byte)(value >> 8)];
    *(word*)(str+6) = *(word*)dlib_word_hex_symbols_table_a[(byte)value];
    str[8] = 0;
}


void dqtoahex_qword(qword value,char *str)
{
	if(!str)
		return;

    *(word*)str      = *(word*)dlib_word_hex_symbols_table[(byte)(value >> 56)];
    *(word*)(str+2)  = *(word*)dlib_word_hex_symbols_table[(byte)(value >> 48)];
    *(word*)(str+4)  = *(word*)dlib_word_hex_symbols_table[(byte)(value >> 40)];
    *(word*)(str+6)  = *(word*)dlib_word_hex_symbols_table[(byte)(value >> 32)];
    *(word*)(str+8)  = *(word*)dlib_word_hex_symbols_table[(byte)(value >> 24)];
    *(word*)(str+10) = *(word*)dlib_word_hex_symbols_table[(byte)(value >> 16)];
    *(word*)(str+12) = *(word*)dlib_word_hex_symbols_table[(byte)(value >> 8)];
    *(word*)(str+14) = *(word*)dlib_word_hex_symbols_table[(byte)value];
    str[16] = 0;
}


void dqtoahex_qword_a(qword value,char *str)
{
	if(!str)
		return;

    *(word*)str      = *(word*)dlib_word_hex_symbols_table_a[(byte)(value >> 56)];
    *(word*)(str+2)  = *(word*)dlib_word_hex_symbols_table_a[(byte)(value >> 48)];
    *(word*)(str+4)  = *(word*)dlib_word_hex_symbols_table_a[(byte)(value >> 40)];
    *(word*)(str+6)  = *(word*)dlib_word_hex_symbols_table_a[(byte)(value >> 32)];
    *(word*)(str+8)  = *(word*)dlib_word_hex_symbols_table_a[(byte)(value >> 24)];
    *(word*)(str+10) = *(word*)dlib_word_hex_symbols_table_a[(byte)(value >> 16)];
    *(word*)(str+12) = *(word*)dlib_word_hex_symbols_table_a[(byte)(value >> 8)];
    *(word*)(str+14) = *(word*)dlib_word_hex_symbols_table_a[(byte)value];
    str[16] = 0;
}









byte dtokencmp(char *str,char *token)
{
	char b1,b2;

	if(!str || !token)
		return 0x80;

	while(*str)
	{
		b1 = *token;
		if(b1 == 0)
			return 0x81;
		b2 = *str;
		if(b1 < b2)
			return 0xff;
		if(b1 > b2)
			return 0x01;
		token++;
		str++;
	}
	return 0;
}








char* dtokencmp(char *str,char *token,byte *ret)
{
	byte b1,b2,r;

	if(!str || !token || !ret)
		return 0;

	r = 0;

	while(*str)
	{
		b1 = *token;
		if(b1 == 0)
		{
			r = 0x81;
			break;
		}
		b2 = *str;
		if(b1 < b2)
		{
			r = 0xff;
			break;
		}
		if(b1 > b2)
		{
			r = 0x01;
			break;
		}
		token++;
		str++;
	}
	*ret = r;
	return str;
}








char* dtokencmpl(char *str,char *token)
{
	char b1,b2;

	if(!str || !token)
		return 0;

	while(*token)
	{
		b1 = *token;
		b2 = *str;
		if(b1 != b2)
			return 0;
		token++;
		str++;
	}
	return str;
}












char* dgettoken(char *str,char *token,dword *size,bool enter)
{
	char *s1,*s2;
	dword len;

	if(!str || !token || !size)
		return 0;

	s1 = (char*)dispace(str,enter);
	if(*s1 == 0)
		return 0;
	s2 = (char*)diabc(s1);
	len = (dword)(s2 - s1);
	if(!len || (len > *size))
		return 0;
	dstrncpy(token,s1,len);
	*size = len;
	return s2;
}













char* dgettoken_param(char *str,char *token,dword *size,dword *value,bool *usevalue)
{
	char *s;
	char  temp[10];
	dword d;

	if(!str || !token || !size || !value || !usevalue)
		return 0;


	s = (char*)dgettoken(str,token,size,true);
	if(!s)
		return 0;

	s = (char*)dispace(s);
	if(*s != 0x5b)
	{
		*usevalue = false;
		*value = 0;
	} 
	else
	{
		d = 10;
		s = (char*)dgettoken(s+1,temp,&d);
		if(!s)
			return 0;
		s = (char*)dispace(s);
		if(*s != 0x5d)
			return 0;  
		*value = datod(temp);
		*usevalue = true;
		s = (char*)dispace(s+1,true);
	}
	return s;
}

















char* dgetdworddec(char *str,dword *value)
{
	dword result = 0;
	byte symbol;

	if(!str || !value)
    return 0;




	if( (byte)(str[0] - 0x30) > 9)
		return 0;

	while(*str)
	{
		symbol = *str;
        symbol -= 0x30;
		if(symbol > 9)
			break;
		if(result > 0x19999999)
			return 0;
		result = (result * 10);
		result = (result + symbol);
		str++;
	}
	*value = result;
	return str;
}

char* dgetqworddec(char *str,qword *value)
{
	qword result = 0;
	byte symbol;

	if(!str || !value)
    return 0;




	if( (byte)(str[0] - 0x30) > 9)
		return 0;

	while(*str)
	{
		symbol = *str;
        symbol -= 0x30;
		if(symbol > 9)
			break;
		if(result > 0x1999999999999999)
			return 0;
		result = (result * 10);
		result = (result + symbol);
		str++;
	}
	*value = result;
	return str;
}


char* dgetdworddec_fixlen(char *str,dword str_len,dword *value,dword *dl)
{
	dword result=0;
	byte symbol;
	dword l = 0;

	if(!str || !str_len || !value)
		return 0;

	if( (byte)(str[0] - 0x30) > 9)
		return 0;

	for(dword i=0;i<str_len;i++)
	{
		symbol = *str;
        symbol -= 0x30;
		if(symbol > 9)
			break;
		if(result > 0x19999999)
			return 0;
		result = (result * 10);
		result = (result + symbol);
		str++;
		l++;
	}
	*value = result;
	if(dl)
		*dl = l;
	return str;
}

char* dgetqworddec_fixlen(char *str,dword str_len,qword *value,dword *dl)
{
	qword result=0;
	byte symbol;
	dword l = 0;

	if(!str || !str_len || !value)
		return 0;

	if( (byte)(str[0] - 0x30) > 9)
		return 0;

	for(dword i=0;i<str_len;i++)
	{
		symbol = *str;
        symbol -= 0x30;
		if(symbol > 9)
			break;
		if(result > 0x1999999999999999)
			return 0;
		result = (result * 10);
		result = (result + symbol);
		str++;
		l++;
	}
	*value = result;
	if(dl)
		*dl = l;
	return str;
}

















char* dgetdwordhex(char *str,dword *value)
{
	dword result=0;
	byte symbol,count=0;

	if(!str || !value)
		return 0;
	if(str[0] == 0)
		return 0;

	while(*str)
	{
        symbol = dlib_hex_table[(byte)*str];
		if(symbol == 0xff)
		{
			if(!count)
				return 0;
			break;
		}
		if(count >= 8)
			return 0;

		result = result << 4;
		result = result | symbol;
		count++;
		str++;
	}
	*value = result;
	return str;
}

char* dgetdwordhex_fixlen(char *str,dword str_len,dword *value,dword *dl)
{
	dword result=0;
	byte symbol,count=0;
	dword l = 0;

	if(!str || !str_len || !value)
		return 0;
	if(str[0] == 0)
		return 0;

	for(dword i=0;i<str_len;i++)
	{
        symbol = dlib_hex_table[(byte)*str];
		if(symbol == 0xff)
		{
			if(!count)
				return 0;
			break;
		}
		if(count >= 8)
			return 0;

		result = result << 4;
		result = result | symbol;
		count++;
		str++;
		l++;
	}
	*value = result;
	if(dl)
		*dl = l;
	return str;
}









char* dgetqwordhex(char *str,qword *value)
{
	qword result=0;
	byte symbol,count=0;

	if(!str || !value)
		return 0;
	if(str[0] == 0)
		return 0;

	while(*str)
	{
        symbol = dlib_hex_table[(byte)*str];
		if(symbol == 0xff)
		{
			if(!count)
				return 0;
			break;
		}
		if(count >= 16)
			return 0;

		result = result << 4;
		result = result | symbol;
		count++;
		str++;
	}
	*value = result;
	return str;
}

char* dgetqwordhex_fixlen(char *str,dword str_len,qword *value,dword *dl)
{
	qword result = 0;
	byte symbol,count = 0;
	dword l = 0;

	if(!str || !str_len || !value)
		return 0;
	if(str[0] == 0)
		return 0;

	for(dword i=0;i<str_len;i++)
	{
        symbol = dlib_hex_table[(byte)*str];
		if(symbol == 0xff)
		{
			if(!count)
				return 0;
			break;
		}
		if(count >= 16)
			return 0;

		result = result << 4;
		result = result | symbol;
		count++;
		str++;
		l++;
	}
	*value = result;
	if(dl)
		*dl = l;
	return str;
}


bool dgethex_byte(char *str,byte *value)
{
	byte result = 0;
	byte symbol;

	if(!str || !value)
		return 0;

	for(byte i=0;i<2;i++)
	{
		symbol = dlib_hex_table[(byte)*str];
		if(symbol == 0xff)
			return false;

		result = result << 4;
		result = result | symbol;
		str++;
	}
	*value = result;
	return true;
}

bool dgethex_word(char *str,word *value)
{
	word result = 0;
	byte symbol;

	if(!str || !value)
		return 0;

	for(byte i=0;i<4;i++)
	{
		symbol = dlib_hex_table[(byte)*str];
		if(symbol == 0xff)
			return false;

		result = result << 4;
		result = result | symbol;
		str++;
	}
	*value = result;
	return true;
}

bool dgethex_dword(char *str,dword *value)
{
	dword result = 0;
	byte symbol;

	if(!str || !value)
		return 0;

	for(byte i=0;i<8;i++)
	{
		symbol = dlib_hex_table[(byte)*str];
		if(symbol == 0xff)
			return false;

		result = result << 4;
		result = result | symbol;
		str++;
	}
	*value = result;
	return true;
}

bool dgethex_qword(char *str,qword *value)
{
	qword result = 0;
	byte symbol;

	if(!str || !value)
		return 0;

	for(byte i=0;i<16;i++)
	{
		symbol = dlib_hex_table[(byte)*str];
		if(symbol == 0xff)
			return false;

		result = result << 4;
		result = result | symbol;
		str++;
	}
	*value = result;
	return true;
}

















char* dgetdwordex(char *str,dword *value)
{
	if(!str || !value)
		return 0;



	if( (byte)(str[0] - 0x30) > 9)
		return 0;

	if(str[0] == 0x30)
	{
		if((str[1] == 0x58) || (str[1] == 0x78))
			return dgetdwordhex(str+2,value);
	}

	return dgetdworddec(str,value);
}

char* dgetdwordex_fixlen(char *str,dword str_len,dword *value,dword *dl)
{
	if(!str || !str_len || !value)
		return 0;

	if( (byte)(str[0] - 0x30) > 9)
		return 0;

	if(str[0] == 0x30)
	{
		if(str_len > 2)
			if((str[1] == 0x58) || (str[1] == 0x78))
				return dgetdwordhex_fixlen(str+2,str_len-2,value,dl);
	}

	return dgetdworddec_fixlen(str,str_len,value,dl);
}

char* dgetqwordex(char *str,qword *value)
{
	if(!str || !value)
		return 0;

	if( (byte)(str[0] - 0x30) > 9)
		return 0;

	if(str[0] == 0x30)
	{
		if((str[1] == 0x58) || (str[1] == 0x78))
			return dgetqwordhex(str+2,value);
	}

	return dgetqworddec(str,value);
}

char* dgetqwordex_fixlen(char *str,dword str_len,qword *value,dword *dl)
{
	if(!str || !str_len || !value)
		return 0;

	if( (byte)(str[0] - 0x30) > 9)
		return 0;

	if(str[0] == 0x30)
	{
		if(str_len > 2)
			if((str[1] == 0x58) || (str[1] == 0x78))
				return dgetqwordhex_fixlen(str+2,str_len-2,value,dl);
	}

	return dgetqworddec_fixlen(str,str_len,value,dl);
}










char* dgetbytedumpex(char *str,byte *array,dword *len)
{
	dword value;
	dword l=0;
	dword maxlen;

	if(!len)
		return 0;

	maxlen = *len;
	*len = 0;

	if(!str || !array)
		return 0;

	while(*str)
	{
		if(maxlen)
			if(l >= maxlen)
				break;

		str = (char*)dispace(str);

		if( (str=dgetdwordex(str,&value)) == 0)
			return 0;
		if(value > 255)
			return 0;

		*array++ = (byte)value;
		l++;

		str = (char*)dispace(str);
		if((*str == 0x2c) || (*str == 0x2e))
			str++;
		else
			if(*str == 0x3b)
				break;
	}

	*len = l;

	return str;
}

char* dgetbytedumpex_fixlen(char *str,dword str_len,byte *array,dword *dl)
{
	dword value;
	dword l=0;
	dword maxlen;

	if(!dl)
		return 0;

	maxlen = *dl;
	*dl = 0;

	if(!str || !array)
		return 0;

	for(dword i=0;i<str_len;i++)
	{
		if(maxlen)
			if(l >= maxlen)
				break;

		str = (char*)dispace_fixlen(str,str_len - i,false);

		if( (str=dgetdwordex(str,&value)) == 0)
			return 0;
		if(value > 255)
			return 0;

		*array++ = (byte)value;
		l++;

		str = (char*)dispace_fixlen(str,str_len - i,false);
		if((*str == 0x2c) || (*str == 0x2e))
			str++;
		else
			if(*str == 0x3b)
				break;
	}

	*dl = l;

	return str;
}










char* dgetworddumpex(char *str,word *array,dword *len)
{
	dword value;
	dword l = 0;
	dword maxlen;

	if(!len)
		return 0;

	maxlen = *len;
	*len = 0;

	if(!str || !array)
		return 0;

	while(*str)
	{
		if(maxlen)
			if(l >= maxlen)
				break;

		str = (char*)dispace(str);

		if( (str=dgetdwordex(str,&value)) == 0)
			return 0;
		if(value > 65535)
			return 0;

		*array++ = (word)value;
		l++;

		str = (char*)dispace(str);
		if((*str == 0x2c) || (*str == 0x2e))
			str++;
		else
			if(*str == 0x3b)
				break;
	}

	*len = l;

	return str;
}










char* dgetdworddumpex(char *str,dword *array,dword *len)
{
	dword value;
	dword l = 0;
	dword maxlen;

	if(!len)
		return 0;

	maxlen = *len;
	*len = 0;

	if(!str || !array)
		return 0;

	while(*str)
	{
		if(maxlen)
			if(l >= maxlen)
				break;

		str = (char*)dispace(str);

		if( (str=dgetdwordex(str,&value)) == 0)
			return 0;

		*array++ = value;
		l++;

		str = (char*)dispace(str);
		if((*str == 0x2c) || (*str == 0x2e))
			str++;
		else
			if(*str == 0x3b)
				break;
	}

	*len = l;

	return str;
}










char* dgetbytedumphex(char *str,byte *array,dword *len)
{
	dword value;
	dword l=0;
	dword maxlen;

	if(!len)
		return 0;

	maxlen = *len;
	*len = 0;

	if(!str || !array)
		return 0;

	while(*str)
	{
		if(maxlen)
			if(l >= maxlen)
				break;

		str = (char*)dispace(str);

		if( (str=dgetdwordhex(str,&value)) == 0)
			return 0;
		if(value > 255)
			return 0;

		*array++ = (byte)value;
		l++;

		str = (char*)dispace(str);
		if((*str == 0x2c) || (*str == 0x2e))
			str++;
		else
			if(*str == 0x3b)
				break;
	}

	*len = l;

	return str;
}













char* diptostr(char *str,dword ip)
{
	char temp[4];

	if(!str)
		return 0;

	*str = 0;

	ddtoa(((byte*)&ip)[3],temp);
	strcat(str,temp);
	strcat(str,s_point);

	ddtoa(((byte*)&ip)[2],temp);
	strcat(str,temp);
	strcat(str,s_point);

	ddtoa(((byte*)&ip)[1],temp);
	strcat(str,temp);
	strcat(str,s_point);

	ddtoa(((byte*)&ip)[0],temp);
	strcat(str,temp);

	return str;
}

char* diptostrex(char *str,dword max_len,dword ip,dword *dl)
{
	char temp[4];
	byte l;
	dword _dl = 0;

	if(!str)
		return 0;

	*str = 0;

	l = ddtoa(((byte*)&ip)[3],temp);
	if(((dword)l + 1) > max_len)
		return 0;
	memcpy(str,temp,l);
	str += l;
	*str++ = '.';
	l++;
	max_len -= l;
	_dl += l;

	l = ddtoa(((byte*)&ip)[2],temp);
	if(((dword)l + 1) > max_len)
		return 0;
	memcpy(str,temp,l);
	str += l;
	*str++ = '.';
	l++;
	max_len -= l;
	_dl += l;

	l = ddtoa(((byte*)&ip)[1],temp);
	if(((dword)l + 1) > max_len)
		return 0;
	memcpy(str,temp,l);
	str += l;
	*str++ = '.';
	l++;
	max_len -= l;
	_dl += l;

	l = ddtoa(((byte*)&ip)[0],temp);
	if(((dword)l + 1) > max_len)
		return 0;
	memcpy(str,temp,l);
	str += l;
	_dl += l;
	*str = 0;

	if(dl)
		*dl = _dl;

	return str;
}













char* dipporttostr(char *str,dword ip,word port)
{
	char temp[6];

	if( (str=diptostr(str,ip)) == 0)
		return 0;

	ddtoa(port,temp);
	strcat(str,":");
	strcat(str,temp);

	return str;
}

char* dporttostrex(char *str,dword max_len,word port,dword *dl)
{
	char temp[6];
	dword _dl;
	byte l;

	l = ddtoa(port,temp);
	if(((dword)l + 2) > max_len)
		return 0;
	*str++ = ':';
	memcpy(str,temp,l);
	str += l;
	*str = 0;
	_dl = l + 1;

	if(dl)
		*dl = _dl;

	return str;
}

char* dipporttostrex(char *str,dword max_len,dword ip,word port,dword *dl)
{
	dword _dl,_dl2;

	if( (str=diptostrex(str,max_len,ip,&_dl)) == 0)
		return 0;
	if(_dl > max_len)
		return 0;
	max_len -= _dl;

	str = dporttostrex(str,max_len,port,&_dl2);
	if(!str)
		return 0;
	_dl += _dl2;

	if(dl)
		*dl = _dl;

	return str;
}


char* dgetbytedecex(char *str,byte *value,char end_symbol)
{
	dword v;

	while(*str)
	{
		if(*str > 0x20)
			break;
		str++;
	}
	if((*str < 0x30) || (*str > 0x39) )
		return 0;

	str = dgetdworddec(str,&v);
	if(!str)
		return 0;
	if(v > 255)
		return 0;

	while(*str)
	{
		if(*str > 0x20)
			break;
		str++;
	}
	if(end_symbol)
		if(*str != end_symbol)
			return 0;

	*value = (byte)v;

	return str;
}

char* dgetbytedecex_fixlen(char *str,dword str_len,byte *value,char end_symbol,dword *dl)
{
	dword v;
	dword _dl;

	while(*str)
	{
		if(*str > 0x20)
			break;
		str++;
	}
	if((*str < 0x30) || (*str > 0x39) )
		return 0;

	str = dgetdworddec_fixlen(str,str_len,&v,&_dl);
	if(!str)
		return 0;
	if(v > 255)
		return 0;
	if(_dl > str_len)
		return 0;
	str_len -= _dl;

	for(dword i=0;i<str_len;i++)
	{
		if(*str > 0x20)
			break;
		str++;
		_dl++;
	}
	if(end_symbol)
		if(*str != end_symbol)
			return 0;

	*value = (byte)v;
	if(dl)
		*dl = _dl;

	return str;
}



char* dstrtoip(char *str,dword *ip)
{
	dword _ip;
	byte v;

	if(!str || !ip)
		return 0;



	str = dgetbytedecex(str,&v,'.');
	if(!str)
		return 0;

	str++;
	((byte*)&_ip)[3] = (byte)v;


	str = dgetbytedecex(str,&v,'.');
	if(!str)
		return 0;

	str++;
	((byte*)&_ip)[2] = (byte)v;


	str = dgetbytedecex(str,&v,'.');
	if(!str)
		return 0;

	str++;
	((byte*)&_ip)[1] = (byte)v;


	str = dgetbytedecex(str,&v,0);
	if(!str)
		return 0;


	((byte*)&_ip)[0] = (byte)v;

	*ip = _ip;

	return str;
}

char* dstrtoip_swap(char *str,dword *ip)
{
	dword _ip;
	byte v;

	if(!str || !ip)
		return 0;



	str = dgetbytedecex(str,&v,'.');
	if(!str)
		return 0;

	str++;
	((byte*)&_ip)[0] = (byte)v;


	str = dgetbytedecex(str,&v,'.');
	if(!str)
		return 0;

	str++;
	((byte*)&_ip)[1] = (byte)v;


	str = dgetbytedecex(str,&v,'.');
	if(!str)
		return 0;

	str++;
	((byte*)&_ip)[2] = (byte)v;


	str = dgetbytedecex(str,&v,0);
	if(!str)
		return 0;


	((byte*)&_ip)[3] = (byte)v;

	*ip = _ip;

	return str;
}

char* dstrtoipport(char *str,dword *ip,word *port)
{
	dword _port;

	if(!port)
		return 0;

	str = dstrtoip(str,ip);
	if(!str)
		return 0;

	if(*str != ':')
		return 0;

	str = dispace(str + 1);
	if(!str)
		return 0;

	str = dgetdworddec(str,&_port);
	if(!str)
		return 0;
	if(_port > 65535)
		return 0;

	*port = (word)_port;

	return str;
}

char* dstrtoipport_swap(char *str,dword *ip,word *port)
{
	dword _port;

	if(!port)
		return 0;

	str = dstrtoip_swap(str,ip);
	if(!str)
		return 0;

	if(*str != ':')
		return 0;

	str = dispace(str + 1);
	if(!str)
		return 0;

	str = dgetdworddec(str,&_port);
	if(!str)
		return 0;
	if(_port > 65535)
		return 0;

	*port = (word)_port;

	return str;
}

#define DSTRTOIP(end) \
{\
	str = dgetbytedecex_fixlen(str,str_len,&v,end,&tdl);\
	if(!str)\
		return 0;\
	if(tdl > str_len)\
		return 0;\
	str_len -= tdl;\
	_dl += tdl;\
}

char* dstrtoip_fixlen(char *str,dword str_len,dword *ip,dword *dl)
{
	dword _ip;
	byte v;
	dword tdl,_dl=0;

	if(!str || !ip)
		return 0;



	DSTRTOIP('.');
	if(!str_len)
		return 0;
	str++;
	str_len--;
	_dl++;
	((byte*)&_ip)[3] = (byte)v;


	DSTRTOIP('.');
	if(!str_len)
		return 0;
	str++;
	str_len--;
	_dl++;
	((byte*)&_ip)[2] = (byte)v;


	DSTRTOIP('.');
	if(!str_len)
		return 0;
	str++;
	str_len--;
	_dl++;
	((byte*)&_ip)[1] = (byte)v;


	DSTRTOIP(0);
	((byte*)&_ip)[0] = (byte)v;

	*ip = _ip;

	if(dl)
		*dl = _dl;

	return str;
}

char* dstrtoip_swap_fixlen(char *str,dword str_len,dword *ip,dword *dl)
{
	dword _ip;
	byte v;
	dword tdl,_dl=0;

	if(!str || !ip)
		return 0;



	DSTRTOIP('.');
	if(!str_len)
		return 0;
	str++;
	str_len--;
	_dl++;
	((byte*)&_ip)[0] = (byte)v;


	DSTRTOIP('.');
	if(!str_len)
		return 0;
	str++;
	str_len--;
	_dl++;
	((byte*)&_ip)[1] = (byte)v;


	DSTRTOIP('.');
	if(!str_len)
		return 0;
	str++;
	str_len--;
	_dl++;
	((byte*)&_ip)[2] = (byte)v;


	DSTRTOIP(0);
	((byte*)&_ip)[3] = (byte)v;

	*ip = _ip;

	if(dl)
		*dl = _dl;

	return str;
}

char* dstrtoport_fixlen(char *str,dword str_len,word *port,dword _dl,dword *dl)
{
	dword _port;
	dword tdl;

	if(!port)
		return 0;

	if(!str_len)
		return 0;
	if(*str != ':')
		return 0;
	str++;
	str_len--;
	_dl++;

	str = dispace_fixlen(str,str_len,false,&tdl);
	if(!str)
		return 0;
	if(tdl > str_len)
		return 0;
	_dl += tdl;

	str = dgetdworddec_fixlen(str,str_len,&_port,&tdl);
	if(!str)
		return 0;
	if(tdl > str_len)
		return 0;
	_dl += tdl;

	if(_port > 65535)
		return 0;

	*port = (word)_port;

	if(dl)
		*dl = _dl;

	return str;
}

char* dstrtoipport_fixlen(char *str,dword str_len,dword *ip,word *port,dword *dl)
{
	dword _dl;

	str = dstrtoip_fixlen(str,str_len,ip,&_dl);
	if(!str)
		return 0;
	if(_dl > str_len)
		return 0;
	str_len -= _dl;

	return dstrtoport_fixlen(str,str_len,port,_dl,dl);
}

char* dstrtoipport_swap_fixlen(char *str,dword str_len,dword *ip,word *port,dword *dl)
{
	dword _dl;

	str = dstrtoip_swap_fixlen(str,str_len,ip,&_dl);
	if(!str)
		return 0;
	if(_dl > str_len)
		return 0;
	str_len -= _dl;

	return dstrtoport_fixlen(str,str_len,port,_dl,dl);
}

#undef DSTRTOIP


#ifdef LINUX
}
#endif
