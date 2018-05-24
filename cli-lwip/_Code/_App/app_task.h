/**
  ********************************************************************************
  * @File Name    : app_task.h
  * @Author       : Jungle
  * @Mail         : Mail
  * @Created Time : 2018/4/10 15:53:38
  * @Version      : V1.0
  * @Last Changed : 2018/4/10 15:53:38
  * @Brief        : brief
  ********************************************************************************
  */

/* Define to prevent recursive inclusion ---------------------------------------*/
#ifndef __APP_TASK_H_
#define __APP_TASK_H_

/* Inlcude ---------------------------------------------------------------------*/

/* Exported typedef ------------------------------------------------------------*/
/* Exported constants define ---------------------------------------------------*/
#define     CLI_TASK_SEND_STACK_DEPTH               128
#define     CLI_TASK_RECEIVE_STACK_DEPTH            256

#define     LED_TASK_STACK_DEPTH                    128

/* Exported macro define -------------------------------------------------------*/
#define     CLI_TASK_SEND_PRIORITY                  1
#define     CLI_TASK_RECEIVE_PRIORITY               2

#define     LED_TASK_PRIORITY                       2

/* Exported variables ----------------------------------------------------------*/
/* Exported functions ----------------------------------------------------------*/

#endif /* __APP_TASK_H_ */

/************************** Coopyright (C) Jungleeee 2018 *******END OF FILE*******/

