#include "memman.h"
#include "video.h"
#include "utility.h"

unsigned int* virtAllocator = (unsigned int*)0xD0000000;






void MarkVirtualAvailable(unsigned int virtual)
{
	if(virtual < 0xD0002000 || virtual >= 0xDFFFF000)
	{
		return; //Only allowed to mark addresses from 0xD0002000 to 0xE0000000
	}
	((unsigned char*)virtAllocator)[(virtual-0xD0000000)>>15] |= (1<<(((virtual-0xD0000000)>>12)%8));
}

void MarkVirtualUnavailable(unsigned int virtual)
{
	if(virtual < 0xD0002000 || virtual >= 0xDFFFF000)
	{
		return; //Only allowed to mark addresses from 0xD0002000 to 0xE0000000
	}


	((unsigned char*)virtAllocator)[(virtual-0xD0000000)>>15] &= ~(1<<(((virtual-0xD0000000)>>12)%8));

}

unsigned int VirtualAllocateContiguousPages(unsigned short pages)
{
	int i = 0,j,done=0,pageIndex=0;
	unsigned char mask;
	unsigned int pagesFound=0;
	unsigned int startPage = 0;
	unsigned char* alloc = (unsigned char*)virtAllocator;
	
	if(pages > 65534) return 0;
	
	while(!done && i < 8192)
	{
		pageIndex=0;
		for(mask = 1; mask != 0x00; mask = mask << 1)
		{
			if(alloc[i]&mask)
			{
				if(pagesFound == 0) 
				{
					startPage = ((i<<15)|(pageIndex<<12)) + 0xD0000000;
				}
				pagesFound++;
				if(pagesFound == pages)
				{
					done = 1;
				}
			}
			else
			{
				if(pagesFound != 0) pagesFound = 0;
			}
			pageIndex++;
		}
		i++;
	}
	if(i == 8192) return null;

	for(i = 0; i < pages; i++)
	{
		MarkVirtualUnavailable(startPage + (i<<12));

	}
	

	
	return startPage;

}


unsigned int VirtualAllocatePage()
{
	int i = 0,j,done=0;
	while(virtAllocator[i] == 0x00000000 && i < 2048)
	{
		i++;
	}
	if(i == 2048) return 0;
	i=i<<2;
	unsigned char mask;
	unsigned char page;
	for(j = 0; j < 4; j++)
	{
		page = 0;
		for(mask = 0x01; mask!=0x00; mask = mask<<1)
		{
			if(((unsigned char*)virtAllocator)[i]&mask) 
			{
				done = 1;
				break;
			}
			page++;
		}
		if(done) break;
		i++;
	}
	MarkVirtualUnavailable(((i<<15)|(page<<12)) + 0xD0000000);
	return ((i<<15)|(page<<12)) + 0xD0000000;
}

void InitKMalloc()
{
	if(MMAllocatePage((unsigned int)virtAllocator) || MMAllocatePage((unsigned int)virtAllocator + 4096))
	{
		print("PANIC: Could not allocate memory for kmalloc(). Stopping.");
		for(;;);
	}

	memset((unsigned char*)virtAllocator, 0xFF, 0x2000);
	virtAllocator[0] = 0xFFFFFFFC;				//First 2 pages are reserved for allocator.
}

void *kmalloc(unsigned int size)
{
	/*if(size < 0xC00)
	{
		//TODO: Implement Bit Bucket
		return 0;
	}*/
	/*if(size <= 0xFFC)
	{
		unsigned int ret = VirtualAllocatePage();
		if(ret == 0) return null;
		if(MMAllocatePage(ret)) return null;
		((unsigned int*)ret)[0] = size;
		return (void*)(ret+4);
	}
	else
	{*/
		unsigned int pages = ((size+4)>>12)+(((size+4)%4096)==0?0:1);
		unsigned int ret = VirtualAllocateContiguousPages(pages);
		if(ret == 0) return null;
		int i;
		for(i=0; i < pages; i++)
		{
			if(MMAllocatePage(((unsigned int)ret)+(i<<12))) return null;
		}
		((unsigned int*)ret)[0] = size;
		return (void*)(ret+4);
	//}

}

void kfree(void *ptr)
{
		unsigned int size = *((unsigned int*)(((unsigned int)ptr)-4));
		unsigned int pages = ((size+4)>>12)+(((size+4)%4096)==0?0:1);
		int i;
		if(((unsigned int)ptr)< 0xD0002000 || (((unsigned int)ptr)&0xFFFFF000) > 0xDFFFF000) return;
		

		for(i = 0; i < pages; i++)
		{
			MarkVirtualAvailable(((unsigned int)ptr) + (i<<12));
			MMFreePage((unsigned int)ptr);
		}
}