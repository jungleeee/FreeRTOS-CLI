/**
  ********************************************************************************
  * @File Name    : serial.h
  * @Author       : Jungle
  * @Mail         : Mail
  * @Created Time : 2018Äê3ÔÂ19ÈÕ 17:21:38
  * @Version      : V1.0
  * @Last Changed : 2018/4/11 10:51:26
  * @Brief        : brief
  ********************************************************************************
  */

/* Define to prevent recursive inclusion ---------------------------------------*/
#ifndef __SERIAL_H_
#define __SERIAL_H_

/* Inlcude ---------------------------------------------------------------------*/
#include "stdint.h"

/* Exported typedef ------------------------------------------------------------*/
/* Exported constants define ---------------------------------------------------*/
/* Exported macro define -------------------------------------------------------*/
/* Exported variables ----------------------------------------------------------*/
/* Exported functions ----------------------------------------------------------*/
void serial_cli_init(int baudRate, void (* const cliReceiveOneCharCallBack)(uint16_t));
void serial_cli_print_init(void (* const cliPrintOverCallBack)(void));

void serial_cli_receive_enable(void);

void serial_cli_print(uint8_t *buf, uint8_t length);

#endif /* __SERIAL_H_ */

/************************** Coopyright (C) Jungleeee 2018 *******END OF FILE*******/

