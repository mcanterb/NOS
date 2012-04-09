#include "memman.h"
#include "video.h"
#include "utility.h"
#include "kmalloc.h"

unsigned int* pagedirectory = (unsigned int*)0x7FF000;
unsigned int* pagetables = (unsigned int*)0x400000;
unsigned char* allocator = (unsigned char*)0x10000;
unsigned char* tableAllocator = (unsigned char*)0x31000;
unsigned int _memAvail = 0;
unsigned int _memTotal = 0;


void WriteCR0(int cr0)
{
	__asm__("mov %%eax, %%cr0" :: "a" (cr0));
}

int ReadCR0()
{
	unsigned int result;
	__asm__ ("mov %%cr0, %%eax" : "=a" (result));
    return result;
}

void WriteCR3(int cr3)
{
	__asm__("mov %%eax, %%cr3" :: "a" (cr3));
}

int ReadCR3()
{
	unsigned int result;
	__asm__ ("mov %%cr3, %%eax" : "=a" (result));
    return result;
}

/*int AddPageTableEntry(unsigned int* table, unsigned int page)
{
	if(page > 1023) return 0;
	pagetable[page] = (i*4096) | 3;	
}

int RemovePageTableEntry(unsigned int* table, unsigned int page)
{
	if(page > 1023) return 0;
	pagetable[i] = (i*4096) | 
}


int MapPage(unsigned int physical, unsigned int virtual, unsigned short flags)
{
	if(pagedirectory[virtual>>22])
		pagedirectory[virtual>>22][(virtual&0x003FFFFF)>>12] = (physical&0xFFFFF000)|flags;
	else
		CreatePageTable(virtual>>22)
	
}
*/

void MarkTableAvailable(int index)
{
	tableAllocator[index>>3] |= (1<<((index)%8));


}

void MarkTableUnavailable(int index)
{
	tableAllocator[index>>3] &= ~(1<<((index)%8));
}


void AddTableToDirectory(unsigned int* pagetable, unsigned int* pagedirectory, short index, unsigned short flags)
{
	pagedirectory[index] = (((unsigned int)pagetable)&0xFFFFF000) | flags;
}

/*****
* Creates an Empty Page table allocated at the next available
* Page Table Slot. 
* Returns the Physical Address to the new Page Table.
*
*/
unsigned int* CreateNewPageTable()
{
	int i = 0,j,done=0;
	unsigned int *pagetable;
	while(tableAllocator[i] == 0x00)
	{
		i++;
	}
	unsigned char mask;
	unsigned char index=0;
	for(mask = 0x01; mask!=0x00; mask = mask<<1)
	{
		if(tableAllocator[i]&mask) 
		{
			break;
		}
		index++;
	}
	
	MarkTableUnavailable((i<<3)|(index));
	pagetable = (unsigned int*)(((unsigned int)pagetables) + (((i<<3)|(index))<<12));
	
	for(i = 0; i < 1024; i++)
	{
		pagetable[i] = 0;	
	}

	return pagetable;
}

void CreatePageDirectory(unsigned int* pagedirectory)
{
	int i;
	//pagedirectory[1023] = ((unsigned int)0x400000) | MATTR_SUPER_RW_PRESENT; //Always map the top page to the page directory
	for(i = 0; i < 1023; i++)
	{
		pagedirectory[i] = 0 | 2; //Supervisor, r/w, not present	
	}
	unsigned int* table = CreateNewPageTable();

	pagedirectory[1023] = ((((unsigned int)table)-((unsigned int)pagetables))+0x400000) | MATTR_SUPER_RW_PRESENT;
	for(i = 0; i < 1024; i++)
	{
		table[i] = (0x400000 + i*4096) | MATTR_SUPER_RW_PRESENT;
	}
}

void MarkAvailable(int physical)
{
	allocator[physical>>15] |= (1<<((physical>>12)%8));
}

void MarkUnavailable(int physical)
{
	allocator[physical>>15] &= ~(1<<((physical>>12)%8));
}

void CreateAllocatorStruct()
{	
	struct MemSegment *memMap = (struct MemSegment *)0x1004;
	unsigned int length = *((int *)0x1000);
	int i;
	int j;
	memset(allocator, 0, 0x20000); 		//Initially set all memory to unavailable
	
	for(i = 0; i < length; i++) 		//Then set all memory reported by bios as available
	{
		if(memMap->type == MTYPE_AVAIL)
		{
			_memAvail+=memMap->length;
			for(j=0;j < (memMap->length>>12); j++)
			{
				if(memMap->base+(j<<12) >= 0x100000) //Forbid allocator to allocate under 1 meg
				{
					MarkAvailable((memMap->base) + (j<<12));
				}
			}
		}
		_memTotal+=memMap->length;
		memMap = (struct MemSegment *)(((unsigned char*)(memMap))+20);
	}
}


unsigned int AllocateContiguousPages(unsigned short pages)
{
	int i = 0,j,done=0,pageIndex=0;
	unsigned char mask;
	unsigned int pagesFound=0;
	unsigned int startPage = 0;
	
	if(pages > 65534) return 0;
	
	while(!done && i < 32768)
	{
		pageIndex=0;
		for(mask = 1; mask != 0x00; mask = mask << 1)
		{
			if(allocator[i]&mask)
			{
				if(pagesFound == 0) 
				{
					startPage = ((i<<15)|(pageIndex<<12));
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
	if(i == 32768) return null;

	for(i = 0; i < pages; i++)
	{
		MarkUnavailable(startPage + (i<<12));

	}
		
	return startPage;

}


/****************************************
* Low level physical allocate function.
* Returns physical address of allocated
* page.
*/
unsigned int AllocatePage()
{
	int i = 0,j,done=0;
	unsigned int *intAlloc = (unsigned int *)allocator;
	while(intAlloc[i] == 0x00000000)
	{
		i++;
	}
	i=i<<2;
	unsigned char mask;
	unsigned char page;
	for(j = 0; j < 4; j++)
	{
		page = 0;
		for(mask = 0x01; mask!=0x00; mask = mask<<1)
		{
			if(allocator[i]&mask) 
			{
				done = 1;
				break;
			}
			page++;
		}
		if(done) break;
		i++;
	}
	MarkUnavailable((i<<15)|(page<<12));
	return (i<<15)|(page<<12);
}


/**
* Map the physical page of memory into virtual address space using the
* given page directory.
*/
int PageMap(unsigned int physical, unsigned int virtual, unsigned int *pagedirectory)
{
	unsigned int* pagetable;
	if((virtual&0xFFC00000) == 0xFFC00000)	return -1;
	if(!(pagedirectory[virtual>>22]&1))
	{
		pagetable = CreateNewPageTable();
		pagedirectory[virtual>>22] = ((((unsigned int)pagetable)-((unsigned int)pagetables))+0x400000) | MATTR_SUPER_RW_PRESENT;
	} 
	else
	{
		pagetable = (unsigned int*)((pagedirectory[virtual>>22]&0xFFFFF000)-0x400000+((unsigned int)pagetables));
	}

	pagetable[(virtual&0x003FF000)>>12] = (physical & 0xFFFFF000) | MATTR_SUPER_RW_PRESENT; 
	return 0;
}

void MMInit()
{
	int i;
	unsigned int temp = 0;
	const unsigned int *kernelSize = (unsigned int *)0x1804;
	unsigned char buffer[12];

	CreateAllocatorStruct();
	
	for(i = 0x100000; i < ((*kernelSize)<<9)+0x100000; i=i+4096) /*Do not allocate over the kernel*/
	{
		MarkUnavailable(i);
	}
	for(i = 0x400000; i < 0x800000; i=i+4096) /*Reserve Space for PageTables*/
	{
		MarkUnavailable(i);
	}
	memset(tableAllocator, 0xFF, 0x80); 		/*Initially set all PageSlots to available*/

	CreatePageDirectory(pagedirectory);
	for(i = 0; i < 1024; i++)
	{
		PageMap(i*4096,i*4096,pagedirectory);	
	}
	

	WriteCR3((int)pagedirectory);
	WriteCR0(ReadCR0() | 0x80000000);
	pagetables = 0xFFC00000;
	pagedirectory = 0xFFFFF000;
	InitKMalloc();
	for(i = 0; i < 256; i++)
	{
		PageMap(i<<12, 0xF0000000 + (i<<12), pagedirectory);
	}
}

int MMGetAvailable()
{
	return _memAvail;
}

int MMGetTotal()
{
	return _memTotal;
}

int MMAllocatePage(unsigned int virtual)
{
	unsigned int phys = AllocatePage();
	return PageMap(phys, virtual, (unsigned int*)0xFFFFF000);
}


int MMFreePage(unsigned int virtual)
{
	if((virtual&0xD0002000) < 0xD0002000 || (virtual&0xDFFFF000) > 0xDFFFF000)
	{
		return -1;
	}
	unsigned int* pagetables = 0xFFC00000;
	unsigned int* pagedirectory = 0xFFFFF000;
	unsigned int* pagetable = (unsigned int)((pagedirectory[virtual>>22]&0xFFFFF000)-0x400000+((unsigned int)pagetables));
	unsigned int phys = pagetable[(virtual&0x003FF000)>>12]&0xFFFFF000;
	MarkAvailable(phys);
}

int MMPageMap(unsigned int physical, unsigned int virtual)
{
	return PageMap(physical, virtual, (unsigned int*)0xFFFFF000);
}

unsigned int MMAllocateDMABuffer(unsigned int virt, unsigned int size)
{
    unsigned int phys = AllocateContiguousPages(size);
    int i;
    if(phys + size*0x1000 > 0x1000000) 
    {
        for(i = 0; i < size; i++)
        {
            MarkAvailable(phys + (i<<12));
        }
        return 0;
    }
    for(i = 0; i < size; i++)
    {
        PageMap(phys + (i << 12), virt + (i<<12), (unsigned int*)0xFFFFF000);
    }
    return phys;
}