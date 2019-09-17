#ifndef _ScsiProtocolParser_H_
#define _ScsiProtocolParser_H_


#ifndef _SCSI_PROT_H  // scsi_prot.h

#define SCSICmdCode unsigned char
#define SCSIRetCode unsigned char

#define USB_DMA_ADDR(x) ( x )
 

#define SCSI_LOB(w) ((unsigned char) ((w) & 255))
#define SCSI_HIB(w) ((unsigned char)((w) >> 8))

// define Reserve/Release Units
#define RESERVE_UNIT_PAGE_START      0
#define RESERVE_UNIT_JOB_START       2
#define RELEASE_UNIT_PAGE_END        0
#define RELEASE_UNIT_ABORT           1
#define RELEASE_UNIT_JOB_END         2

// define SCSI command
#define SCSI_CMD_INVAL          0xFF
#define SCSI_CMD_TESTUNITREADY  0x00
#define SCSI_CMD_REQUESTSENSE   0x03
#define SCSI_CMD_MEDIACHECK     0x08
#define SCSI_CMD_INQUIRY        0x12
#define SCSI_CMD_RESERVEUNIT    0x16
#define SCSI_CMD_RELEASEUNIT    0x17
#define SCSI_CMD_SCAN           0x1B
#define SCSI_CMD_SENDDIAG       0x1D
#define SCSI_CMD_SETWINDOW      0x24
#define SCSI_CMD_READ           0x28
#define SCSI_CMD_RW_MEM  	    0x29
#define SCSI_CMD_SEND           0x2A
#define SCSI_CMD_OBJPOS         0x31
#define SCSI_CMD_BUFSTATUS      0x34
#define SCSI_CMD_HWTEST			0x40

#define SCSI_CMD_FLEXRISC_READ  0x26
#define SCSI_CMD_FLEXRISC_SEND  0x27

#define SCSI_CMD_DUMPSD			0x3A

#define SCSI_CMD_PRINTER_READ  0x41
#define SCSI_CMD_PRINTER_SEND  0x42


typedef enum enumOperationCode
{
	/** Capability counter etc.*/
	eQueryCapability	=	0x50,	//Query Capability
	eQueryQueuingInfo,				//Query Queuing Information
	eQueryReportInfo,				//Query Report Information
	eQueryCounterInfo,				//Query Counter Information
	eWrtieCounterInfo,				//Write Counter Information
	eFirmwareVersion,				//Query Firmware Version Information
	
	/** Error status Report*/
	eQueryErrStatus		=	0x60,	//Query Error Status Report

    /** Specific Operation command*/
    eTriggerCopy        =   0x65,
    eQueryMachineSN,
    eQueryCurrentPrintPageCN,


	
	
	/** Resource Command protocol*/
	eResCMD				=	0x70,   //Present all command set of resource command. it could be ignore the opcode start from 0x71, use this to instead others.
	eResRead			=	0x71,	//Read Operation from Resource
	eResWrite,						//Write Operation from Resource
	eResUpdate,						//Update the Resource by RES Format Data (Write by RES)
	eResBackup,						//Backup (Copy)
	eResInfoQuery,					//Information Query
	eResInfoUpdate,					//Information Update
	
	
	eFuserLampRisingCheck=0x80,
	eFusreLampRisingInfo,
	eFuserTemperature,
	eFuserTemperatureStartRecording,
	eFuserTemperatureStopRecording,
	eFuserTemperatureRetriveStart,
	eFuserTemperatureInfo,
	eFuserTemperatureRetriveEnd,
    eQueryComponentID, //Query Component ID and PNP String.
	

	
    eInvalidOperationCode
}enumOperationCode;


// define SCSI cmd return code
#define SCSI_RET_INVAL          0xFF
#define SCSI_RET_OK             0x00 // return ok
#define SCSI_RET_FAIL           0x01 // return fail
#define SCSI_RET_RS             0x02 // return request sense
#define SCSI_RET_BZ             0x08 // return busy
#define SCSI_RET_END            0x09 // job finished
#define SCSI_RET_TEST           0x10
#define SCSI_RET_FULL           0x20
#define SCSI_RET_EMPTY          0x21

// for easy to implement, the size of SCSI cmd are always 10 bytes , sent
// from driver. 
#define MAX_SCSI_CMD_SIZE       10
#define SCSI_RET_BUF_SIZE	65536

// define requese sense error code
#define SCSI_SENSE_ERR_NO             0x00
#define SCSI_SENSE_ERR_STD            0x70 // for ErrorCode, check ILI : sense key

#define SCSI_SENSE_ILI_NO_SENSE       0x00
#define SCSI_SENSE_ILI_NOT_READY      0x02
#define SCSI_SENSE_ILI_MEDIUM_ERR     0x03
#define SCSI_SENSE_ILI_HARDWARE_ERR   0x04
#define SCSI_SENSE_ILI_BADREQ         0x05
#define SCSI_SENSE_ILI_UNITATTENTION  0x06
#define SCSI_SENSE_ILI_VENDOR         0x09
#define SCSI_SENSE_ILI_ABORTED        0x0b

#define SCSI_SENSE_HOME_SENSOR_ERROR  0x1502
#define SCSI_SENSE_LOCK_ERROR         0x1504
#define SCSI_INVALID_CMD_OPCODE       0x2000
#define SCSI_SENSE_PARAM_INVALID      0x2602
#define SCSI_SENSE_INTERNAL_FAILURE   0x4400
#define SCSI_SENSE_LIGHT_CHECK_ERROR  0x6000
#define SCSI_SENSE_PAPERJAM           0x8001
#define SCSI_SENSE_ADF_OPEN           0x8002
#define SCSI_SENSE_NO_PAPER           0x8003
#define SCSI_SENSE_SCANNER_BUSY       0x9000



#endif // _SCSI_PROT_H




/*SCSI endian transfer*/
#define ScsiSetDouble(var,val)    var[0] = ((val) >> 8 ) & 0xff ; \
                                  var[1] = ((val)      ) & 0xff
							
#define ScsiSetTriple(var,val)    var[0] = ((val) >> 16) & 0xff ; \
                                  var[1] = ((val) >> 8 ) & 0xff ; \
                                  var[2] = ((val) 	 ) & 0xff 

#define ScsiSetQuad(var,val)      var[0] = ((val) >> 24) & 0xff ; \
                                  var[1] = ((val) >> 16) & 0xff ; \
                                  var[2] = ((val) >> 8 ) & 0xff ; \
                                  var[3] = ((val)      ) & 0xff 

#define ScsiGetDouble(var)       ((((Uint32)*(var  )) <<  8) + \
                                   ((Uint32)*(var+1))        )
                                 
#define ScsiGetTriple(var)       ((((Uint32)*(var  )) << 16) + \
                                  (((Uint32)*(var+1)) <<  8) + \
                                  (((Uint32)*(var+2))      ))
						 
                                  
#define ScsiGetQuad(var)         ((((Uint32)*(var  )) << 24) + \
                                  (((Uint32)*(var+1)) << 16) + \
                                  (((Uint32)*(var+2)) <<  8) + \
                                   ((Uint32)*(var+3))       )



// sense data type
typedef struct
{
	unsigned char errorCode;
	unsigned char segNum;     // Segment Number
	unsigned char senseKey;   // FilMrk, EOM, ILI, SenseKey
	unsigned char info[4];
	unsigned char addLength;  // Additional Sense Length
	unsigned char cmdInfo[4]; // Cmd Specific Info
	unsigned char acs;        // Additional Sense Code
	unsigned char ascq;       // Additional Sense Code Qualifier
	unsigned char fruc;       // Field Replaceable Unit Code
	unsigned char sKey[3];    // Sense Key Specific
	unsigned char sByte[4];   // Additional Sense Bytes
	
} stSenseDataType;


#define SENSE_ERROR(sensePtr,key,err)      { \
										   sensePtr->errorCode = SCSI_SENSE_ERR_STD ;   \
										   sensePtr->senseKey = key ;                   \
										   sensePtr->acs = SCSI_HIB(err);               \
										   sensePtr->ascq = SCSI_LOB(err);              \
										   }


#ifndef _SCSI_PROT_H  // scsi_prot.h
// inquiry vender info & product info & basic res , ability
// size = 32
// stInquiryBasicInfo.ability
#define AB_ADF	                (1<<7)     // with adf
#define AB_SCAN	                (1<<5)     // 1pass scan mode
#define AB_BGR	                (1<<0)     // b,g,r sequence, 0h:r, g, b sequence

// stInquiryBasicInfo.ability_ext
#define AB_EXT_TRANS_NO         (1<<7)     // with Trans. function
#define AB_EXT_QMODE_YES        (1<<6)     // quality scan function
#define AB_EXT_NEW_PROT         (1<<2)     // new protocal
#define AB_EXT_AV               (0x03)     // 11b - AVISION, 01b - OEM

// stInquiryExtInfo
#define SCAN_OPT_RES                (1200)
#define SCAN_MAX_RES                (1200)
#define SCAN_OPT_RES_100BASE        (12)
#define SCAN_MAX_RES_100BASE        (12)
#define SCAN_FLATBED_OPT_RES_X      (1200)
#define SCAN_FLATBED_OPT_RES_Y      (600)

#define SCAN_ADF_OPT_RES_X          (1200)
#define SCAN_ADF_OPT_RES_Y          (600)
#define SCANNER_TYPE_FLATBED        (1<<7)
#define SCANNER_TYPE_FLATBED_ADF    (1<<5)

#define SCAN_FLATBED_MAX_W          (2560)	
#define SCAN_FLATBED_MAX_H          (3510)
#define SCAN_ADF_MAX_W              (2560)
#define SCAN_ADF_MAX_H              (4200)

#define ASIC_TYPE_OTI4110	        (0x82)

// stInquiryExtInfo.ability_ext1
#define AB_EXT1_SUP_LC              (1<<7) // suppurt light control
#define AB_EXT1_SUP_BC              (1<<6) // support button control
#define AB_EXT1_SOFT_COLORPACK      (1<<5) // need software do color pack
#define AB_EXT1_SUP_SC              (1<<4) // support software calibration
#define AB_EXT1_NEED_GT             (1<<3) // need download gamma table 
#define AB_EXT1_KEEP_GT             (1<<2) // scanner will keep gamma table
#define AB_EXT1_KEEP_WINDOW         (1<<1) // scanner will keep window
#define AB_EXT1_DIFF_RES            (1<<0) // support diff res in x and y
// stInquiryExtInfo.ability_ext2
#define AB_EXT2_SUP_EC              (1<<7) // support exposure control
#define AB_EXT2_NEED_STC            (1<<6) // need software trigger calibration
#define AB_EXT2_NEED_WPC            (1<<5) // need white paper to do calib.
#define AB_EXT2_SUP_QSC             (1<<4) // support quality/speed calibration
#define AB_EXT2_NEED_TC             (1<<3) // need calib. for transparency
#define AB_EXT2_HAS_PB              (1<<2) // scanner has push button
#define AB_EXT2_NEW_C               (1<<1) // scanner apply new calib. method
#define AB_EXT2_NEED_AM             (1<<0) // adf need mirror image
// stInquiryExtInfo.ability_ext3
#define AB_EXT3_SUP_GW              (1<<7) // support gray white
#define AB_EXT3_SUP_GC              (1<<6) // support gain control
#define AB_EXT3_SUP_TET             (1<<5) // support TET
#define AB_EXT3_SUP_3x3             (1<<4) // support 3x3 matrix
#define AB_EXT3_SUP_1x3             (1<<3) // support 1x3 filter
#define AB_EXT3_SUP_IC              (1<<2) // support index color
#define AB_EXT3_SUP_PS              (1<<1) // support power saving
#define AB_EXT3_SUP_NVM             (1<<0) // support NVRAM



// stInquiryMiscInfo.ability_ext4
#define AB_EXT4_SUP_DA              (1<<7) // support detect accessories
#define AB_EXT4_ADF_BGR             (1<<6)
#define AB_EXT4_SUP_FU              (1<<4) // support flash update
#define AB_EXT4_SUP_LD              (1<<2) // support light detect
// stInquiryMiscInfo.ability_ext5
#define AB_EXT5_NEED_LP             (1<<6) // need software to do line pack

#endif // _SCSI_PROT_H

#ifndef _SCSI_PROT_H  // scsi_prot.h
// The set window image window size is based on 1200 dpi.
#define SCSI_BASE_RES	(1200)
// define set window
#endif // _SCSI_PROT_H

// stSetWindowType
// stSetWindowType.image_comp
#define SETW_LINEART        0
#define SETW_DITHER         1
#define SETW_GRAY           2
#define SETW_COLOR          5

// stSetWindowType.bitset1 -> color, (bitset1&0x38)>>3
#define SETW_NO_FILTER      0
#define SETW_RED_FILTER     1
#define SETW_GREEN_FILTER   2
#define SETW_BLUE_FILTER    3
#define SETW_COLOR_FILTER   4
#define SETW_TRUE_GRAY      6

typedef struct
{
	SCSIRetCode (*TestUnitReady)( void * );
	SCSIRetCode (*ReserveUnit)( void * );
	SCSIRetCode (*ReleaseUnit)( void * );
	SCSIRetCode (*MediaCheck)( void * );
	SCSIRetCode (*Inquiry)( void * );
	SCSIRetCode (*SetWindow)( void * );
	SCSIRetCode (*Send)( void * );
	SCSIRetCode (*Scan)( void * );
	SCSIRetCode (*Read)( void * );
	SCSIRetCode (*RequestSense)( void * );
	SCSIRetCode (*SendDiag)( void * );							

} stScsiOperation;

#ifndef _SCSI_PROT_H  // scsi_prot.h
// define SCSI send command : data type code.
#define SCSI_SDTC_PRINT_DATA    0x40 // send print data
#define SCSI_SDTC_PRINT_INFO    0x41 // send print info
#define SCSI_SDTC_GAMMA         0x81 // set gamma table
#define SCSI_SDTC_CALIB         0x82 // set calibration data
#define SCSI_SDTC_M3x3          0x83 // set 3x3 matrix
#define SCSI_SDTC_NVRAM         0x85 // set nvram data
#define SCSI_SDTC_FLASH         0x86 // set flash data
#define SCSI_SDTC_OUC           0x88 // optical unit control

#define SCSI_SDTC_TARGET_REG    0x89 // send TargetReg
#define SCSI_SDTC_VERIFY_MOTOR  0x8A // send Motor Verify Setting

#define SCSI_SDTC_DVT_STARTSCAN 0x8C // DVT Start Scan
#define SCSI_SDTC_DVT_STOPSCAN  0x8D // DVT Stop Scan
#define SCSI_SDTC_DVT_MOTORMOVE 0x8E // DVT Motor Moving


#define SCSI_SDTC_GAIN          0x91 // send gain value
#define SCSI_SDTC_RDF           0x93 // send raw data format

#define SCSI_SDTC_THREAD_INFO   0x95 // send thread info
#define SCSI_SDTC_COPY_INFO		0x96 // set copy info
#define SCSI_SDTC_RESTART		0x97 // restart MFP
#define SCSI_SDTC_FAX_TEST_INFO	0x98 // set fax test info
#define SCSI_SDTC_GET_ERROR		0x99 // Display error message and clear 

#define SCSI_SDTC_LC            0xA0 // send light source control
#define SCSI_SDTC_BUTTON        0xA1
#define SCSI_SDTC_TIMER         0xA2 // send power-saving timer
#define SCSI_SDTC_ASIC_REG      0xB0 // Send ASIC register value
#define SCSI_SDTC_NVRAM_POINTER 0xD0 // send nvram pointer
#define SCSI_SDTC_FILE_PATH_NAME 0xD1 // send full file name

#define SCSI_SDTC_SCAN2APP      0xc1 // scan to app send command data code

#define SCSI_SDTC_SET_PG_LINEAR		0xe1 // set pregamma linear 
#define SCSI_SDTC_SET_PG_NONLINEAR	0xe2 // set pregamma nonlinear
#define SCSI_SDTC_SET_START_POS		0xe3 // set start scan position
#define SCSI_SDTC_LED_TARGET		0xe4
#define SCSI_SDTC_OFFSET_TARGET		0xe5
#define SCSI_SDTC_SKEW_DISTANCE		0xe6 // set skew distance

// define SCSI read command : data type code
#define SCSI_RDTC_IMAGE         0x00 // Image Data
#define SCSI_RDTC_PRINTER       0x20 // Printer infomation
#define SCSI_RDTC_EOP           0x40 // end of page
#define SCSI_RDTC_CALIB_FMT     0x60 // Get calibration format
#define SCSI_RDTC_CALIB_GRAY    0x61 // Read data for gray calib.
#define SCSI_RDTC_CALIB_COLOR   0x62 // Read data for color calibration
#define SCSI_RDTC_DETECT_ACC    0x64 // Detect accessories
#define SCSI_RDTC_EXPOSURE      0x65 // get exposure info.
#define SCSI_RDTC_CALIB_DARK    0x66 // get dark calib. data
#define SCSI_RDTC_NVRAM_DATA    0x69 // Read NVRAM data
#define SCSI_RDTC_FLASH_INFO    0x6A // Read Flash RAM information
#define SCSI_RDTC_FLASH_DATA    0x6B // Read Flash RAM data
#define SCSI_RDTC_ACCEL_INFO    0x6C // Read acceleration information
#define SCSI_RDTC_RAW_FMT       0x70 // Get RAW data format
#define SCSI_RDTC_RAW_DATA      0x71 // Get Raw data
#define SCSI_RDTC_M3x3          0x83 // Get 3x3 color matrix
#define SCSI_RDTC_OPTICAL       0x88 // Optical unit control

#define SCSI_RDTC_TARGET_REG    	0x89 // read TargetReg
#define SCSI_RDTC_MOTOR_STATUS   0x8A // Read Motor status
#define SCSI_RDTC_MOTOR_REPORT   0x8B // Read Motor Report
#define SCSI_RDTC_DVT_READLINE   0x8C // DVT Read a Line

#define SCSI_RDTC_TEST_IMAGE_INFO	0x93 // read test image infomation
#define SCSI_RDTC_TEST_IMAGE		0x94 // read test image

#define SCSI_RDTC_THREAD_INFO		0x95 // read thread infomation
#define SCSI_RDTC_SHADING_HEADER	0x96 // shading's header
#define SCSI_RDTC_LED_TARGET		0x97
#define SCSI_RDTC_OFFSET_TARGET		0x98


#define SCSI_RDTC_FIRMWARE      0x90 //  Firmware Status
#define SCSI_RDTC_SCANNER       0x92 // Read scanner state
#define SCSI_RDTC_LIGHT         0xA0 // Light source status
#define SCSI_RDTC_BUTTON        0xA1 // button status
#define SCSI_RDTC_SYS_INFO  	0xA3
#define SCSI_RDTC_ASIC_REG      0xB0 // Read ASIC register value

//SCSI_CMD_FLEXRISC_READ's sub command
#define SCSI_FLEX_RISC_RDTC_ACTION_STATE    0x01 //0 ->stop 1 ->run
#define SCSI_FLEX_RISC_RDTC_REG_LIST  		0x04
#define SCSI_FLEX_RISC_RDTC_DATA_MEM  		0x05


//SCSI_CMD_FLEXRISC_SEND's sub command
#define SCSI_FLEX_RISC_SDTC_ACTION_RUN		0x01 
#define SCSI_FLEX_RISC_SDTC_ACTION_STOP		0x02
#define SCSI_FLEX_RISC_SDTC_SINGLE_STEP		0x03
#define SCSI_FLEX_RISC_SDTC_COMMAND			0x04
#define SCSI_FLEX_RISC_SDTC_DATA_MEM  		0x05
#define SCSI_FLEX_RISC_SDTC_BP		  		0x06
#define SCSI_FLEX_RISC_SDTC_WP		  		0x07

#ifdef PANTHER
//Read data
#define SCSI_RDTC_PANTHER_GET_SENSOR_INFO	0xD0
#define PANTHER_GET_SENSOR_INFO_LEN		4	//Unit Byte
#define SCSI_RDTC_PANTHER_GET_A4_A3_THERMISTER_TEMP		0xDA
#define PANTHER_GET_A4_A3_THERMISTER_TEMP_LEN		2	//Unit Byte

//Write data
#define SCSI_SDTC_PANTHER_SET_MOTOR			0xD1
#define SCSI_SDTC_PANTHER_SET_CLUTCH		0xD2
#define SCSI_SDTC_PANTHER_SET_LAMP			0xD3
#define SCSI_SDTC_PANTHER_SET_HIGHV			0xD4
#define SCSI_SDTC_PANTHER_SET_LPH_TURNON_TIMING			0xD5
#define SCSI_SDTC_PANTHER_SET_TRANSFER_TURNON_TIMING	0xD6
#define SCSI_SDTC_PANTHER_SET_A4_FUSER_TEMP				0xD7
#define SCSI_SDTC_PANTHER_SET_FUSER_MOTOR	0xD8	//SET FUSER MOTOR TURN ON TRMPERATURE
#define SCSI_SDTC_PANTHER_SET_A3_FUSER_TEMP	0xD9

#endif  //end of #define PANTHER

// scan to application read command data type code
#define SCSI_RDTC_SCAN2APP      0xC1


#endif //_SCSI_PROT_H


// for stDownloadInfo.tegNum
#define FRTYPE_PHBK				1
#define FRTYPE_FIRMWARE			2
#define FRTYPE_RAWLINE			3
#define FRTYPE_FMDBG			4
#define FRTYPE_DEBUG			5
#define FRTYPE_FONT				6
#define FRTYPE_DEBUG_FILE		7
#define FRTYPE_DEBUG_LEVEL		8
#define FRTYPE_LOADER			9
#define FRTYPE_JBIG				10	// 0xA
#define FRTYPE_REPORT			11	// 0xB
#define FRTYPE_FAX_FW			12	// 0xC
#define FRTYPE_FW_TO_RAM		13	// 0xD
#define FRTYPE_UI_FW			14	// 0xE
#define FRTYPE_STORAGE_FW		15	// 0xF
#define FRTYPE_CASSETTE_FW		16	// 0x10
#define FRTYPE_DEFAULT_SETTING	17	// 0x11
#define FRTYPE_LEARNNING_DATA	18	// 0x12
#define FRTYPE_FIRMWARE_BIND	19	// 0x13
#define	FRTYPE_NETWORK_UPDATE	20	// 0x14
#define FRTYPE_WRITE_NVRAM		21	// 0x15
#define FRTYPE_WRITE_FLASH		22	// 0x16

#define FRTYPE_WEBPAGE			45	// 0x2D
#define FRTYPE_WEBPAGE_LANG		46	// 0x2E

#define FRTYPE_PRINTCALIBRATIONCHAR1		47	// 0x2F
#define FRTYPE_PRINTCALIBRATIONCHAR2		48	// 0x30
#define FRTYPE_QUALITY_CHART				49	// 0x31
#define FRTYPE_SCANNER_FW				    50	// 0x32
#define FRTYPE_PRINTER_FW				    51	// 0x33
#define FRTYPE_PARSER_FW				    52	// 0x34
#define FRTYPE_NET_FW				        53	// 0x35
#define FRTYPE_FILE	   			            54	// 0x36
#define FRTYPE_BATCH_FILE                   55	// 0x37
#define FRTYPE_TAR_GZ_FILE                  56	// 0x38
#define FRTYPE_ZIP_FILE                     57	// 0x39

// for SCSI DUMPSD command :data type code
#define SCSI_DDTC_READ			0x01
#define SCSI_DDTC_SEND			0x02


//	for DumpSD Data Type Qualifier

//	for SCSI_DDTC_READ
#define DUMPTYPE_DUMPSD			1	//trigger dump sd file
#define	DUMPTYPE_GETSIZE		2	//get data size
#define DUMPTYPE_READ			3	//read data
#define DUMPTYPE_ABORT			4	//abort dump
//SCSI_DDTC_SEND
#define DUMPTYPE_TRIGGER_RECV	1	//store sd start
#define DUMPTYPE_SEND			2	//send data
#define	DUMPTYPE_FINISH			3	//store sd end

#ifndef _SCSI_PROT_H  // scsi_prot.h
// define light status, return 1 byte
#define LIGHT_OFF           0x00
#define LIGHT_ON            0x01
#define LIGHT_WARMUP        0x02
#define LIGHT_WARMUP_TEST   0x03
#define LIGHT_FAIL          0x04
#endif //_SCSI_PROT_H

// define detect accessories
typedef struct
{
#ifndef _SCSI_PROT_H  // scsi_prot.h
#define ACC_WITH_ADF	0x01
#define ACC_WITH_DUPLEX 0x02
#endif // _SCSI_PROT_H
	unsigned char adf ;
	unsigned char reserved[7];
	
} stAccessoriesType;


#ifndef _SCSI_PROT_H  // scsi_prot.h

#define FRABILITY_PHBK          (FRTYPE_PHBK       -1)
#define FRABILITY_FIRMWARE      (FRTYPE_FIRMWARE   -1)
#define FRABILITY_RAWLINE       (FRTYPE_RAWLINE    -1)
#define FRABILITY_FMDBG         (FRTYPE_FMDBG      -1)
#define FRABILITY_DEBUG         (FRTYPE_DEBUG      -1)
#define FRABILITY_FONT          (FRTYPE_FONT       -1)
#define FRABILITY_DEBUG_FILE    (FRTYPE_DEBUG_FILE -1)
#define FRABILITY_DEBUG_LEVEL   (FRTYPE_DEBUG_LEVEL-1)
#define FRABILITY_LOADER        (FRTYPE_LOADER     -1)
// define flash size room for :
// currently is predefined
// 1. color table .
// 2. firmware code size
// 3. font code size
#define FRSIZE_CTAB        (128*1024)
#define FRSIZE_FIRMWARE    3*1024*1024      
#define FRSIZE_DEBUG       (MAX_SCSI_DBG_LINE*MAX_SCSI_DBG_CHARS)
#define FRSIZE_PHBK        (FAX_PHBK_L_SIZE + FAX_MENU_CFG_SIZE )
#define FRSIZE_FONT			(16*1024 )
#endif // _SCSI_PROT_H

// define flash ram infomation for :
// 1. color table infomation
// 2. firmware infomation
// 3. font information
// change back the define, and add ability define
typedef struct
{
	unsigned char bytes_per_unit ;
	unsigned char ability1 ;
	unsigned char size[8][4] ;
	unsigned char x_type;
	unsigned char reserved[5] ;
	
} stFlashInfoType;

typedef struct
{
	Uint32 tegNum;		//index for loader, F/W and etc.
	Uint32 dataSize;	//entire size include header and data
	Uint32 checkSum;	//the sum of data 
	Uint32 position;	//the position of flash
	Uint32 offset;		//position of data from start position
	Uint32 length;		//size of data
} stDownloadInfo;

typedef struct
{
	Uint32 identify;	//identify code //0x1B, 0x2A, 0x6D, 0x00
	Uint8  type;		//0xD0:burn file 0xD1: ram file
	Uint8  dummy01;
	Uint8  dummy02;
	Uint8  version;
	Uint32 checkSum;	//the sum of data 
	Uint32 length;		//size of data
	Uint32 ramAddr;		//ram address
	Uint32 romAddr;		//rom address
} stZoranFirmHdr;

typedef struct
{
	Uint8  index;
	Uint8  isOn;
	Uint16 addr;		
}stBreakPointCmd;

typedef struct
{
	Uint8  index;
	Uint8  isOn;
	Uint8  isRead;
	Uint8  isWrite;
	Uint16 addr;	
	Uint16 mask;
}stWatchPointCmd;

typedef struct
{	
	Uint16 addr;
	Uint16 dataLen;
	Uint8  data[2048];
}stWriteDataMemCmd;

#define set_quad(var,val)	var[0] = ((val) >> 24) & 0xff ; \
							var[1] = ((val) >> 16) & 0xff ; \
							var[2] = ((val) >> 8 ) & 0xff ; \
							var[3] = ((val) 	 ) & 0xff 
							
//void ScsiProtocolParser( stScsiDbgProtocol * );

typedef void (*GetSCSIDbgDataPtr)(Uint8* pData, Uint32 size);
AvApiRet ScsiDbgDataParserRegister(Uint16 dataType, GetSCSIDbgDataPtr pCallback);

#endif // _ScsiProtocolIF_H_
