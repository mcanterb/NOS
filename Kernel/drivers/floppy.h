#define FLOPPY_PARAM_ADDRESS 			0xF00FEFC7
#define FLOPPY_PRIMARY		 			0x3F0
#define FLOPPY_SECONDARY				0x370

#define FLOPPY_DIGITAL_OUTPUT_REG      	0x0002
#define FLOPPY_MAIN_STATUS_REG         	0x0004
#define FLOPPY_DATA_REGISTER           	0x0005
#define FLOPPY_CONFIG_CONTROL_REG      	0x0007 
#define FLOPPY_PRIMARY_RESULT_STATUS   	0x0000
#define FLOPPY_SECONDARY_RESULT_STATUS 	0x0000

#define FLOPPY_FIX_DRIVE_DATA          0x03
#define FLOPPY_CHECK_DRIVE_STATUS      0x04
#define FLOPPY_CALIBRATE_DRIVE         0x07
#define FLOPPY_CHECK_INTERRUPT_STATUS  0x08
#define FLOPPY_FORMAT_TRACK            0x4D
#define FLOPPY_READ                    0x06
#define FLOPPY_READ_DELETE_SECTOR      0xCC
#define FLOPPY_READ_SECTOR_ID          0x4A
#define FLOPPY_READ_TRACK              0x42
#define FLOPPY_SEEK_TRACK              0x0F
#define FLOPPY_WRITE                   0x05
#define FLOPPY_WRITE_DELETE_SECTOR     0xC9

typedef enum { FLOPPY_MOTOR_OFF = 0, FLOPPY_MOTOR_ON, FLOPPY_MOTOR_WAIT} FloppyMotorStatus;
typedef enum { FLOPPY_DIR_READ = 1, FLOPPY_DIR_WRITE = 2} FloppyDirection;


typedef struct{
  unsigned char steprateHeadunload;
  unsigned char headloadNdma;
  unsigned char motorDelayFff;
  unsigned char bytesPerSector;
  unsigned char sectorsPerTrack;
  unsigned char gapLength;
  unsigned char dataLength; 
  unsigned char formatGapLength;
  unsigned char filler;
  unsigned char headSettleTime; 
  unsigned char motorStartTime; 
}__attribute__ ((packed)) floppyparams;



void GetFloppyParams(floppyparams *out);
const unsigned char *GetFloppyDriveType(char drive);
void InitFloppy();
