/**
  ********************************************************************************
  * @File Name    : gpio.h
  * @Author       : Jungle
  * @Mail         : Mail
  * @Created Time : 2018/3/29 14:45:32
  * @Version      : V1.0
  * @Last Changed : 2018/3/29 14:45:39
  * @Brief        : brief
  ********************************************************************************
  */

/* Define to prevent recursive inclusion ---------------------------------------*/
#ifndef __GPIO_H_
#define __GPIO_H_

/* Inlcude ---------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Exported typedef ------------------------------------------------------------*/
/* Exported constants define ---------------------------------------------------*/
/* Exported macro define -------------------------------------------------------*/
#define sys_led_pin             GPIO_Pin_0
#define sys_led_gpio_port       GPIOC
#define sys_led_gpio_clk        RCC_AHB1Periph_GPIOC
#define sys_led_gpio_bit        GPIO_ReadOutputDataBit(sys_led_gpio_port, sys_led_pin)
#define sys_led_gpio_set        GPIO_SetBits(sys_led_gpio_port, sys_led_pin)
#define sys_led_gpio_reset      GPIO_ResetBits(sys_led_gpio_port, sys_led_pin)

/* Exported variables ----------------------------------------------------------*/
/* Exported functions ----------------------------------------------------------*/
void gpio_system_led_init(void);

#endif /* __GPIO_H_ */

/************************** Coopyright (C) Jungleeee 2018 *******END OF FILE*******/

