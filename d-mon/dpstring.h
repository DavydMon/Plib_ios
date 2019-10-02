/*
   $Id: 
   $Change: 135243 $
   $DateTime: 2012/05/28 11:50:19 $
   $Author: DavidMon $
*/


#ifndef dpstringH
#define dpstringH

#include <string.h>
#include "dmain.h"
#include "dstr.h"


#ifdef LINUX
#include <string.h>

namespace dmon {
#endif

class DPString
{
	protected:

		dword size;
		dword len;
		char  *str;


		inline bool nexttoken(char *p,dword _len,dword &dl);

		bool _AddRnd(dword _len,char *srnd,byte srnd_len);

	public:

		dword Size();
		dword Length();
		dword Len();
		bool  SetLength(dword length);
		bool  SetLen(dword length);
		void  Clear();
		char* c_str();
		bool  Set(char character);
		bool  Set(const char *string);
		bool  Set(const char *string, dword _len);
		bool  Set(DPString& string);
		bool  Add(char character);
		bool  Add(const char *string);
		bool  Add(const char *string, dword _len);
		bool  Add(DPString& string);
		bool  AddW    (short *string,dword _len);;
		bool  AddWSwap(short *string,dword _len);
		bool  AddWAsUTF8(short *string,dword _len);
		bool  AddWAsUTF8b(short *string,dword _len);
		bool  Get(char *string);
		bool  Get(char *string,dword max_len);
		bool  Cmp(char *string);
		bool  Cmp(char *string,dword _len);
		bool  Cmp(DPString& string);

		char operator [](dword index);
		operator char*();
		DPString& operator = (char character);
#ifndef LINUX
		DPString& operator = (char* string);
		DPString& operator +=(char character);

		DPString& operator +=(char* string);
		DPString& operator = (DPString& string);

		DPString& operator +=(DPString& string);
#else
		DPString& operator = (char* string) = delete;
		DPString& operator = (const char* string) = delete;
		DPString& operator +=(char character);
		DPString& operator +=(char* string);
		DPString& operator = (DPString& string) = delete;
		DPString& operator = (const DPString& string) = delete;
		DPString& operator +=(DPString& string);
#endif

		bool operator ==(char* string);
		bool operator !=(char* string);
		bool operator ==(DPString& string);
		bool operator !=(DPString& string);

		bool ChangeChar(dword index,char character);

		bool AddDec     (dword value);
		bool AddDecEx   (dword value,byte count=10,byte symbol=0,byte end_space_count=0);
		bool AddQDec    (qword value);
		bool AddQDecEx  (dword value,byte count=20,byte symbol=0,byte end_space_count=0);
		bool AddLong    (sdword value);
		bool AddLongEx  (sdword value,byte end_space_count=0);
		bool AddSQDec   (sqword value);
		bool AddSQDecEx (sqword value,byte end_space_count=0);

        bool AddDouble  (double value,byte dl=3);

		bool AddHex     (dword value,byte count=8);
		bool AddHexEx   (dword value,byte count=8,byte symbol='0',byte end_space_count=0);
		bool AddQHex    (qword value,byte count=16);
		bool AddQHexEx  (qword value,byte count=16,byte symbol='0',byte end_space_count=0);
		bool AddHexByte (byte value);
		bool AddHexWord (word value);
		bool AddHexDword(dword value);
		bool AddHexQword(qword value);

		bool AddIP(dword ip);
		bool AddIPSwap(dword ip);
		bool AddIPPort(dword ip,word port);
		bool AddIPPortSwap(dword ip,word port);

		bool AddSpace(dword count);
		bool AddSpaceAlignLen(dword _size);
		bool AddSpaceTab(dword tab);
		void Trim();
		void TrimLeft();
		void TrimRight();
		void TrimPath();
		void TrimQuotes(bool trim_space_before=false,bool trim_space_after=false);
		bool TrimFileExt();

		void UpperCase();
		void LowerCase();
#if defined(ddateH)
		bool AddDate (dword year,byte mon,byte day,char separator='.'); 
                bool AddDate (ddate *date,char separator='.');                  
		bool AddDate2(dword year,byte mon,byte day,char separator='.'); 
                bool AddDate2(ddate *date,char separator='.');                  
		bool AddDate_yymmdd(dword year,byte mon,byte day,char separator='.');
                bool AddDate_yymmdd(ddate *date,char separator='.');

		bool AddTime (dword hour,dword min,dword sec,char separator=':');
                bool AddTime (dtime *time,char separator=':');
		bool AddSecAsTime(dword sec,char separator=':');
#endif
		bool AddPathDelimiter();
		bool AddStrEnd();

		bool AddDumpByte   (byte *dump,dword _len,char separator=' ');
		bool AddDumpByteA  (byte *dump,dword _len,char separator=' ');
		bool AddDumpByte0x (byte *dump,dword _len,char separator=',',byte space=1);
		bool AddDumpByteA0x(byte *dump,dword _len,char separator=',',byte space=1);

		char* AddAsToken(char *string,dword _len,dword *_dl=0,bool next_token=true);
		char* AddAsToken(DPString *string,dword *_dl=0,bool next_token=true);
		char* AddAsTokenDelimiter(char *string,dword _len,char delimiter,bool space=false,bool cs=false,dword *_dl=0,bool next_token=true);
		char* AddAsTokenDelimiter(DPString *string,char delimiter,bool space=false,bool cs=false,dword *_dl=0,bool next_token=true);
		char* AddStrLine(char *string,dword _len,dword *_dl);
		char* AddStrLine(DPString *string,dword *_dl=0,bool next_line=true);

		dword FindStr(char *string,dword _len);
		dword FindStr(DPString *string);

		bool AddRnd_AaNum(dword _len);
		bool AddRnd_ANum (dword _len);
		bool AddRnd_aNum (dword _len);

		void DeleteLeft(dword value);

		char* GetDwordEx(dword *data,dword *_dl=0,bool next_token=true);
		char* GetByteEx (byte  *data,dword *_dl=0,bool next_token=true);
		char* GetWordEx (word  *data,dword *_dl=0,bool next_token=true);
		char* GetQwordEx(qword *data,dword *_dl=0,bool next_token=true);

		char* GetIP        (dword *ip,dword *_dl=0,bool next_token=true);
		char* GetIPSwap    (dword *ip,dword *_dl=0,bool next_token=true);
		char* GetIPPort    (dword *ip,word *port,dword *_dl=0,bool next_token=true);
		char* GetIPPortSwap(dword *ip,word *port,dword *_dl=0,bool next_token=true);

		DPString(char *_str,dword _size);
		DPString(char *_str,dword _size,dword _len);
		DPString(char *_str,dword _size,char symbol);
		DPString(char *_str,dword _size,char *string);
		DPString(char *_str,dword _size,char *string,dword _len);
		DPString(char *_str,dword _size,DPString& string);
};

#define _str_pl(v)  v.c_str(),v.Len()
#define _str_plb(v) v.c_str(),(byte)v.Len()
#define _str_plw(v) v.c_str(),(word)v.Len()


#ifdef LINUX
}
#endif

#endif

