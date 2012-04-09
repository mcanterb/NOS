#include "floppy.h"

#include "utility.h"
#include "ports.h"
#include "memman.h"
#include "kmalloc.h"
#include "timer.h"
#include "interrupts.h"

unsigned char * driveTypes[8] = {
    "none",
    "360kB 5.25\"",
    "1.2MB 5.25\"",
    "720kB 3.5\"",

    "1.44MB 3.5\"",
    "2.88MB 3.5\"",
    "unknown type",
    "unknown type"
};

static volatile unsigned char primaryFloppyType;
static volatile unsigned char secondaryFloppyType;
static volatile char __waiting = 0;
static volatile int floppyMotorState[2] = {0,0};
static volatile int timers[2];
static volatile unsigned char* floppyDMABuf;
static volatile unsigned int floppyDMAPhys;

void FloppyMotor(unsigned short base, FloppyMotorStatus onoff);


void FloppyInterruptHandler(struct regs *r)
{
    __waiting--;
}

void FloppyIRQWait()
{
	__waiting++;
    while(__waiting > 0);
}


void FloppyOut(unsigned short base, unsigned short reg, unsigned char data)
{
	sleep(1);
    out(base+reg, data);
}

unsigned char FloppyIn(unsigned short base, unsigned short reg)
{
	sleep(1);
    return in(base+reg);
}



//
// The MSR byte: [read-only]
// -------------
//
//  7   6   5    4    3    2    1    0
// MRQ DIO NDMA BUSY ACTD ACTC ACTB ACTA
//
// MRQ is 1 when FIFO is ready (test before read/write)
// DIO tells if controller expects write (1) or read (0)
//
// NDMA tells if controller is in DMA mode (1 = no-DMA, 0 = DMA)
// BUSY tells if controller is executing a command (1=busy)
//
// ACTA, ACTB, ACTC, ACTD tell which drives position/calibrate (1=yes)
//
//

void FloppyCommand(unsigned short base, unsigned char command)
{
	int i;
	for(i = 0; i < 200; i++)
	{
		sleep(1);
		if(0x80 & FloppyIn(base, FLOPPY_MAIN_STATUS_REG))
		{
			return FloppyOut(base, FLOPPY_DATA_REGISTER, command);
		}
	}
	print("Floppy Timeout!!\n");
}

unsigned char FloppyReadData(unsigned short base)
{
	int i;
	for(i = 0; i < 200; i++)
	{
		sleep(1);
		if(0x80 & FloppyIn(base, FLOPPY_MAIN_STATUS_REG))
		{
			return FloppyIn(base, FLOPPY_DATA_REGISTER);
		}
	}
	print("Floppy Timeout!!\n");
	return 0;
}

void GetFloppyParams(floppyparams *out)
{
	memcpy((void*)FLOPPY_PARAM_ADDRESS, (void*)out, sizeof(floppyparams));
}

void FloppyCheckInterrupt(unsigned short base, int* st0, int* cyl)
{
	FloppyCommand(base, FLOPPY_CHECK_INTERRUPT_STATUS);
	
	*st0 = FloppyReadData(base);
	*cyl = FloppyReadData(base);	
}

int FloppyCalibrate(unsigned short base)
{
	int i, st0, cyl = -1;
    int drive = ((base == FLOPPY_PRIMARY)?0:1);

	FloppyMotor(base, FLOPPY_MOTOR_ON);

	for(i = 0; i < 10; i++)
	{
		
		FloppyCommand(base, FLOPPY_CALIBRATE_DRIVE);
        FloppyCommand(base, drive);

        FloppyIRQWait();
        FloppyCheckInterrupt(base, &st0, &cyl);

        if(st0 & 0xC0) 
        {
            continue;
        }

        if(!cyl)
        {
            FloppyMotor(base, FLOPPY_MOTOR_OFF);
            return 0;
        }
	}

    FloppyMotor(base,FLOPPY_MOTOR_OFF);
    return -1;
}

int ResetFloppyController(unsigned short base)
{
	out(base+FLOPPY_DIGITAL_OUTPUT_REG, 0x00);
	out(base+FLOPPY_DIGITAL_OUTPUT_REG, 0x0C);
	
	FloppyIRQWait();							//Wait for controller to reinitialize
	{	
		int st0,cyl;
		FloppyCheckInterrupt(base, &st0, &cyl); //Clear interrupt
	}
	FloppyOut(base, FLOPPY_CONFIG_CONTROL_REG, 0x00);
	
	FloppyCommand(base, FLOPPY_FIX_DRIVE_DATA);
	FloppyCommand(base, 0xDF);
	FloppyCommand(base, 0x02);

	if(FloppyCalibrate(base)) return -1;
	return 0;

}

void FloppyMotor(unsigned short base, FloppyMotorStatus onoff)
{
    int drive = ((base == FLOPPY_PRIMARY)?0:1);
    if(onoff)
    {
        if(!floppyMotorState[drive])
        {
            FloppyOut(base, FLOPPY_DIGITAL_OUTPUT_REG, 0x1C);
            sleep(50);
        }
        floppyMotorState[drive] = FLOPPY_MOTOR_ON;
    }
    else
    {
        ResetTimer(timers[drive], 300);
        floppyMotorState[drive] = FLOPPY_MOTOR_WAIT;
    }
    
}

void FloppyMotorKill(int base)
{
    int drive = ((base == FLOPPY_PRIMARY)?0:1);
    FloppyOut(base, FLOPPY_DIGITAL_OUTPUT_REG, 0x0C);
    floppyMotorState[drive] = FLOPPY_MOTOR_WAIT;
}

int FloppySeek(unsigned short base, unsigned int cyli, unsigned int head)
{
    unsigned int i, st0, cyl = -1;

    FloppyMotor(base, FLOPPY_MOTOR_ON);

    for(i = 0; i < 10; i++)
    {
        FloppyCommand(base, FLOPPY_SEEK_TRACK);
        FloppyCommand(base, head << 2);
        FloppyCommand(base, cyli);
        FloppyIRQWait();
        FloppyCheckInterrupt(base, &st0, &cyl);

        if(cyl == cyli)
        {
            FloppyMotor(base, FLOPPY_MOTOR_OFF);
            return 0;
        }
        
    }
    print("FloppySeek(): Failed to Seek Track!\n");
    FloppyMotor(base, FLOPPY_MOTOR_OFF);
    return -1;
}

static void FloppyDMAInit(FloppyDirection dir)
{
    union {
        unsigned char b[4];
        unsigned int l;
    } a, c;

    a.l = floppyDMAPhys;
    if(a.l == 0) 
    {
        print("FloppyDMAInit(): Could not allocate DMA Buffer!\n");
        return;
    }
    c.l = (unsigned int)0x4800 - 1;
    unsigned char mode;
    switch(dir)
    {
    case FLOPPY_DIR_READ: mode = 0x46; break;
    case FLOPPY_DIR_WRITE: mode = 0x4A; break;
    default: print("FloppyDMAInit(): Attempt to use invalid direction! Aborting!\n");
        return;
    }
    out(0x0A, 0x06);
    out(0x0C, 0xFF);
    out(0x04, a.b[0]);
    out(0x04, a.b[1]);
    
    out(0x81, a.b[2]);

    out(0x0C, 0xFF);
    out(0x05, c.b[0]);
    out(0x05, c.b[1]);
    
    out(0x0B, mode);
    
    out(0x0A, 0x02);
}

int FloppyDoTrack(unsigned short base, unsigned int cyl, FloppyDirection dir)
{
    unsigned char cmd;
    
    static const int flags = 0xC0;
    switch(dir)
    {
    case FLOPPY_DIR_READ:
        cmd = FLOPPY_READ | flags;
        break;
    case FLOPPY_DIR_WRITE:
        cmd = FLOPPY_WRITE | flags;
        break;
    default:
        print("FloppyDoTrack(): Invalid Direction Specified! Aborting!\n");
        return 0;
    }

    if(FloppySeek(base, cyl, 0)) return -1;
    if(FloppySeek(base, cyl, 1)) return -1;
    
    int i;
    for(i = 0; i < 20; i++)
    {
        FloppyMotor(base, FLOPPY_MOTOR_ON);
        FloppyDMAInit(dir);
        sleep(10);
        FloppyCommand(base, cmd);
        FloppyCommand(base, 0);
        FloppyCommand(base, cyl);
        FloppyCommand(base, 0);
        FloppyCommand(base, 1);
        FloppyCommand(base, 2);
        FloppyCommand(base, 18);
        FloppyCommand(base, 0x1B);
        FloppyCommand(base, 0xFF);

        FloppyIRQWait();

        unsigned char st0, st1, st2, rcy, rhe, hse, bps;
        st0 = FloppyReadData(base);
        st1 = FloppyReadData(base);
        st2 = FloppyReadData(base);

        rcy = FloppyReadData(base);
        rhe = FloppyReadData(base);
        hse = FloppyReadData(base);

        bps = FloppyReadData(base);

        int error = 0;

        if(st0 & 0xC0) {
            static const char * status[] =
            { 0, "error", "invalid command", "drive not ready" };
            print("FloppyDoSector: status = ");
            print(status[st0 >> 6]);
            print("\n");
            error = 1;
        }
        if(st1 & 0x80) {
            print("FloppyDoSector: end of cylinder\n");
            error = 1;
        }
        if(st0 & 0x08) {
            print("FloppyDoSector: drive not ready\n");
            error = 1;
        }
        if(st1 & 0x20) {
            print("FloppyDoSector: CRC error\n");
            error = 1;
        }
        if(st1 & 0x10) {
            print("FloppyDoSector: controller timeout\n");
            error = 1;
        }
        if(st1 & 0x04) {
            print("FloppyDoSector: no data found\n");
            error = 1;
        }
        if((st1|st2) & 0x01) {
            print("FloppyDoSector: no address mark found\n");
            error = 1;
        }
        if(st2 & 0x40) {
            print("FloppyDoSector: deleted address mark\n");
            error = 1;
        }
        if(st2 & 0x20) {
            print("FloppyDoSector: CRC error in data\n");
            error = 1;
        }
        if(st2 & 0x10) {
            print("FloppyDoSector: wrong cylinder\n");
            error = 1;
        }
        if(st2 & 0x04) {
            print("FloppyDoSector: uPD765 sector not found\n");
            error = 1;
        }
        if(st2 & 0x02) {
            print("FloppyDoSector: bad cylinder\n");
            error = 1;
        }
        if(bps != 0x2) {
            print("FloppyDoSector: wanted 512B/sector. Got something different.\n");
            error = 1;
        }
        if(st1 & 0x02) {
            print("FloppyDoSector: not writable\n");
            error = 2;
        }
        
        if(!error)
        {
            FloppyMotor(base, FLOPPY_MOTOR_OFF);
            return 0;
        }

        if(error > 1)
        {   
            print("FloppyDoSector: not retrying...\n");
            FloppyMotor(base, FLOPPY_MOTOR_OFF);
            return -2;
        }

    }

    print("FloppyDoTrack(): 20 retries exhausted.\n");
    FloppyMotor(base, FLOPPY_MOTOR_OFF);
    return -1;   
}

int FloppyReadTrack(unsigned short base, unsigned int cyl)
{
    return FloppyDoTrack(base, cyl, FLOPPY_DIR_READ);
}

int FloppyWriteTrack(unsigned short base, unsigned int cyl)
{
    return FloppyDoTrack(base, cyl, FLOPPY_DIR_WRITE);
}


const unsigned char *GetFloppyDriveType(char drive)
{
	if(drive != 0 && drive != 1) return driveTypes[0];
	 if(drive == 0) return driveTypes[primaryFloppyType];
	 else return driveTypes[secondaryFloppyType];
}

void DetectFloppyDrives()
{
	out(0x70,0x10);
	unsigned char drives = in(0x71);
	secondaryFloppyType = drives & 0xf;
	primaryFloppyType = drives >> 4;
}

void InitFloppy()
{
	DetectFloppyDrives();
	IRQInstallRoutine(6, &FloppyInterruptHandler);
    timers[0] = NewTimer(0, &FloppyMotorKill, FLOPPY_PRIMARY);
    timers[1] = NewTimer(0, &FloppyMotorKill, FLOPPY_SECONDARY);
    if(primaryFloppyType) ResetFloppyController(FLOPPY_PRIMARY);
    if(secondaryFloppyType) ResetFloppyController(FLOPPY_SECONDARY);
    floppyDMAPhys = MMAllocateDMABuffer(0xF0100000,5);
    floppyDMABuf = (unsigned char*)0xF0100000;
    if(FloppyReadTrack(FLOPPY_PRIMARY, 0)) print("Error Reading Track!\n");
    else
    {
        char buffer[12];
        itoh(floppyDMABuf[0], buffer);
        print(floppyDMABuf+3);
        print("\n");
    }

}

