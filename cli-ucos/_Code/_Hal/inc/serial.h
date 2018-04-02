/**
  ********************************************************************************
  * @File Name    : serial.h
  * @Author       : Jungle
  * @Mail         : Mail
  * @Created Time : 2018年3月19日 17:21:38
  * @Version      : V1.0
  * @Last Changed : 2018年3月23日 17:45:37
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
void serial_cli_printInfo_init(void (* const cliPrintInfoOverCallBack)(void));
void serial_cli_print_info(uint8_t *buf, uint8_t length);

#endif /* __SERIAL_H_ */

/************************** Coopyright (C) Jungleeee 2018 *******END OF FILE*******/

