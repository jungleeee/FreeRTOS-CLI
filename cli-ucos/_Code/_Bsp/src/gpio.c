/**
  ********************************************************************************
  * @File Name    : gpio.c
  * @Author       : Jungle
  * @Mail         : Mail
  * @Created Time : 2018/3/29 14:44:56
  * @Version      : V1.0
  * @Last Changed : 2018/3/29 14:44:56
  * @Brief        : brief
  ********************************************************************************
  */

/* Inlcude ---------------------------------------------------------------------*/
#include "gpio.h"

/** @addtogroup Cli_Project
  * @{
  */

/* Private typedef -------------------------------------------------------------*/
/* Private constants define ----------------------------------------------------*/
/* Private macro define --------------------------------------------------------*/
/* Private variables -----------------------------------------------------------*/
/* Private function declaration ------------------------------------------------*/
/* Private functions -----------------------------------------------------------*/
/**
  * @brief  gpio_system_led_init
  * @param  None
  * @retval None
  */
void gpio_system_led_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructre;

    RCC_AHB1PeriphClockCmd(sys_led_gpio_clk,ENABLE);
      
    GPIO_InitStructre.GPIO_Pin = sys_led_pin;
    GPIO_InitStructre.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructre.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructre.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructre.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(sys_led_gpio_port, &GPIO_InitStructre);

    sys_led_gpio_set;
}

/**
  * @}
  */

/************************** Coopyright (C) Jungleeee 2018 *******END OF FILE*******/

