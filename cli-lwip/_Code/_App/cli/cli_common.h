/**
  ********************************************************************************
  * @File Name    : cli_common.h
  * @Author       : Jungle
  * @Mail         : Mail
  * @Created Time : 2018/4/9 15:54:38
  * @Version      : V1.0
  * @Last Changed : 2018/4/9 15:54:38
  * @Brief        : brief
  ********************************************************************************
  */

/* Define to prevent recursive inclusion ---------------------------------------*/
#ifndef __CLI_COMMON_H_
#define __CLI_COMMON_H_

/* Inlcude ---------------------------------------------------------------------*/

/* Exported typedef ------------------------------------------------------------*/
enum DEBUG_PRINT_INDEX {
    CLI_DEBUG_PRINT = 0,
    SYS_DEBUG_PRINT,
    COM_DEBUG_PRINT,
    POS_DEBUG_PRINT,
    NET_DEBUG_PRINT,
    DEBUG_PRINT_INDEX_NUM
};

typedef union {
    struct {//search bit-field
		unsigned int CLI	    :1;//LSB
		unsigned int SYS		:1;
		unsigned int COM        :1;
		unsigned int POS        :1;
		unsigned int NET        :1;
		unsigned int OTHER		:27;//!!! warning: total 32 bits !!!
	}Bits;
	unsigned int cliDebugStatus;
}CLI_DEBUG_PEINT_STATUS;

/* Exported constants define ---------------------------------------------------*/
#define     CLI_DEBUG_DEFAULT_STATUS    0x00000003  /* CLI¡¢SYS default OPEN & Alaways OPEN */

#define     CLI_MAX_TX_QUEUE_LEN        200         /* Define send queue parameters. */

/* Exported macro define -------------------------------------------------------*/
#define     SYS_IS_ON_DEBUG             xCliDebugStatus.Bits.SYS
#define     DEBUG_PRINT_INFO( s, ... )  do {\
                                            if(s) {\
                                                if(snprintf(cCliOutputString, CLI_MAX_TX_QUEUE_LEN, __VA_ARGS__) > CLI_MAX_TX_QUEUE_LEN) { cli_print_info();\
                                                    sprintf(cCliOutputString, pcWarningMessageString); cli_print_info();\
                                                }else cli_print_info();\
                                            }\
                                        }while(0)

/* Exported config define -------------------------------------------------------*/

/* Exported variables ----------------------------------------------------------*/
static const char * const pcCmdLineString  = "ROOT";
static const char * const pcWarningMessageString = "\r\n\r\n **Warning Output string length > (macro)CLI_MAX_TX_QUEUE_LEN *\r\n\r\n";

extern char cCliOutputString[CLI_MAX_TX_QUEUE_LEN];
extern volatile CLI_DEBUG_PEINT_STATUS xCliDebugStatus;

/* Exported functions ----------------------------------------------------------*/
extern void cli_print_info(void);//cli_task.c

#endif /* __CLI_COMMON_H_ */

/************************** Coopyright (C) Jungleeee 2018 *******END OF FILE*******/

