/**
  ********************************************************************************
  * @File Name    : led.h
  * @Author       : Jungle
  * @Mail         : Mail
  * @Created Time : 2018/3/30 15:26:21
  * @Version      : V1.0
  * @Last Changed : 2018/3/30 15:26:21
  * @Brief        : brief
  ********************************************************************************
  */

/* Define to prevent recursive inclusion ---------------------------------------*/
#ifndef __LED_H_
#define __LED_H_

/* Inlcude ---------------------------------------------------------------------*/

/* Exported typedef ------------------------------------------------------------*/
/* Exported constants define ---------------------------------------------------*/
/* Exported macro define -------------------------------------------------------*/
/* Exported variables ----------------------------------------------------------*/
/* Exported functions ----------------------------------------------------------*/
void            led_set_SYS_init(void);
void            led_set_SYS_status(unsigned int status);
unsigned int    led_get_SYS_status(void);

#endif /* __LED_H_ */

/************************** Coopyright (C) Jungleeee 2018 *******END OF FILE*******/

