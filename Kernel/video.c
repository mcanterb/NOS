#include "video.h"
#include "ports.h"

void clrscr()
{
	unsigned char *vidmem = (unsigned char *)0xB8000;
	long loop;
	for(loop=0; loop<80*25; loop++)
	{
		*vidmem++ = 0;
		*vidmem++ = 0xF0;
	}
	out(0x3D4, 14);
	out(0x3D5, 0);
	out(0x3D4, 15);
	out(0x3D5, 0);
}

void print(char *string)
{
	unsigned char *vidmem = (unsigned char *)0xB8000;
	unsigned short offset = 0;
	long i = 0;
	out(0x3D4, 0xE);
	offset = (in(0x3D5) << 8);
	out(0x3D4, 0xF);
	offset |= in(0x3D5);
	vidmem += offset * 2;
	while(string[i] != 0)
	{
		if(string[i] == '\n')
		{
			short temp = 0xA0-(((int)vidmem - 0xB8000)%0xA0);
			offset+= (temp>>1)-1;
			vidmem+=temp;
		}
		else if(string[i] == '\b')
		{		
			if((((unsigned int)vidmem)-0xB8000)%80 != 0)
			{
				vidmem-=2;
				*vidmem = ' ';
				offset-=2;
			}
			else offset--;
		}else
		{
			*vidmem++ = string[i];
			vidmem++;
		}
		i++;
	}
	offset+=i;
	out(0x3D5, (unsigned char)(offset));
	out(0x3D4, 0xE);
	out(0x3D5, (unsigned char)(offset>>8));
}