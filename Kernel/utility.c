#include "utility.h"

const unsigned char __convtext[] = "0123456789ABCDEF";

void convertI(unsigned int n, unsigned char *buff, int *i, int radix)
{
	if(n/radix > 0) convertI(n/radix, buff, i, radix);
	buff[(*i)++] = __convtext[n%radix] ;
}


void itoa(unsigned int i, unsigned char *buff)
{
	int temp=0;
	convertI(i, buff, &temp, 10);
	buff[temp] = 0;
}


void convertL(unsigned long n, unsigned char *buff, int *i, int radix)
{
	if(n/radix > 0) convertL(n/radix, buff, i, radix);
	buff[(*i)++] = __convtext[n%radix];
}


void ltoa(unsigned long l, unsigned char *buff)
{
	int temp=0;
	convertL(l, buff, &temp, 10);
	buff[temp] = 0;
}

void memset(unsigned char* dest, unsigned char ch, unsigned int size)
{
	int i;
	for(i=0; i < size; i++)
	{
		*dest++ = ch;
	}
}

void itoh(unsigned int i, unsigned char* buff)
{
	buff[0] = '0';
	buff[1] = 'x';
	int temp=0;
	convertI(i, buff+2, &temp, 16);
	buff[temp+2] = 0;
}

void memcpy(void* s1, const void* s2, unsigned int n)
{
	int i;
	int itts = n/4;
	for(i=0; i<itts; ++i)
	{
		((unsigned int*) s1)[i] = ((unsigned int*) s2)[i];	
	}
	for(i=0; i<n%4; ++i)
	{
		((unsigned char*) s1)[i] = ((unsigned char*) s2)[i];
	}
	
}