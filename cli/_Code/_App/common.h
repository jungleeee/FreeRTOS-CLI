/**
  ********************************************************************************
  * @File Name    : common.h
  * @Author       : Jungle
  * @Mail         : Mail
  * @Created Time : 2018年3月22日 9:25:59
  * @Version      : V1.0
  * @Last Changed : 2018年3月26日 10:12:04
  * @Brief        : brief
  ********************************************************************************
  */

/* Define to prevent recursive inclusion ---------------------------------------*/
#ifndef __COMMON_H_
#define __COMMON_H_

/* Inlcude ---------------------------------------------------------------------*/

/* Exported typedef ------------------------------------------------------------*/
enum DEBUG_PRINT_INDEX {
    CLI_DEBUG_PRINT = 0,
    SYS_DEBUG_PRINT,
    COM_DEBUG_PRINT,
    POS_DEBUG_PRINT,
    DEBUG_PRINT_INDEX_NUM
};

typedef union {
    struct {//search bit-field
		unsigned int CLI	    :1;//LSB
		unsigned int SYS		:1;
		unsigned int COM        :1;
		unsigned int POS        :1;
		unsigned int OTHER		:28;
	}cliDebugStatusBit;
	unsigned int cliDebugStatus;
}CLI_DEBUG_PEINT_STATUS;

/* Exported constants define ---------------------------------------------------*/
#define     FALSE			            0
#define     TRUE			            1

/* CLI、SYS default OPEN & Alaways OPEN */
#define     cli_DEBUG_DEFAULT_STATUS    0x00000003

/* If you need to increase cli_MAX_TX_QUEUE_LEN,
   you must need to increase cli_MAX_TX_QUEUE_BUG_WAIT, and you can set cli_MAX_TX_QUEUE_BUG_WAIT = 0 & input 'help' to test the BUG.
   It is recommended that you print data more than once, instead of printing large amounts of data at a time.*/
#define     cli_MAX_TX_QUEUE_LEN        200
#define     cli_MAX_TX_QUEUE_BUG_WAIT   20

/* Exported macro define -------------------------------------------------------*/
#define     SYS_isDebugOn               xCliDebugStatus.cliDebugStatusBit.SYS
#define     debugPrintInfo( s, ... )    {if(s) {\
                                            if(snprintf(cCliOutputString, cli_MAX_TX_QUEUE_LEN, __VA_ARGS__) > cli_MAX_TX_QUEUE_LEN) { cli_print_info();\
                                                sprintf(cCliOutputString, pcWarningMessageString); cli_print_info();\
                                            }else cli_print_info();\
                                        }}

/* Exported variables ----------------------------------------------------------*/
static const char * const pcCmdLineString  = "ROOT";
static const char * const pcWarningMessageString = "\r\n\r\n **Warning Output string length > (macro)cli_MAX_TX_QUEUE_LEN *\r\n\r\n";

extern char cCliOutputString[cli_MAX_TX_QUEUE_LEN];
extern volatile CLI_DEBUG_PEINT_STATUS xCliDebugStatus;//CLI,SYS: default open;

/* Exported functions ----------------------------------------------------------*/
extern void cli_print_info(void);//cli_task.c

#endif /* __COMMON_H_ */

/************************** Coopyright (C) Jungleeee 2018 *******END OF FILE*******/

