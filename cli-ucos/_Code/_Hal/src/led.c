/**
  ********************************************************************************
  * @File Name    : led.c
  * @Author       : Jungle
  * @Mail         : Mail
  * @Created Time : 2018/3/30 15:25:49
  * @Version      : V1.0
  * @Last Changed : 2018/3/30 15:25:49
  * @Brief        : brief
  ********************************************************************************
  */

/* Inlcude ---------------------------------------------------------------------*/
#include <gpio.h>

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
  * @brief  : System status light control initialization.
  * @param  : None
  * @retval : None
  */
void led_set_SYS_init(void)
{
    gpio_system_led_init();
}

/**
  * @brief  : Control system status light.
  * @param  : status: 0: open; !0: close
  * @retval : None
  */
void led_set_SYS_status(unsigned char status)
{
    if(!status) sys_led_gpio_set;
    else sys_led_gpio_reset;
}

/**
  * @brief  : Gets the system lamp status.
  * @param  : None
  * @retval : 1: open; 0: close
  */
unsigned char led_get_SYS_status(void)
{
    return (unsigned char)sys_led_gpio_bit;
}

/**
  * @}
  */

/************************** Coopyright (C) Jungleeee 2018 *******END OF FILE*******/

