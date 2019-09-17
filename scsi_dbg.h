#ifndef _SCSI_DBG_H
#define _SCSI_DBG_H

#include "ErrorDef.h"
/**
 * \file        scsi_dbg.h
 * \brief       Header file to output debug message and send to Host.
 * \author      Hairay
 * \version     0.1
 * \date        2006/02/20  
 */

#define HALT_MSG_LEVEL          0x00000001
#define ERROR_MSG_LEVEL         0x00000002
#define IMPORTANT_MSG_LEVEL     0x00000004
#define NOTE_MSG_LEVEL          0x00000008
#define SCANNER_MSG_LEVEL       0x00000010
#define PRINTDOC_MSG_LEVEL      0x00000020
#define PARSER_MSG_LEVEL        0x00000040
#define IO_MSG_LEVEL            0x00000080
#define IO_USB_MSG_LEVEL        0x00000100
#define PRINTENG_MSG_LEVEL      0x00000200
#define SCANDOC_MSG_LEVEL       0x00000400
#define FUNCTION_MSG_LEVEL      0x00000800
#define IP_MSG_LEVEL            0x00001000
#define COPY_MSG_LEVEL          0x00002000
#define UI_MSG_LEVEL            0x00004000
//#define DOGWOOD_MSG_LEVEL       0x00008000
#define MGR_MSG_LEVEL           0x00008000
#define DEBUG_TOOL_MSG_LEVEL    0x00010000
#define PRT_DVT_MSG_LEVEL       0x00020000
#define FLASH_MSG_LEVEL         0x00040000
#define BACKUP_MSG_LEVEL        0x00080000
#define FAX_MSG_LEVEL           0x00100000
#define NET_MSG_LEVEL           0x00200000
#define ADF_MSG_LEVEL           0x00400000
#define SHELL_MSG_LEVEL         0x00800000
#define JOB_MGR_LEVEL           0x01000000
#define SYS_MGR_LEVEL           0x02000000
#define JOBPRINT_MSG_LEVEL      0x04000000
#define IPS_MSG_LEVEL           0x08000000  //IPS

//#define PB_MSG1_LEVEL           0x10000000
//#define PB_MSG2_LEVEL           0x20000000
#define M3S_MSG_LEVEL           0x40000000  

#define NONSENSE_MSG_LEVEL      0x80000000
#define TURN_ON_ALL_MSG_LEVEL   0xFFFFFFFF

#define ACC_MSG_LEVEL           IP_MSG_LEVEL
#define USAGE_MSG_LEVEL         SHELL_MSG_LEVEL
#define M3P_MSG_LEVEL           NONSENSE_MSG_LEVEL

/* message level for debug build */
#ifndef DBGBUILD_MSG_LEVEL
#define DBGBUILD_MSG_LEVEL     (TURN_ON_ALL_MSG_LEVEL)
#endif

/* message level for release build */
#ifndef RELBUILD_MSG_LEVEL
#define RELBUILD_MSG_LEVEL     (HALT_MSG_LEVEL|ERROR_MSG_LEVEL|IMPORTANT_MSG_LEVEL|NOTE_MSG_LEVEL\
                                |PRINTDOC_MSG_LEVEL|PRINTENG_MSG_LEVEL|JOB_MGR_LEVEL|SYS_MGR_LEVEL)
#endif

#ifndef DEFAULT_DBG_MSG_LEVEL
  #ifdef _RELEASE
    #define DEFAULT_DBG_MSG_LEVEL  (RELBUILD_MSG_LEVEL)
  #else
    #define DEFAULT_DBG_MSG_LEVEL  (DBGBUILD_MSG_LEVEL)
  #endif
#endif

#define MAX_SCSI_DBG_LINE (8000)
#define MAX_SCSI_DBG_CHARS (160)
#define MAX_MEM_LIST 1024

typedef struct  {
    int  dbg_current_line;  
    int  dbg_read_line;
       Uint32 pattern;
} stScsiDebug, *stScsiDebugPtr;

typedef struct _scsi_debug_format {
    int   size;
    int   max_bytes_per_line;   
    int   read_line;
} stScsiDebugFormat , *stScsiDebugFormatPtr;

struct log_msg_st {
    long int log_level_type;
    char some_text[MAX_SCSI_DBG_CHARS];
};


#ifndef __MODULE__
#define __MODULE__ \
(strrchr(__FILE__,'/') \
? strrchr(__FILE__,'/')+1 \
: __FILE__ \
)
#endif

typedef struct _thread_info {   
    unsigned char   disableLog; 
    char            name[25];
    unsigned short  id; 
} stThreadInfo , *stThreadInfoPtr;

//#define DbgPrintf(a,...) TSlogLevel(a, ## __VA_ARGS__)
AvApiRet InitScsiDbg(void);
void BIOSassertFail(const char* exp, const char* file, int line);
void DbgPrintf(Uint32 msgLevel, const char *fmt, ...);
#define IpcPrintf DbgPrintf
AvApiRet ErrorPrintRet(const char *func, int line,AvApiRet errCode);
#define LogPrintf(level,fmt, ...) DbgPrintf(level, "%s:%04d : "fmt, __FUNCTION__, __LINE__, ## __VA_ARGS__)
#define LogPrintf2(level,fmt, ...) DbgPrintf(level, "%04d: "fmt, __LINE__, ## __VA_ARGS__)
#define ErrcodePrintf(fmt, ...) DbgPrintf(ERROR_MSG_LEVEL, "%s:%04d : "fmt, __FUNCTION__, __LINE__, ## __VA_ARGS__)
#define ErrPrintf(fmt, ...) DbgPrintf(ERROR_MSG_LEVEL, ""fmt, ## __VA_ARGS__)
#define ErrPrtRet(code) ErrorPrintRet(__FUNCTION__,__LINE__, code)
#define FunctionPrintf(fmt, ...) DbgPrintf(FUNCTION_MSG_LEVEL, "%s:%04d : "fmt, __FUNCTION__, __LINE__, ## __VA_ARGS__)
#define ScsiPrintf(fmt, ...) DbgPrintf(NOTE_MSG_LEVEL, "%s:%04d : "fmt, __FUNCTION__, __LINE__, ## __VA_ARGS__)
#define FlashPrintf(fmt, ...) DbgPrintf(FLASH_MSG_LEVEL, ""fmt, ## __VA_ARGS__)
#endif
