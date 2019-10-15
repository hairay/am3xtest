#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <semaphore.h>
#include <libgen.h>
#include <time.h>
#include "SysDef.h"
#include "usbapi.h"
#include "scsi_dbg.h"
#include "ScsiProtocolParser.h"
#include "scsi_parser_dbg_ep.h"
#include "read_write_ini.h"

typedef struct
{
    unsigned char sec;
    unsigned char min;
    unsigned char hour;
    unsigned char date;
    unsigned char mon;
    unsigned char dweek;
    unsigned short year;
} stDateTimePara;

static int gLogFileHandle[2];
static Uint16 gVid, gPid;
static int gScanDevTimes = 0;
static int gIsPrinter[2] = {0, 1};
static sem_t *gSem = NULL;
static int gSemCount;
static int gExitAm3xtest = 0;
static int gSaveLogLevel = 0;

static int SeizeSYSControl()
{
    struct timespec tm;

    clock_gettime(CLOCK_REALTIME, &tm);
    tm.tv_sec += 30;

    if (sem_timedwait(gSem, &tm) == 0)
    {
        //DbgMsg( "QueryPrinterStatus::  sem_wait success.");
        gSemCount++;
        return 0;
    }
    else
    {
        printf("sem_wait failed.(%d)", errno);
        return  errno;
    }
}

int ReleaseSYSControl()
{
    if(gSemCount>0 && sem_post(gSem) == 0)
    {
        gSemCount--;
        return 0;
    }
    else
    {
        printf("sem_post failed.(%d) gSemCount=%d", errno, gSemCount);
        return  errno;
    }
}

int read_dbg_data(usb_handle *usbHandle,char *buf,unsigned int size, unsigned char dtc, unsigned char dtq, int isPrinter)
{
    unsigned char temp[16];
    int write_bytes,read_bytes =0;
    unsigned char read_cmd[] = { SCSI_CMD_READ,0x00,dtc,dtq,0x00,dtq,0x00,0x00,0x00,0x00 } ;
    int sts, retSize;
    unsigned char *ptr = read_cmd + 6 ;
    ScsiSetTriple(ptr,size);

    if(isPrinter)
        read_cmd[0]	 = SCSI_CMD_PRINTER_READ;

    SeizeSYSControl();
    usbHandle->desc = open(usbHandle->fname, O_RDWR);

    if(usbHandle->desc == -1)
    {
        printf("open %s: %s\n", usbHandle->fname, strerror(errno));
    }

    write_bytes = usb_write(usbHandle, (const void *)read_cmd, sizeof(read_cmd));
    if( write_bytes <= 0 || write_bytes != sizeof(read_cmd))
    {
        close(usbHandle->desc);
        usbHandle->desc = -1;
        ReleaseSYSControl();
        printf("write_bytes %d: %s\n", write_bytes, strerror(errno));
        return -1;
    }

    retSize = 0;
    while(size >0)
    {
        read_bytes = usb_read(usbHandle, (void *)(buf+retSize), size);
        if(read_bytes <=0)
        {
            printf("read_bytes %d: %s\n", read_bytes, strerror(errno));
            break;
        }
        if(read_bytes)
        {
            retSize += read_bytes;
            size -= read_bytes;
        }
    }

    sts = SCSI_RET_INVAL;
    if(retSize)
        read_bytes = usb_read(usbHandle, (void *)temp, sizeof(temp));
    if(read_bytes)
        sts = temp[read_bytes-1];
    if(sts != SCSI_RET_OK)
    {
        printf("sts=%d: read_bytes=%d\n",sts,read_bytes);
    }
    close(usbHandle->desc);
    usbHandle->desc = -1;
    ReleaseSYSControl();
    return (sts==SCSI_RET_OK)? retSize :-1;

}

int send_dbg_data(usb_handle *usbHandle,char *buf,unsigned int size, unsigned char dtc, unsigned char dtq, int isPrinter)
{
    unsigned char temp[16];
    int write_bytes,read_bytes =0;
    unsigned char  send_cmd[10] = {SCSI_CMD_SEND,0x00,dtc,0x00,0x00,dtq,0x00,0x00,0x00,0x00};
    int sts, retSize;
    unsigned char *ptr = send_cmd + 6 ;
    ScsiSetTriple(ptr,size);

    if(isPrinter)
        send_cmd[0]	 = SCSI_CMD_PRINTER_SEND;

    SeizeSYSControl();
    usbHandle->desc = open(usbHandle->fname, O_RDWR);

    if(usbHandle->desc == -1)
    {
        printf("open %s: %s\n", usbHandle->fname, strerror(errno));
    }

    write_bytes = usb_write(usbHandle, (const void *)send_cmd, sizeof(send_cmd));
    if( write_bytes <= 0 || write_bytes != sizeof(send_cmd))
    {
        close(usbHandle->desc);
        usbHandle->desc = -1;
        ReleaseSYSControl();
        printf("write_bytes %d: %s\n", write_bytes, strerror(errno));
        return -1;
    }

    retSize = 0;
    while(size >0)
    {
        read_bytes = usb_write(usbHandle, (void *)(buf+retSize), size);
        if(read_bytes <=0)
        {
            printf("usb_write %d: %s\n", read_bytes, strerror(errno));
            break;
        }
        if(read_bytes)
        {
            retSize += read_bytes;
            size -= read_bytes;
        }
    }

    sts = SCSI_RET_INVAL;
    if(retSize)
        read_bytes = usb_read(usbHandle, (void *)temp, sizeof(temp));
    if(read_bytes)
        sts = temp[read_bytes-1];
    if(sts != SCSI_RET_OK)
    {
        printf("sts=%d: read_bytes=%d\n",sts,read_bytes);
    }
    close(usbHandle->desc);
    usbHandle->desc = -1;
    ReleaseSYSControl();
    return (sts==SCSI_RET_OK)? retSize :-1;

}

int ReadDbg(usb_handle *usbHandle)
{
    int ret = 1, i, zeroCount;
    char *log, dummy[256];
    stScsiDebugFormat dbgInfo;
    unsigned long bufSize;

    while(ret > 0 && gExitAm3xtest == 0)
    {
        zeroCount = 0;
        for(i=0 ; i < gScanDevTimes; i++)
        {
            if(gExitAm3xtest)
            {
                zeroCount = 0;
                i= gScanDevTimes;
                break;
            }
            dbgInfo.read_line = 0;
            ret = read_dbg_data(usbHandle,(char *)&dbgInfo,sizeof(stScsiDebugFormat), SCSI_RDTC_FLASH_DATA, FRTYPE_DEBUG, gIsPrinter[i]);
            if(ret < 0)
                printf("ret :%d read_line=%d \n", ret, dbgInfo.read_line);
            if(dbgInfo.read_line == 0)
                zeroCount ++;
            while(ret > 0 && dbgInfo.read_line > 0)
            {
                bufSize = dbgInfo.max_bytes_per_line * dbgInfo.read_line;
                log = (char *)malloc(bufSize);
                if(log)
                {
                    char *pBuf;

                    ret = read_dbg_data(usbHandle,log,bufSize, SCSI_RDTC_FLASH_DATA, FRTYPE_DEBUG, gIsPrinter[i]);
                    pBuf = log;
                    bufSize = ret;
                    while(bufSize)
                    {
                        Uint8 check;
                        Uint32 level = 0;
                        char strTmp[64];

                        check = (pBuf[dbgInfo.max_bytes_per_line-6]);			
                        if(check == 0xC1)                                                            		
                            level = *((Uint32 *)&pBuf[dbgInfo.max_bytes_per_line - 4]);     

                        if(gSaveLogLevel)
                        {                                                                               
                            sprintf(strTmp, "[%08X]", level);
				            write(gLogFileHandle[i], strTmp, strlen(strTmp));
                        }
                        else
                        {
                            if(level>0 && level <= 0x3)
                            {
                                sprintf(strTmp, "Next Line Is Error Level\n");
                                write(gLogFileHandle[i], strTmp, strlen(strTmp));
                            }
                        }
                        write(gLogFileHandle[i], pBuf, strlen(pBuf));
                        pBuf += dbgInfo.max_bytes_per_line;
                        bufSize -= dbgInfo.max_bytes_per_line;
                    }
                    free(log);
                    dbgInfo.read_line =0;
                }
                else
                {
                    ret = read_dbg_data(usbHandle,dummy,dbgInfo.max_bytes_per_line, SCSI_RDTC_FLASH_DATA, FRTYPE_DEBUG, gIsPrinter[i]);
                    printf("%s",dummy);
                    dbgInfo.read_line --;
                }
            }
        }
        if(zeroCount == gScanDevTimes)
            MsSleep(100);
    }

    return ret;
}

int usb_match_func(usb_ifc_info *ifc)
{
#ifdef _DEBUG_LOG_
    printf("VID :%x PID=%x\n", ifc->dev_vendor, ifc->dev_product);
    printf("device class :%x sub class=%x protocol=%x\n", ifc->dev_class, ifc->dev_subclass, ifc->dev_protocol);
    printf("interface class :%x sub class=%x protocol=%x\n", ifc->ifc_class, ifc->ifc_subclass, ifc->ifc_protocol);

    printf("has_bulk_in :%x has_bulk_out=%x\n", ifc->has_bulk_in, ifc->has_bulk_out);
    printf("sn :%s path=%s\n", ifc->serial_number, ifc->device_path);
#endif

    if(gVid != 0 && gVid != ifc->dev_vendor)
        return -1;

    if(gPid != 0 && gPid != ifc->dev_product)
        return -1;

    if(ifc->ifc_class == 6 || ifc->ifc_class == 0xFF)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

static void ExitAm3xtestEvent(int signum)
{
    if(gLogFileHandle[0] >=  0)
    {
        char temp[512];

        sprintf(temp, "AM3XTEST exit:%d signum=%d\n",gExitAm3xtest, signum);
        write(gLogFileHandle[0], temp, strlen(temp));
    }
    gExitAm3xtest ++;
}

int ClockUCO_GetCurrentTime(stDateTimePara * date)
{
    struct tm *nPtr;
    struct tm loc_time_info;
    time_t t1 = time(NULL);


    localtime_r(&t1, &loc_time_info);
    nPtr = &loc_time_info;

    date->year = nPtr->tm_year + 1900;;
    date->mon = nPtr->tm_mon + 1;
    date->date = nPtr->tm_mday;
    date->dweek = nPtr->tm_wday;
    date->hour = nPtr->tm_hour;
    date->min = nPtr->tm_min;
    date->sec = nPtr->tm_sec;

    return 0;
}

typedef struct  {
    int  cmd;
    int  action;
    int  address;
    int  length;
    int  loop;
    int  text;
    int  hex;
    int  positive;
    int  lsb;
    int  printer;
    int  dataCnt;
    Uint32 data[64];
    char txtData[512];
} stCmdInfo, *stCmdInfoPtr;

static char *itemCmdDef[] = {"nvram", "loglevel", NULL};
static char *itemActionDef[] = {"read", "write", NULL};
static char *itemTypeDef[] = {"address", "length", "loop", "text", "hex", "unsigned", "lsb", "printer", NULL};

static stCmdInfo cmdInfo;

int MapItemNum(char *itemDef[], char *checkItem)
{
    int i = 0;

    while(itemDef[i] != NULL)
    {
        if(strcasecmp(itemDef[i], checkItem) == 0)
            return i;
        i++;
    }
    return -1;
}

int CheckFixedItem(char *pToken)
{
    int mapVal;

    mapVal = MapItemNum(itemCmdDef, pToken);
    if(mapVal >=0)
    {
        cmdInfo.cmd = mapVal;
        return 0x10000 | mapVal;
    }

    mapVal = MapItemNum(itemActionDef, pToken);
    if(mapVal >=0)
    {
        cmdInfo.action = mapVal;
        return 0x20000 | mapVal;
    }

    mapVal = MapItemNum(itemTypeDef, pToken);
    if(mapVal >=0)
    {
        int *addr = &cmdInfo.address;
        addr[mapVal] = 1;
        return 0x30000 | mapVal;
    }
    return -1;
}

int GetDataFromStr(char *inStrPtr)
{
    int num =0, mapVal, lastMapVal = -1;
    char *pToken, *saveptr, *endptr;
    int maxVal = 64, status =0;

    memset(&cmdInfo.cmd, 0, sizeof(int) * 12);
    cmdInfo.loop = 1;
    cmdInfo.cmd = -1;
    cmdInfo.positive = 1;

    pToken = strtok_r(inStrPtr, " ,;\n", &saveptr);
    while(pToken != NULL)
    {
        mapVal = CheckFixedItem(pToken);
        //printf("pToken=%s CheckFixedItem = 0x%x lastMapVal=0x%x\n", pToken, mapVal, lastMapVal);
        if((lastMapVal & 0xF0000) ==  0x30000 && (lastMapVal & 0xF) <= 2)
        {
            int *addr = &cmdInfo.address;
            addr[lastMapVal & 0xF] = strtol(pToken, &endptr, 0);
            if(endptr[0] != 0)
            {
                addr[lastMapVal & 0xF] = -1;
                status = -1;
            }                
        }
        else if(mapVal == -1)
        {
            if(cmdInfo.text)
            {
                strcpy(cmdInfo.txtData, pToken);
            }
            else
            {
                if(num ==0)
                    strcpy(cmdInfo.txtData, pToken);
                if(cmdInfo.hex)
                    cmdInfo.data[num] = strtol(pToken, &endptr, 16);
                else
                    cmdInfo.data[num] = strtol(pToken, &endptr, 0);
                
                if(endptr[0] ==0)
                {
                    num ++;
                    if(num >= maxVal)
                        break;
                }
            }
        }
        lastMapVal = mapVal;
        pToken = strtok_r(NULL, " ,;\n", &saveptr);
    }
    cmdInfo.dataCnt = num;
    if(cmdInfo.cmd == -1 && status == 0)
        status = -1;

    return status;
}

int DoNvramWrite(usb_handle *usbHandle, int isWrite)
{
    int status = 0, i;
    Uint32 value;
    Uint8    data[4096];
    char posData[2];
    int    wAddress, m_unit_length, m_unit_loop, m_enable_text, m_enable_lsb, m_enable_hex;

    m_unit_length = cmdInfo.length;
    m_unit_loop = cmdInfo.loop;
    m_enable_text = cmdInfo.text;
    m_enable_lsb = cmdInfo.lsb;
    m_enable_hex = cmdInfo.hex;
    wAddress = cmdInfo.address;
    if(m_unit_length > 4)
        m_enable_text = 1;
    printf("--> addr=0x%x length=%d loop=%d\n", wAddress, m_unit_length, m_unit_loop);
    printf("--> isText=%d isLsb=%d isHex=%d\n", m_enable_text, m_enable_lsb, m_enable_hex);
    if(wAddress >= 0xFFFF || m_unit_length <= 0 || m_unit_loop <= 0)
        status = -1;
    if(m_enable_text && m_unit_loop > 1)
        status = -1;
    posData[0] = (char)(cmdInfo.address  >> 8);
    posData[1] = (char)(cmdInfo.address);

    if(isWrite && status == 0)
    {        
        memset(data, 0, m_unit_length*m_unit_loop);
        for(i=0; i<m_unit_loop; i++)
        {
            if(m_enable_text)
            {            
                strcpy((char *)data, cmdInfo.txtData);
            }
            else if(m_unit_length == 2)
            {            
                value = cmdInfo.data[i];
                
                if(m_enable_lsb)
                {
                    data[i*m_unit_length+1] = (Uint8)((value & 0xFF00) >> 8);
                    data[i*m_unit_length] = (Uint8)((value & 0xFF));
                }
                else
                {
                    data[i*m_unit_length] = (Uint8)((value & 0xFF00) >> 8);
                    data[i*m_unit_length+1] = (Uint8)((value & 0xFF));
                }
            }
            else if(m_unit_length == 4)
            {
                value = cmdInfo.data[i];

                if(m_enable_lsb)
                {
                    data[i*m_unit_length+3] = (Uint8)((value & 0xFF000000) >> 24);
                    data[i*m_unit_length+2] = (Uint8)((value & 0xFF0000) >> 16);
                    data[i*m_unit_length+1] = (Uint8)((value & 0xFF00) >> 8);
                    data[i*m_unit_length] = (Uint8)((value & 0xFF));
                }
                else
                {
                    data[i*m_unit_length] = (Uint8)((value & 0xFF000000) >> 24);
                    data[i*m_unit_length+1] = (Uint8)((value & 0xFF0000) >> 16);
                    data[i*m_unit_length+2] = (Uint8)((value & 0xFF00) >> 8);
                    data[i*m_unit_length+3] = (Uint8)((value & 0xFF));
                }
            }
            else if(m_unit_length == 1)
            {
                value = cmdInfo.data[i];
                data[i*m_unit_length] = (Uint8)((value & 0xFF));
            }

        }        
        
        status = send_dbg_data(usbHandle,posData,2,SCSI_SDTC_NVRAM_POINTER, 0,cmdInfo.printer);
        if(status > 0)
            status = send_dbg_data(usbHandle,(char *)data,m_unit_length*m_unit_loop,SCSI_SDTC_NVRAM, 0,cmdInfo.printer); 
        printf("--> write nvram status=%d\n", status);           
    }
    else if(status == 0)
    {
        char szShow[16384], szTemp[512];

        szShow[0] = 0;    
        status = send_dbg_data(usbHandle,posData,2,SCSI_SDTC_NVRAM_POINTER, 0,cmdInfo.printer);
        if(status > 0)
            status = read_dbg_data(usbHandle,(char *)data,m_unit_length*m_unit_loop,SCSI_RDTC_NVRAM_DATA, 0,cmdInfo.printer);        
        
        if(status > 0)
        {            
            for(i=0; i<m_unit_loop; i++)
            {     
                if(m_enable_text)
                {
                    strcpy(szShow, (char *)data);                    
                }     
                else if(m_unit_length == 2)
                {
					if(m_enable_lsb)
						value = (data[i*m_unit_length+1] << 8) + (data[i*m_unit_length]);
					else
						value = (data[i*m_unit_length] << 8) + (data[i*m_unit_length+1]);
					if(m_enable_hex)
						sprintf(szTemp, "%X ", value); 
					else if(cmdInfo.positive)
						sprintf(szTemp, "%u ", value); 
					else
						sprintf(szTemp, "%d ", (short)value); 
                    strcat(szShow, szTemp);
                }
                else if(m_unit_length == 4)
                {
					if(m_enable_lsb)
						value = (data[i*m_unit_length+3] << 24) + (data[i*m_unit_length+2] << 16)
							+(data[i*m_unit_length+1] << 8)+(data[i*m_unit_length]);
					else	
						value = (data[i*m_unit_length] << 24) + (data[i*m_unit_length+1] << 16)
							+(data[i*m_unit_length+2] << 8)+(data[i*m_unit_length+3]);
                    if(m_enable_hex)
						sprintf(szTemp, "%X ", value); 
					else if(cmdInfo.positive)
						sprintf(szTemp, "%u ", value); 
					else
						sprintf(szTemp, "%d ", (int)value); 
                    strcat(szShow, szTemp);
                }
                else if(m_unit_length == 1)
                {
                    value = data[i*m_unit_length];
                    if(m_enable_hex)
						sprintf(szTemp, "%X ", value); 
					else if(cmdInfo.positive)
						sprintf(szTemp, "%u ", value); 
					else
						sprintf(szTemp, "%d ", (signed char)value); 
                    strcat(szShow, szTemp);
                }
                                            
            }            
        }
        printf("--> read nvram = %s\n", szShow);
    }
    return status;
}

void* CmdParser(void* data)
{
    usb_handle *usbHandle = (usb_handle *)data;
    char *linePtr = NULL;
    size_t len = 0;
    ssize_t nread;
    int status;

    while(gExitAm3xtest == 0)
    {
        int retApi ;

        nread  = getline(&linePtr, &len, stdin);
        if(nread)
        {
            status = GetDataFromStr(linePtr);
            
            if(status == 0 && cmdInfo.cmd == 1) //logLevel
            {
                Uint32 dbgLevel;

                retApi = -1;
                dbgLevel = cmdInfo.data[0];
                if(cmdInfo.action == 0)
                {
                    retApi = read_dbg_data(usbHandle,(char *)&dbgLevel,sizeof(dbgLevel),SCSI_RDTC_FLASH_DATA, FRTYPE_DEBUG_LEVEL,cmdInfo.printer);
                    printf("--> read loglevel = 0x%08X\n", dbgLevel);
                }
                else if(cmdInfo.action == 1 && cmdInfo.dataCnt == 1)
                {
                    retApi = send_dbg_data(usbHandle,(char *)&dbgLevel,sizeof(dbgLevel),SCSI_SDTC_FLASH, FRTYPE_DEBUG_LEVEL,cmdInfo.printer);
                    printf("--> write loglevel = 0x%08X \n", dbgLevel);
                }                
                if(retApi <= 0)                
                    status = -1;                                               
            }
            else if(status == 0 && cmdInfo.cmd == 0) //nvram
            {
                status = DoNvramWrite(usbHandle, cmdInfo.action);
            }
            if(status < 0)
                printf("--> command error or execute fail %d\n", status);

        }
    }
    if(linePtr)
        free(linePtr);
    pthread_exit(NULL);
}

int main(int argc, char** argv)
{
    stDateTimePara recordTime;
    usb_handle *usbHandle = NULL;
    char old_dev_path[64] = {0};
    char path[256];
    char id[64];
    char *current_dir;
    int i;
    pthread_t threadID;

    gSem = sem_open ("AM3XTEST", O_CREAT, 0644, 1);

    sprintf(id, "/proc/%d/exe", getpid());
    readlink(id, path, 255);
    current_dir = dirname(path);

    signal(SIGTERM, ExitAm3xtestEvent);
    signal(SIGINT, ExitAm3xtestEvent);

    umask(0000);
    while(gExitAm3xtest == 0)
    {
        SeizeSYSControl();
        usbHandle = usb_open(usb_match_func);
        if(usbHandle)
        {
            close(usbHandle->desc);
            usbHandle->desc = -1;
            pthread_create(&threadID, NULL, CmdParser, usbHandle); // 建立子執行緒
        }
        ReleaseSYSControl();

        if(usbHandle)
        {
            char logName[256], data[16];
            int isPrinter;

            sprintf(logName, "%s/am3xtest.ini", current_dir);
            get_private_profile_string(NULL, "VID", "0000", (char *)data, 16, logName);
            gVid = strtoul((char *)data, NULL, 16);
            get_private_profile_string(NULL, "PID", "0000", (char *)data, 16, logName);
            gPid = strtoul((char *)data, NULL, 16);
            gSaveLogLevel = get_private_profile_int(NULL, "save debug level", 0, logName);
            isPrinter = get_private_profile_int(NULL, "read or write printer", 0, logName);
            printf("am3xtest.ini: gVid=0x%x gPid=0x%x Printer=%d LogLevel=%d\n", gVid, gPid, isPrinter, gSaveLogLevel);
            printf("Press Ctrl + C to exit am3xtest\n");
            gScanDevTimes = 1;
            if(isPrinter)
                gScanDevTimes ++;
            if(strcmp(old_dev_path, usbHandle->fname))
            {
                ClockUCO_GetCurrentTime(&recordTime);
                strcpy(old_dev_path, usbHandle->fname);
            }
            sprintf(logName, "%s/test_%04d%02d%02d_%02d%02d%02d.txt", current_dir, recordTime.year, recordTime.mon, recordTime.date, recordTime.hour, recordTime.min, recordTime.sec);
            gLogFileHandle[0] = open(logName, O_RDWR| O_CREAT| O_APPEND, 00666);
            if(isPrinter)
            {
                sprintf(logName, "%s/print_%04d%02d%02d_%02d%02d%02d.txt", current_dir, recordTime.year, recordTime.mon, recordTime.date, recordTime.hour, recordTime.min, recordTime.sec);
                gLogFileHandle[1] = open(logName, O_RDWR| O_CREAT| O_APPEND, 00666);
            }

            ReadDbg(usbHandle);
            pthread_cancel(threadID);
            pthread_join(threadID, NULL);
            usb_close(usbHandle);
            free(usbHandle);
            usbHandle = NULL;
            for(i=0; i<gScanDevTimes; i++)
            {
                if(gLogFileHandle[i] >= 0)
                {
                    close(gLogFileHandle[i]);
                    gLogFileHandle[i] = -1;
                }
            }
            printf("Exit ReadDbg\n");
        }
        else
        {
            MsSleep(500);
            gScanDevTimes = 0;
        }
    }
    if(usbHandle != NULL)
    {
        pthread_cancel(threadID);
        pthread_join(threadID, NULL);
        usb_close(usbHandle);
        free(usbHandle);
    }
    for(i=0; i<gScanDevTimes; i++)
        if(gLogFileHandle[i] >=  0)
            close(gLogFileHandle[i]);

    sem_close(gSem);
    sem_destroy(gSem);
    //sem_unlink("AM3XTEST");

    return 0;
}
