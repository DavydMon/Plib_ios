#ifndef dsymbconvH
#define dsymbconvH

#include "dmain.h"

#ifdef LINUX
namespace dmon {
#endif





extern unsigned char  dtable_UCS2_win1251[96];

void dconvert_ISO88595_to_win1251 (unsigned char  *dest,unsigned char  *sour,dword len);
void dconvert_win1251_to_ISO88595 (unsigned char  *dest,unsigned char  *sour,dword len);
void dconvert_UCS2_to_win1251     (unsigned char  *dest,unsigned short *sour,dword len);
void dconvert_UCS2_to_win1251_swap(unsigned char  *dest,unsigned short *sour,dword len);
void dconvert_win1251_to_UCS2     (unsigned short *dest,unsigned char  *sour,dword len);
void dconvert_win1251_to_UCS2_swap(unsigned short *dest,unsigned char  *sour,dword len);

void dconvert_UCS2_to_ISO88595     (unsigned char  *dest,unsigned short *sour,_dword len);
void dconvert_UCS2_to_ISO88595_swap(unsigned char  *dest,unsigned short *sour,_dword len);
void dconvert_ISO88595_to_UCS2     (unsigned short *dest,unsigned char  *sour,_dword len);
void dconvert_ISO88595_to_UCS2_swap(unsigned short *dest,unsigned char  *sour,_dword len);

void dconvert_ascii_to_gsm_        (unsigned char  *dest,unsigned char  *sour,_dword len);
void dconvert_gsm_to_ascii_        (unsigned char  *dest,unsigned char  *sour,_dword len);
_dword dconvert_ascii_to_gsm(unsigned char  *dest,_dword max,unsigned char *sour,_dword len);
_dword dconvert_gsm_to_ascii(unsigned char  *dest,_dword max,unsigned char *sour,_dword len);

_dword dconvert_gsm_to_UCS2      (unsigned short *dest,_dword max,unsigned char  *sour,_dword len);
_dword dconvert_gsm_to_UCS2_swap (unsigned short *dest,_dword max,unsigned char  *sour,_dword len);
void   dconvert_gsm_to_UCS2_     (unsigned short *dest           ,unsigned char  *sour,_dword len);
void   dconvert_gsm_to_UCS2_swap_(unsigned short *dest           ,unsigned char  *sour,_dword len);
_dword dconvert_UCS2_to_gsm      (unsigned char  *dest,_dword max,unsigned short *sour,_dword len);
void   dconvert_UCS2_to_gsm_     (unsigned char  *dest           ,unsigned short *sour,_dword len);

void dconvert_8b_to_ASCII         (unsigned char  *dest,unsigned char  *sour,dword len);

_dword dconvert_UCS2_to_UTF8     (unsigned char  *dest,_dword max,unsigned short *sour,_dword len);
_dword dconvert_UCS2_to_UTF8_swap(unsigned char  *dest,_dword max,unsigned short *sour,_dword len);
_dword dconvert_UTF8_to_UCS2     (unsigned short *dest,_dword max,unsigned char  *sour,_dword len);
_dword dconvert_UTF8_to_UCS2_swap(unsigned short *dest,_dword max,unsigned char  *sour,_dword len);


#ifdef LINUX
}
#endif

#endif
