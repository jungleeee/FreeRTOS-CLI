/**
  ********************************************************************************
  * @File Name    : serial.c
  * @Author       : Jungle
  * @Mail         : Mail
  * @Created Time : 2018年3月19日 17:06:47
  * @Version      : V1.0
  * @Last Changed : 2018年3月23日 17:45:36
  * @Brief        : brief
  ********************************************************************************
  */

/* Inlcude ---------------------------------------------------------------------*/
#include <serial.h>
#include <usart.h>

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
  * @brief  : commond line serial init
  * @param  : baud Rate
  * @param  : the pointer of receive callback function
  * @retval : None
  */
void serial_cli_init(int baudRate, void (* const cliReceiveOneCharCallBack)(uint16_t))
{
    uart_debug_init(baudRate, cliReceiveOneCharCallBack);
}

/**
  * @brief  : commond line serial print info enable
  * @param  : the pointer of print info complete callback function
  * @retval : None
  */
void serial_cli_printInfo_init(void (* const cliPrintInfoOverCallBack)(void))
{
    uart_debug_DMA_init(cliPrintInfoOverCallBack);
}

/**
  * @brief  : print info
  * @param  : pointer of buf: data buffer
  * @param  : length: data buffer length
  * @retval : None
  */
void serial_cli_print_info(uint8_t *buf, uint8_t length)
{
    uart_send_data_by_DMA(debug_uart, buf, length);
}

/**
  * @}
  */

/************************** Coopyright (C) Jungleeee 2018 *******END OF FILE*******/

