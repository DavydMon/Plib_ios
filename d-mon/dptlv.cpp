#include <memory.h>

#include "dmain.h"

#include "dptlv.h"

#ifdef LINUX
namespace dmon {
#endif



DPrimitveTLV::DPrimitveTLV()
{
	p = 0;
	size = 0;
	l = 0;
	error = false;
}

DPrimitveTLV::DPrimitveTLV(byte *data,dword max_size)
{
	p = data;
	size = max_size;
	l  = 0;
	error = false;
	if(p && size)
		state = DPrimitveTLVState_empty;
	else
		state = DPrimitveTLVState_error;
}


DPrimitveTLV::DPrimitveTLV(byte *data,dword max_size,byte *value,dword value_len)
{
	p = data;
	size = max_size;
	error = false;

	if(p && size && value)
	{
		if( value_len >= size)
		{
			l = 0;
			state = DPrimitveTLVState_error;
		}
		else
		{
			memcpy(p,value,value_len);
			l = value_len;
			state = DPrimitveTLVState_empty;
		}
	}
	else
	{
		l  = 0;
		state = DPrimitveTLVState_error;
	}
}

DPrimitveTLV::DPrimitveTLV(byte *data,dword max_size,DPrimitveTLV& value)
{
	p = data;
	size = max_size;
	error = false;

	if(p && size)
	{
		if( value.l >= size)
		{
			l = 0;
			state = DPrimitveTLVState_error;
		}
		else
		{
			memcpy(p,value.p,value.l);
			l = value.l;
			state = DPrimitveTLVState_empty;
		}
	}
	else
	{
		l  = 0;
		state = DPrimitveTLVState_error;
	}
}

bool DPrimitveTLV::Init(byte *data,dword max_size)
{
	l  = 0;
	if(data && max_size)
	{
		p = data;
		size = max_size;
		state = DPrimitveTLVState_empty;
		return true;
	}
	else
	{
		p = 0;
		size = 0;
		state = DPrimitveTLVState_error;
		return false;
	}
}

bool DPrimitveTLV::Init(byte *data,dword max_size,dword shift)
{
	if(shift >= max_size)
		return false;

	l  = 0;
	if(data && max_size)
	{
		p = data + shift;
		size = max_size - shift;

		state = DPrimitveTLVState_empty;
		return true;
	}
	else
	{
		p = 0;
		size = 0;

		state = DPrimitveTLVState_error;
		return false;
	}
}


void DPrimitveTLV::Clear()
{
	l = 0;
	error = false;
	if(p && size)
		state = DPrimitveTLVState_empty;
	else
		state = DPrimitveTLVState_error;
}


DPrimitveTLV ::operator byte*()
{
	return p;
}

byte* DPrimitveTLV::GetP()
{
	return p;
}

byte* DPrimitveTLV::GetPS()
{
	return p + l;
}


byte* DPrimitveTLV::GetPTLV()
{
	if(!p || !l)
		return 0;
	return (p + 1);
}


dword DPrimitveTLV::Len()
{
	return l;
}

dword DPrimitveTLV::Size()
{
	return size;
}

dword DPrimitveTLV::EmptySize()
{
	return size - l;
}




DPrimitveTLVState DPrimitveTLV::GetState()
{
	return state;
}

bool DPrimitveTLV::GetError()
{
	return error;
}



bool DPrimitveTLV::SetPrimitve(byte primitve)
{
	if(!p || !size)
	{
		state = DPrimitveTLVState_error;
		error = true;
		return false;
	}

	p[0] = primitve;
	l = 1;

	state = DPrimitveTLVState_ok;
	return true;
}

bool DPrimitveTLV::SetPrimitveWord(word primitve)
{
	if(!p || (size < 2))
	{
		state = DPrimitveTLVState_error;
		error = true;
		return false;
	}

	*(word*)(p + l) = primitve;
	l = 2;

	state = DPrimitveTLVState_ok;
	return true;
}

bool DPrimitveTLV::SetPrimitveDword(dword primitve)
{
	if(!p || (size < 4))
	{
		state = DPrimitveTLVState_error;
		error = true;
		return false;
	}

	*(dword*)(p + l) = primitve;
	l = 4;

	state = DPrimitveTLVState_ok;
	return true;
}

byte* DPrimitveTLV::SetHeader(dword len)
{
	byte* result;

	if(!p || !size)
	{
		state = DPrimitveTLVState_error;
		error = true;
		return 0;
	}
	if( (l + len) > size)
	{
		state = DPrimitveTLVState_error;
		error = true;
		return 0;
	}

	result = p + l;
	l += len;

	return result;
}

bool DPrimitveTLV::Seek(dword len)
{
	if(!p || !size)
	{
		state = DPrimitveTLVState_error;
		error = true;
		return false;
	}
	if( (l + len) > size)
	{
		state = DPrimitveTLVState_error;
		error = true;
		return false;
	}

	l += len;

	state = DPrimitveTLVState_ok;
	return true;
}


bool DPrimitveTLV::AddByte(byte value)
{
	if(!p || !size)
	{
		state = DPrimitveTLVState_error;
		error = true;
		return false;
	}
	if( (l + 2) > size)
	{
		state = DPrimitveTLVState_error;
		error = true;
		return false;
	}

	*(byte*)(p + l) = value;
	l += 1;

	state = DPrimitveTLVState_ok;
	return true;
}

bool DPrimitveTLV::AddWord(word value)
{
	if(!p || !size)
	{
		state = DPrimitveTLVState_error;
		error = true;
		return false;
	}
	if( (l + 2) > size)
	{
		state = DPrimitveTLVState_error;
		error = true;
		return false;
	}

	*(word*)(p + l) = value;
	l += 2;

	state = DPrimitveTLVState_ok;
	return true;
}

bool DPrimitveTLV::AddDword(dword value)
{
	if(!p || !size)
	{
		state = DPrimitveTLVState_error;
		error = true;
		return false;
	}
	if( (l + 4) > size)
	{
		state = DPrimitveTLVState_error;
		error = true;
		return false;
	}

	*(dword*)(p + l) = value;
	l += 4;

	state = DPrimitveTLVState_ok;
	return true;
}

bool DPrimitveTLV::AddBin(void *value,dword len)
{
	if(!p || !size)
	{
		state = DPrimitveTLVState_error;
		error = true;
		return false;
	}
	if( (l + len) > size)
	{
		state = DPrimitveTLVState_error;
		error = true;
		return false;
	}

	memcpy(p + l,value,len);
	l += len;

	state = DPrimitveTLVState_ok;
	return true;
}


bool DPrimitveTLV::GetPrimitve(byte *primitve)
{
	if(!p || !l || !primitve)
		return false;

	*primitve = p[0];

	return true;
}


/*
bool DPrimitveTLV::_AddTLV(byte type,void *value,byte len)
{
	if(!p)
		return false;
	if(len && !value)
		return false;
	if( (l + 2 + len) > size)
		return false;

	*(byte*)(p + l) = type;
	l++;
	*(byte*)(p + l) = len;
	l++;

	if(len)
	{
		memcpy(p + l,value,len);
		l += len;
	}

	return true;
}

bool DPrimitveTLV::_AddTwLV(byte type,void *value,word len)
{
	if(!p)
		return false;
	if(len && !value)
		return false;
	if( (l + 3 + len) > size)
		return false;

	*(byte*)(p + l) = type;
	l++;
	*(word*)(p + l) = len;
	l += 2;

	if(len)
	{
		memcpy(p + l,value,len);
		l += len;
	}

	return true;
}

bool DPrimitveTLV::_AddwTwLV(word type,void *value,word len)
{
	if(!p)
		return false;
	if(len && !value)
		return false;
	if( (l + 4 + len) > size)
		return false;

	*(word*)(p + l) = type;
	l += 2;
	*(word*)(p + l) = len;
	l += 2;

	if(len)
	{
		memcpy(p + l,value,len);
		l += len;
	}

	return true;
}
*/

bool DPrimitveTLV::_AddSwTwLV(word type,byte status,void *value,word len)
{
	if(!p)
		return false;
	if(len && !value)
		return false;
	if( (l + 5 + len) > size)
		return false;

	*(word*)(p + l) = type;
	l += 2;
	*(word*)(p + l) = len + 1;
	l += 2;
	*(p + l) = status;
	l++;

	if(len)
	{
		memcpy(p + l,value,len);
		l += len;
	}

	return true;
}

bool DPrimitveTLV::_AddwTwLV_AsCString(word type,void *value,word len)
{
	if(!p)
		return false;
	if(len && !value)
		return false;
	if( (l + 5 + len) > size)
		return false;

	*(word*)(p + l) = type;
	l += 2;
	*(word*)(p + l) = len + 1;
	l += 2;

	if(len)
	{
		memcpy(p + l,value,len);
		l += len;
	}

	p[l++] = 0;

	return true;
}

bool DPrimitveTLV::_AddSwTwLV_AsCString(word type,byte status,void *value,word len)
{
	if(!p)
		return false;
	if(len && !value)
		return false;
	if( (l + 6 + len) > size)
		return false;

	*(word*)(p + l) = type;
	l += 2;
	*(word*)(p + l) = len + 2;
	l += 2;
	*(p + l) = status;
	l++;

	if(len)
	{
		memcpy(p + l,value,len);
		l += len;
	}

	p[l++] = 0;

	return true;
}


bool DPrimitveTLV::AddTLV(byte type,void *value,byte len)
{
	bool result;


	result = TAddTLV(type, value, len);
	if(result)
		state = DPrimitveTLVState_ok;
	else
	{
		state = DPrimitveTLVState_error;
		error = true;
	}
	
	return result;
}


bool DPrimitveTLV::AddTwLV(byte type,void *value,word len)
{
	bool result;


	result = TAddTLV(type, value, len);
	if(result)
		state = DPrimitveTLVState_ok;
	else
	{
		state = DPrimitveTLVState_error;
		error = true;
	}

	return result;
}

bool DPrimitveTLV::AddwTwLV(word type,void *value,word len)
{
	bool result;


	result = TAddTLV(type, value, len);
	if(result)
		state = DPrimitveTLVState_ok;
	else
	{
		state = DPrimitveTLVState_error;
		error = true;
	}

	return result;
}

bool DPrimitveTLV::AddSwTwLV(word type,byte status,void *value,word len)
{
	bool result;

	result = _AddSwTwLV(type,status,value,len);
	if(result)
		state = DPrimitveTLVState_ok;
	else
	{
		state = DPrimitveTLVState_error;
		error = true;
	}

	return result;
}

bool DPrimitveTLV::AddwTwLV_AsCString(word type,void *value,word len)
{
	bool result;

	result = _AddwTwLV_AsCString(type,value,len);
	if(result)
		state = DPrimitveTLVState_ok;
	else
	{
		state = DPrimitveTLVState_error;
		error = true;
	}

	return result;
}

bool DPrimitveTLV::AddSwTwLV_AsCString(word type,byte status,void *value,word len)
{
	bool result;

	result = _AddSwTwLV_AsCString(type,status,value,len);
	if(result)
		state = DPrimitveTLVState_ok;
	else
	{
		state = DPrimitveTLVState_error;
		error = true;
	}

	return result;
}


bool DPrimitveTLV::AddwTdLV(word type, void *value, dword len)
{
	bool result;

	result = TAddTLV(type, value, len);
	if (result)
		state = DPrimitveTLVState_ok;
	else
	{
		state = DPrimitveTLVState_error;
		error = true;
	}

	return result;
}

bool DPrimitveTLV::AdddTdLV(dword type, void *value, dword len)
{
	bool result;

	result = TAddTLV(type, value, len);
	if (result)
		state = DPrimitveTLVState_ok;
	else
	{
		state = DPrimitveTLVState_error;
		error = true;
	}

	return result;
}


bool DPrimitveTLV::_AddTLVV(byte type,void *value1,byte len1,void *value2,byte len2)
{
	byte len;
	
	if(!p)
		return false;
	if(len1 && !value1)
		return false;
	if(len2 && !value2)
		return false;
	if( ((int)len1 + (int)len2) > 255 )
		return false;
	len = len1 + len2;
	if( (l + 2 + len) > size)
		return false;

	*(byte*)(p + l) = type;
	l++;
	*(byte*)(p + l) = len;
	l++;

	if(len1)
	{
		memcpy(p + l,value1,len1);
		l += len1;
	}
	if(len2)
	{
		memcpy(p + l,value2,len2);
		l += len2;
	}

	return true;
}


bool DPrimitveTLV::AddTLVV(byte type,void *value1,byte len1,void *value2,byte len2)
{
	bool result;

	result = _AddTLVV(type,value1,len1,value2,len2);
	if(result)
		state = DPrimitveTLVState_ok;
	else
	{
		state = DPrimitveTLVState_error;
		error = true;
	}

	return result;
}


bool DPrimitveTLV::_AddTLVVV(byte type,void *value1,byte len1,void *value2,byte len2,void *value3,byte len3)
{
	byte len;
	
	if(!p)
		return false;
	if(len1 && !value1)
		return false;
	if(len2 && !value2)
		return false;
	if(len3 && !value3)
		return false;
	if( ((int)len1 + (int)len2 + (int)len3) > 255 )
		return false;
	len = len1 + len2 + len3;
	if( (l + 2 + len) > size)
		return false;

	*(byte*)(p + l) = type;
	l++;
	*(byte*)(p + l) = len;
	l++;

	if(len1)
	{
		memcpy(p + l,value1,len1);
		l += len1;
	}
	if(len2)
	{
		memcpy(p + l,value2,len2);
		l += len2;
	}
	if(len3)
	{
		memcpy(p + l,value3,len3);
		l += len3;
	}

	state = DPrimitveTLVState_ok;
	return true;
}


bool DPrimitveTLV::AddTLVVV(byte type,void *value1,byte len1,void *value2,byte len2,void *value3,byte len3)
{
	bool result;

	result = _AddTLVVV(type,value1,len1,value2,len2,value3,len3);
	if(result)
		state = DPrimitveTLVState_ok;
	else
	{
		state = DPrimitveTLVState_error;
		error = true;
	}

	return result;
}


bool DPrimitveTLV::AddTLV_byte(byte type,byte value)
{
	if(!p)
		return false;
	if( (l + 3) > size)
		return false;

	*(byte*)(p + l) = type;
	l++;
	*(byte*)(p + l) = 1;
	l++;
	*(byte*)(p + l) = value;
	l++;

	return true;
}


bool DPrimitveTLV::AddTLV_dword(byte type,dword value)
{
	if(!p)
		return false;
	if( (l + 3) > size)
		return false;

	*(byte*)(p + l) = type;
	l++;
	*(byte*)(p + l) = 4;
	l++;
	*(dword*)(p + l) = value;
    l += 4;

	return true;
}


#ifdef LINUX
}
#endif
