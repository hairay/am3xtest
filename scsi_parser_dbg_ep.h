#ifndef _SCSI_PARSER_DBG_EP_H_
#define _SCSI_PARSER_DBG_EP_H_

//#include "ScsiProtocolParser.h"

typedef struct
{
	SCSIRetCode (*Read)( void * );
	SCSIRetCode (*Send)( void * );
} stScsiDbgOp;

// Be careful for items if sould be 4 bytes aligment
// all variable should put __align(4) before argu, for address 4 bytes aligment
typedef struct
{
	unsigned char     scsiCmd[MAX_SCSI_CMD_SIZE+2];	      // 10 bytes
	unsigned char     scsiRet[SCSI_RET_BUF_SIZE];       // return addr by USB, it should be 4 bytes aligment,
	stScsiDbgOp       *opPtr;
	stSenseDataType   *senseDataPtr;
	Uint16            dtc;              // Data Type Code, (READ Cmd, SEND Cmd) dispatch Cmd Code
	Uint16            dtq;              // Data Type Qualifier (READ Cmd, SEND Cmd) depend on different dtc

} stScsiDbgProtocol;

typedef struct _stSyncInfo
{
    Uint32 totalLen;     // totalLen = sizeof(stSyncInfo) + PSNLen + dataLen
    Uint32 checkSum;     // checksum of data, without head and PSN
    Uint32 productSNLen;
    Uint32 dataLen;
}stSyncInfo;

AvApiRet scsi_dump_dbg_file_ep(int readSize);
//SCSICmdCode ScsiGetDbgCmd( stScsiDbgProtocol *protocolPtr );
SCSIRetCode ScsiRetDbgCmd( stScsiDbgProtocol *protocolPtr, SCSIRetCode retCode );

#endif
