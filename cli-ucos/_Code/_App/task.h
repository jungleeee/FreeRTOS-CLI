/**
  ********************************************************************************
  * @File Name    : task.h
  * @Author       : Jungle
  * @Mail         : Mail
  * @Created Time : 2018/3/30 15:43:34
  * @Version      : V1.0
  * @Last Changed : 2018/3/30 15:43:34
  * @Brief        : brief
  ********************************************************************************
  */

/* Define to prevent recursive inclusion ---------------------------------------*/
#ifndef __TASK_H_
#define __TASK_H_

/* Inlcude ---------------------------------------------------------------------*/
#include "os.h"
#include "cpu.h"

#include "stdint.h"

/* Exported typedef ------------------------------------------------------------*/
/* Exported constants define ---------------------------------------------------*/
/* Exported macro define -------------------------------------------------------*/
#define  TASK_CFG_START_TASK_STK_SIZE                      64u
#define  TASK_CFG_CLI_SEND_TASK_STK_SIZE                   128u
#define  TASK_CFG_CLI_RECEIVE_TASK_STK_SIZE                256u

#define  TASK_CFG_START_TASK_PRIO                           5u
#define  TASK_CFG_CLI_SEND_TASK_PRIO                        4u
#define  TASK_CFG_CLI_RECEIVE_TASK_PRIO                     3u

/* Exported variables ----------------------------------------------------------*/
static  OS_TCB   startTaskTCB;
static  OS_TCB   cliSendTaskTCB;
static  OS_TCB   cliReceiveTaskTCB;

static  CPU_STK  startTaskStk[TASK_CFG_START_TASK_STK_SIZE];
static  CPU_STK  cliSendTaskStk[TASK_CFG_CLI_SEND_TASK_STK_SIZE];
static  CPU_STK  cliReceiveTaskStk[TASK_CFG_CLI_RECEIVE_TASK_STK_SIZE];

/* Exported functions ----------------------------------------------------------*/

#endif /* __TASK_H_ */

/************************** Coopyright (C) Jungleeee 2018 *******END OF FILE*******/

