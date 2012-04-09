#define time_t long


typedef struct
{
	unsigned short owner;
	unsigned short group;
	time_t creation;
	time_t modified;
	time_t accessed;
	unsigned short permissions;
    unsigned char numOfLinks
}__attribute__ ((packed))  Inode

typedef struct
{
    

    
} SuperBlock;



typedef struct
{
            
        
	
} fsInterface;



void FSInit();
void Mount();
