#include <stdio.h>




int main(int argc, char *args[])
{
	FILE* bootloader;
	FILE* img;
	
	if(argc < 3)
	{
		printf("Usage: injectboot <bootloader.bin> <diskimage.img>\n");
		return 0;
	}
	
	bootloader = fopen(args[1], "rb");
	img = fopen(args[2], "r+b");
	
	if(bootloader == NULL)
	{
		printf("Could not open %s. Are you sure it exists?\n", args[1]);
		return 0;
	}
	if(img == NULL)
	{
		printf("Could not open %s. Are you sure it exists?\n", args[2]);
		return 0;
	}
	char buffer[448];
	fread(buffer, 1, 448, bootloader);
	fseek(img, 0x3e, SEEK_SET);
	fwrite(buffer, 1, 448, img);
	
	fclose(img);
	fclose(bootloader);


}