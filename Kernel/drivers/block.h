



/**
* Registers a Block Device with the kernel. Creates a device node in /dev.
* Params: device - pointer to a BDevice struct describing the Block Device
*/
void RegisterBlockDevice(BDevice *device);



typedef struct
{
    unsigned char deviceName[32]; //Filled in by driver (eg. "Floppy Disk")
    unsigned char MajorNumber; //Filled in by the driver. This corresponds to a specific device node
    unsigned char MinorNumber; //Filled in by the driver(if applicable). This corresponds to a specific device node.
    void (*AddRequest)(IORequest*); //Function that handles adding IO requests to the devices queue
    void (*EndRequest)(int);        //Function that signals that the most recent request has been serviced. 
     
} BDevice;