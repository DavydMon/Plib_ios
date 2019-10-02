





#include <string.h>



typedef unsigned char *POINTER;


typedef unsigned short UINT2;


typedef unsigned int UINT4;




typedef struct
{
    UINT4 state[4];             
    UINT4 count[2];             
    unsigned char buffer[64];   
} MD5_CTX;

void MD5Init (MD5_CTX *);
void MD5Update (MD5_CTX *, unsigned char *, unsigned int);
void MD5Final (unsigned char[16], MD5_CTX *);


#define MD5_memcpy(output,input,len) 	memcpy(output,input,len)
#define MD5_memset(output,input,len) 	memset(output,input,len)
