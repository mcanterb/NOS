
#define MTYPE_AVAIL 1
#define MTYPE_RESERVED 2
#define MTYPE_ACPI_RECLAIM 3
#define MTYPE_ACPI_NVS 4

#define MATTR_USER_RW_PRESENT 7
#define MATTR_USER_RW_NOTPRESENT 6
#define MATTR_USER_R_PRESENT 5
#define MATTR_USER_R_NOTPRESENT 4
#define MATTR_SUPER_RW_PRESENT 3
#define MATTR_SUPER_RW_NOTPRESENT 2
#define MATTR_SUPER_R_PRESENT 1
#define MATTR_SUPER_R_NOTPRESENT 0

struct MemSegment
{
	unsigned int base;
	unsigned int baseHi;
	unsigned int length;
	unsigned int lengthHi;
	unsigned int type;
} __attribute__((packed));

/**
* Initializes the Memory Manager. Must be called before any other
* functions are called. Paging is enabled upon return.
*/
void MMInit();

/**
* Returns the total amount of RAM reported by the system.
*/
int MMGetTotal();

/**
* Returns the amount of RAM available to the OS. Note this does
* not reflect how much RAM is being used by the OS.
*/
int MMGetAvailable();

/**
* Allocates a new page of memory mapped to the specified
* virtual address. 
* Returns an error code. 0 means no error.
*/
int MMAllocatePage(unsigned int virt);

/**
* Used to get a buffer that can be used for DMA.
* virt specifies the virtual address to map the buffer to.
* size indicates the size of the buffer in terms of pages 
* (so size=2 means a buffer of 8kB).
* Returns the Physical Address of the allocated buffer.
* Returns 0 on error. 
*/
unsigned int MMAllocateDMABuffer(unsigned int virt, unsigned int size);

/**
* Unmaps the page at the specified virtual address
* and marks the page as free space.
* Returns an error code. 0 means no error.
*/
int MMFreePage(unsigned int virt);

/**
* Maps the page specified by the phyiscal address
* to the specified virtual address.
* Returns an error code. 0 means no error.
*/
int MMPageMap(unsigned int phys, unsigned int virt);
