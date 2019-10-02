/*
   $Id: 
   $Change: 135243 $
   $DateTime: 2012/05/28 11:50:19 $
   $Author: DavidMon $
*/


#ifndef dstrH
#define dstrH

#include "dmain.h"

#ifdef LINUX
namespace dmon {
#endif


extern const char dlib_hex_symbol_table[16];
extern const char *dlib_word_hex_symbols_table[256];
extern const char *dlib_word_hex_symbols_table_a[256];
extern byte dlib_hex_table[256];

char* dispace         (char *str,bool enter=false);
char* dispace_fixlen  (char *str,dword str_len,bool enter=false,dword *dl=0);
char* dnspace         (char *str);
char* dnspace_fixlen  (char *str,dword str_len,dword *dl=0);
char* dnsymbol_fixlen (char *str,dword str_len,char symbol,bool space=false,bool cs=false,dword *dl=0);
char* dend_line_fixlen(char *str,dword str_len,dword *dl=0);
char* dnext_line_fixlen(char *str,dword str_len,dword *dl=0);
char* diabc           (char *str);
char* dstr_trim		  (char *str,bool enter=false);

void  dstrcpy         (char *str1,char *str2);
void  dstrncpy        (char *str1,char *str2,dword maxlen);
void  dstrcat         (char *str1,char *str2);
dword dstrlen         (char *str);

dword datod           (char *str);
dword daton_array     (byte *data,dword maxlen,char *str,dword len);
byte  ddtoa           (dword value,char *str,byte count=10,byte symbol=0,byte end_space_count=0);
byte  dqtoa           (qword value,char *str,byte count=20,byte symbol=0,byte end_space_count=0);
byte  ddtoaf          (dword value,char *str);
byte  dqtoaf          (qword value,char *str);
byte  ddtoaex         (dword value,char *str,dword str_max_size,byte count=10,byte symbol=0,byte end_space_count=0);
byte  dqtoaex         (qword value,char *str,dword str_max_size,byte count=20,byte symbol=0,byte end_space_count=0);
byte  ddtoafex        (dword value,char *str,dword str_max_size);
byte  dqtoafex        (qword value,char *str,dword str_max_size);
byte  dltoa           (sdword value,char *str,byte end_space_count=0);
byte  dltoaf          (sdword value,char *str);
byte  dltoaex         (sdword value,char *str,dword str_max_size,byte end_space_count=0);
byte  dltoafex        (sdword value,char *str,dword str_max_size);
byte  dsqtoa          (sqword value,char *str,byte end_space_count=0);
byte  dqstoaf         (sqword value,char *str);
byte  dsqtoaex        (sqword value,char *str,dword str_max_size,byte end_space_count=0);
byte  dsqtoafex       (sqword value,char *str,dword str_max_size);

byte  dgetsymbolhex   (byte symbol);
dword datodhex        (char *str);
dword datodhexstr     (char *str,byte *array,dword max=0);
byte  ddtoahex        (dword value,char *str,byte count=8,byte symbol=0,byte end_space_count=0);
byte  ddtoahexf       (dword value,char *str,byte count=8);
byte  dqtoahexf       (qword value,char *str,byte count=16);
byte  ddtoahexex      (dword value,char *str,dword str_max_size,byte count=8,byte symbol=0,byte end_space_count=0);
byte  dqtoahexex      (qword value,char *str,dword str_max_size,byte count=16,byte symbol=0,byte end_space_count=0);
byte  ddtoahexfex     (dword value,char *str,dword str_max_size,byte count=8);
byte  dqtoahexfex     (qword value,char *str,dword str_max_size,byte count=16);
void  ddtoahex_byte   (byte  value,char *str);
void  ddtoahex_byte_a (byte  value,char *str);
void  ddtoahex_word   (word  value,char *str);
void  ddtoahex_word_a (word  value,char *str);
void  ddtoahex_dword  (dword value,char *str);
void  ddtoahex_dword_a(dword value,char *str);
void  dqtoahex_qword  (qword value,char *str);
void  dqtoahex_qword_a(qword value,char *str);

byte  dtokencmp       (char *str,char *token);
char* dtokencmp       (char *str,char *token,byte *ret);
char* dtokencmpl	  (char *str,char *token);

char* dgettoken       (char *str,char *token,dword *size,bool enter=false);
char* dgettoken_param (char *str,char *token,dword *size,dword *value,bool *usevalue);

char* dgetdworddec	       (char *str,dword *value);                           
char* dgetqworddec         (char *str,qword *value);                           
char* dgetdworddec_fixlen  (char *str,dword str_len,dword *value,dword *dl=0); 
char* dgetqworddec_fixlen  (char *str,dword str_len,qword *value,dword *dl=0); 
char* dgetdwordhex	       (char *str,dword *value);                           
char* dgetqwordhex         (char *str,qword *value);                           
char* dgetdwordhex_fixlen  (char *str,dword str_len,dword *value,dword *dl=0); 
char* dgetqwordhex_fixlen  (char *str,dword str_len,qword *value,dword *dl=0); 
bool  dgethex_byte         (char *str,byte *value);					           
bool  dgethex_word         (char *str,word *value);					           
bool  dgethex_dword        (char *str,dword *value);					       
bool  dgethex_qword        (char *str,qword *value);					       
char* dgetdwordex	       (char *str,dword *value);                           
char* dgetqwordex          (char *str,qword *value);                           
char* dgetdwordex_fixlen   (char *str,dword str_len,dword *value,dword *dl=0); 
char* dgetqwordex_fixlen   (char *str,dword str_len,qword *value,dword *dl=0); 
char* dgetbytedumpex       (char *str,byte  *array,dword *len);                
char* dgetworddumpex       (char *str,word  *array,dword *len);                
char* dgetdworddumpex      (char *str,dword *array,dword *len);                
char* dgetbytedumphex      (char *str,byte  *array,dword *len);                
char* dgetbytedumpex_fixlen(char *str,dword str_len,byte *array,dword *dl=0);  

char* dgetbytedecex        (char *str,byte *value,char end_symbol);            
char* dgetbytedecex_fixlen (char *str,dword str_len,byte *value,char end_symbol,dword *dl=0);

char* diptostr		           (char *str,dword ip);
char* diptostrex               (char *str,dword max_len,dword ip,dword *dl);
char* dipporttostr	           (char *str,dword ip,word port);
char* dstrtoip                 (char *str,dword *ip);
char* dporttostrex             (char *str,dword max_len,word port,dword *dl);
char* dipporttostrex           (char *str,dword max_len,dword ip,word port,dword *dl);
char* dstrtoip_swap            (char *str,dword *ip);
char* dstrtoipport             (char *str,dword *ip,word *port);
char* dstrtoipport_swap        (char *str,dword *ip,word *port);
char* dstrtoip_fixlen          (char *str,dword str_len,dword *ip,dword *dl=0);
char* dstrtoip_swap_fixlen     (char *str,dword str_len,dword *ip,dword *dl=0);
char* dstrtoipport_fixlen      (char *str,dword str_len,dword *ip,word *port,dword *dl=0);
char* dstrtoipport_swap_fixlen (char *str,dword str_len,dword *ip,word *port,dword *dl=0);
char* dstrtoport_fixlen        (char *str,dword str_len,word *port,dword _dl=0,dword *dl=0);



inline char* dgetdec(char *str, dword *value) { return dgetdworddec(str, value); } 
inline char* dgetdec(char *str, qword *value) { return dgetqworddec(str, value); }
inline char* dgethex(char *str, dword *value) { return dgetdwordhex(str, value); } 
inline char* dgethex(char *str, qword *value) { return dgetqwordhex(str, value); }
inline char* dgetex (char *str, dword *value) { return dgetdwordex (str, value); } 
inline char* dgetex (char *str, qword *value) { return dgetqwordex (str, value); }

#ifdef LINUX
}
#endif

#endif
