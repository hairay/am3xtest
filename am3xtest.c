#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
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

int read_dbg_data(usb_handle *usbHandle,char *buf,unsigned int size, unsigned char type, int isPrinter)
{
    unsigned char temp[16];
    int write_bytes,read_bytes =0;
    unsigned char read_cmd[] = { SCSI_CMD_READ,0x00,SCSI_RDTC_FLASH_DATA,type,0x00,type,0x00,0x00,0x00,0x00 } ;
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

int ReadDbg(usb_handle *usbHandle)
{
    int ret = 1, i, zeroCount;
    char *log, dummy[256];
    stScsiDebugFormat dbgInfo;
    unsigned long bufSize;
	
	close(usbHandle->desc);
	usbHandle->desc = -1;
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
            ret = read_dbg_data(usbHandle,(char *)&dbgInfo,sizeof(stScsiDebugFormat), FRTYPE_DEBUG, gIsPrinter[i]);		
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

                    ret = read_dbg_data(usbHandle,log,bufSize, FRTYPE_DEBUG, gIsPrinter[i]);
                    pBuf = log;
                    bufSize = ret;
                    while(bufSize)
                    {
                        write(gLogFileHandle[i], pBuf, strlen(pBuf));
                        pBuf += dbgInfo.max_bytes_per_line;
                        bufSize -= dbgInfo.max_bytes_per_line;
                    }
                    free(log);
                    dbgInfo.read_line =0;
                }
                else
                {
                    ret = read_dbg_data(usbHandle,dummy,dbgInfo.max_bytes_per_line, FRTYPE_DEBUG, gIsPrinter[i]);
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

int main(int argc, char** argv)
{
	stDateTimePara recordTime;
    usb_handle *usbHandle = NULL;
	char old_dev_path[64] = {0};
    char path[256];
    char id[64];
    char *current_dir;
    int i;

    gSem = sem_open ("AM3XTEST", O_CREAT, 0644, 1);
		
    sprintf(id, "/proc/%d/exe", getpid());
    readlink(id, path, 255);
    current_dir = dirname(path);

    signal(SIGTERM, ExitAm3xtestEvent);
    signal(SIGINT, ExitAm3xtestEvent);
    
    umask(0000);    
    while(gExitAm3xtest == 0)
    {
        usbHandle = usb_open(usb_match_func);
        if(usbHandle)
        {
            char logName[256], data[16];            
            int isPrinter;            

            sprintf(logName, "%s/am3xtest.ini", current_dir);
            get_private_profile_string(NULL, "VID", "0000", (char *)data, 16, logName);
            gVid = strtoul((char *)data, NULL, 16);
            get_private_profile_string(NULL, "PID", "0000", (char *)data, 16, logName);
            gPid = strtoul((char *)data, NULL, 16);
            isPrinter = get_private_profile_int(NULL, "read or write printer", 0, logName);
            printf("am3xtest.ini: gVid=0x%x gPid=0x%x Printer=%d\n", gVid, gPid, isPrinter);
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
