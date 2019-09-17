#ifndef SYS_DEF_H
#define SYS_DEF_H

#ifndef ARCH_4XXX_H
typedef int				BOOL;
typedef unsigned char	Uint8;
typedef signed char	    Sint8;
typedef unsigned short	Uint16;
typedef unsigned int 	Uint32;
typedef int AvApiRet ;
#define MsSleep(a) usleep(a*1000)
# define ASSERT(_XX_)                   \
    do { if (!(_XX_)) { BIOSassertFail(#_XX_, __FILE__, __LINE__); }} while (0)
#endif

#ifndef TRUE
   #define TRUE       1
#endif

#ifndef FALSE
   #define FALSE      0
#endif

enum enumWorker
{
	APP_INVALID = 0,
	/* 1 */
	SYS_MGR		= 1,
	APP_SCAN,
	APP_COPY,
	APP_COPY_SCAN,
	APP_COPY_PRINT,

	APP_FAX,
	APP_FAX_SCAN,
	APP_FAX_PRINT,
	APP_REPORT,
	APP_SCANTOPC,

	APP_JOBPRINT,
	APP_STORAGECOPY,
	APP_NETSCAN,
	APP_COLORCOPY,
	APP_STORAGE_FLASH,

	ENG_SCAN,
	ENG_PRINT,
	UI,
    UI_NTF_COPY,
    UI_NTF_NET,
    UI_NTF_PRINT,
    ERR_SHOW,
	APP_PDL,
	ENG_PRINT2,

	APP_NET,
	APP_BACKUP,
	APP_FLASH,
	APP_CARDREADER,

	PC_DOWNLOAD,
	MFP_DEVICE,
	IO_USB,
	APP_HWTEST,
	APP_EXTSYS,  /* external host system */

	JOB_STATUS_MGR,
	ENG_FILE_SYSTEM,
	APP_IPS,
	ACCOUNT_MGR,
	PRR_SEVICEMODE,

	APP_COPY_TEST,
	APP_CUSTOM_WORKFLOW,
	ENG_EXTERNAL_SCAN,
	SYS_EVENT,
	SYS_INFO,
	INFO_MGR,

	SYS_ERR,
	ERR_MGR,
	APP_PC_COPY,
	APP_PWG,
	MEM_MGR,
    PCU,
    APLUS_MGR,

	/*For Network Module. --- added by kevin*/
	APP_NET_WEBPAGE,
	APP_NET_SNMP,
	APP_NET_WECHATPAY,

	APP_SYS_WEBPAGE,	//Add APP_SYS_WEBPAGE for manipulate system setting through webpage 20181022

	APP_PDL_SYS,//For APP_PDL to receive system msg
	APP_PDL_IO,//For APP_PDL to get device job num

	APP_SYS_SNMP,		//Add APP_SYS_SNMP for manipulate SNMP function 20181220
	
	APP_CANCEL,
	
	MBTest,	

	MAX_REGISTER_NUM
};


enum enumJobType
{
	eJOB_INVALID,
	/* 1 */
	eJOB_SCAN,
	eJOB_PRINT,
	eJOB_COPY,
	eJOB_FAX_TX,
	eJOB_FAX_RX,
	/* 6 */
	eJOB_FAX_SCAN,
	eJOB_FAX_PRINT,
	eJOB_REPORT_PRINT,
	eJOB_IMPORTANT_REPORT_PRINT,
	eJOB_STORAGE_COPY,
	/* 11 */
	eJOB_STORAGE_PRINT,
	eJOB_STORAGE_NET_SCAN,
	eJOB_PRINT_STORAGE_JOB,
	eJOB_PRINT_FROM_USB,
	eJOB_COLOR_COPY,
	/* 16 */
	eJOB_SCAN_TO_FLASH,
	eJOB_SYS_MAINTAIN,
	eJOB_NET_EMAIL_SCAN,
	eJOB_NET_EMAIL,
	eJOB_NET_FTP_SCAN,
	/* 21 */
	eJOB_NET_FTP,
	eJOB_CUSTOMIZED_WORKFLOW,
	eJOB_FAX_FROM_PC,
	eJOB_EXTERNAL_SCANNER_COPY,
	eJOB_ID_CARD_COPY,
	/* 26 */
	eJOB_NET_CIFS_SCAN,
	eJOB_NET_CIFS,
	eJOB_NET_APP_SCAN,

	eJOB_COPY_SEPARATE,
	
	eJOB_PRINT_MOBILE,
	eJOB_PRINT_CLOUD,
	eJOB_PRINT_CLOUD_BY_NUMBER,
	
	eJOB_SCAN_TO_EMAIL,
	eJOB_SCAN_TO_MOBILE,
	
	eJOB_PRINT_CLOUD_PAID,
	
	eJOB_NET_CLOUD_SCAN,
	
	eJOB_PRINT_CLOUD_ID_CARD,
	eJOB_PRINT_CLOUD_BY_NUMBER_ID_CARD,
	eJOB_PRINT_CLOUD_PAID_ID_CARD,

	eJOB_TYPE_NUM,
	eJOB_TYPE_NULL = 0xFF
};


enum enumDevice
{
    DEV_NULL,
    DEV_PANEL,
    DEV_SCANNER,
    DEV_PRINTER,
    DEV_STORAGE,
    DEV_FAX,
    DEV_PRINTER2,  /* external printer engine */
    DEV_MAX
};

#define DEV2RESFLAG(dev) ((0x00000001 << (dev)) >> 0x01)

enum enumResourceFlag
{
    RES_NONE      = DEV2RESFLAG(DEV_NULL),
    RES_PANEL     = DEV2RESFLAG(DEV_PANEL),
    RES_SCANNER   = DEV2RESFLAG(DEV_SCANNER),
    RES_PRINTER   = DEV2RESFLAG(DEV_PRINTER),
    RES_STORAGE   = DEV2RESFLAG(DEV_STORAGE),
    RES_FAX       = DEV2RESFLAG(DEV_FAX),
    RES_PRINTER2  = DEV2RESFLAG(DEV_PRINTER2),
    RES_ESSENTIAL = (RES_SCANNER | RES_PRINTER),
    RES_ALL       = (RES_PANEL | RES_SCANNER | RES_PRINTER | RES_STORAGE | RES_FAX | RES_PRINTER2)
};

#if 0
/**
 * \brief pre-defined task type value
 *
 * task type is designed in resource flag
 */
enum enumTaskType
{
	TASK_NONE             = (RES_NONE),
	TASK_PC_SCAN          = (RES_SCANNER),
	/* net scan */
	TASK_NET_SCAN_SCAN    = (RES_PANEL | RES_SCANNER),
	TASK_NET_SCAN_RETR    = (RES_NONE),
	/* copy */
	TASK_COPY_SCAN        = (RES_PANEL | RES_SCANNER),
	TASK_COPY_PRINT       = (RES_PANEL | RES_PRINTER),
	/* storage copy */
	TASK_STORAGE_COPY     = (RES_PANEL | RES_SCANNER),
	/* pc print */
	TASK_PC_PRINT         = (RES_PRINTER),
	TASK_STORAGE_PRINT    = (RES_NONE),
	TASK_JOB_PRINT        = (RES_PANEL | RES_PRINTER),
	/* fax */
	TASK_FAX_SCAN         = (RES_PANEL | RES_SCANNER | RES_FAX),
	TASK_FAX_SCAN_STORE   = (RES_PANEL | RES_SCANNER),
	TASK_FAX_SCAN_TX      = (RES_FAX),
	TASK_FAX_PRINT_RECV   = (RES_FAX),
	TASK_FAX_PRINT_STORE  = (RES_NONE),
	TASK_FAX_PRINT_PRINT  = (RES_PRINTER),
	TASK_FAX_DIAL         = (RES_PANEL | RES_FAX),
	TASK_PC_FAX_STORE     = (RES_NONE),
	TASK_PC_FAX_TX        = (RES_FAX),
	/* color copy */
	TASK_COLOR_COPY       = (RES_PANEL | RES_SCANNER | RES_PRINTER2),
	TASK_INVALID          = (RES_ALL),    /**< for blocking all other type of jobs */
	TASK_MAX              = (RES_ALL)
};
#endif

/**
 * \note smaller number for higher priority
 */
enum enumPriority
{
	eJOB_URGENT_PRIORITY = 50,
	eJOB_HIGH_PRIORITY   = 100,
	eJOB_NORMAL_PRIORITY = 200,
	eJOB_DEFAULT_PRIORITY= 255
};

typedef enum enumWorker      enumEventSender;
typedef enum enumJobType     enumJobType;
typedef enum enumAppType     enumAppType;
typedef enum enumPriority    enumJobPriority;
typedef enum enumAppState    enumAppState;

typedef Uint8 jobId_t;

typedef struct _stJobSig
{
	Uint8   jobType;   /**< enumJobType, input */
	Uint8   appType;   /**< enumAppType, input */
	Uint16  taskType;  /**< input */
	jobId_t jobID;     /**< input,output */
	Uint8   priority;  /**< enumJobPriority, output */
}
stJobSig;

#ifndef JOB_NAME_LEN
#define JOB_NAME_LEN	64
#endif

#ifndef OWNER_NAME_LEN
#define OWNER_NAME_LEN		31
#endif

#ifndef OWNER_PASS_LEN
#define OWNER_PASS_LEN		31
#endif

#ifndef JOB_STATUS_NAME_LEN
#define JOB_STATUS_NAME_LEN		256
#endif

typedef enum
{
	eOwner_UnknowUser,
	eOwner_SystemUser,
	eOwner_LocalUser,
	eOwner_RemoteUser,
	eOwner_Num
}enumOwnerType;

typedef struct _stJobOwner
{
	int				type;
	char			name[OWNER_NAME_LEN+1];
	char			pass[OWNER_PASS_LEN+1];
}stJobOwner;


#define REQUEST_JOB     0
#define JOBID_MIN       1
#define JOBID_MAX       0xFE
#define INVALID_JOBID   ((JOBID_MAX) + 1)

#ifndef NO_SLEEP
#define NO_SLEEP 0
#endif

#ifndef NO_POWER_OFF
#define NO_POWER_OFF 0
#endif

#define __func__		__FUNCTION__

typedef enum
{
	eSysMod_Mfp = 1,
	eSysMod_Printer,
	eSysMod_Scanner,
	eSysMod_Net,
	eSysMod_Fax,
	eSysMod_App,
	eSysMod_Num
}enumSysModule;

typedef enum
{
	eMode_NA = 0,
	eMode_BW,
	eMode_Gray,
	eMode_Color,
}enumImageMode;

typedef enum
{
	eFormat_NA = 0,
	eFormat_TIFF,
	eFormat_JPEG,
	eFormat_PDF,
	eFormat_MPDF,
}enumImageFileFormat;

typedef enum
{
	eRes_NA 	= 0,
	eRes_150	= 150,
	eRes_200	= 200,
	eRes_300	= 300,
	eRes_600	= 600,
	eRes_1200	= 1200
}enumResolution;

#endif /* SYS_DEF_H */
