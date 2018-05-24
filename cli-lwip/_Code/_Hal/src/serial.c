/**
  ********************************************************************************
  * @File Name    : serial.c
  * @Author       : Jungle
  * @Mail         : Mail
  * @Created Time : 2018Äê3ÔÂ19ÈÕ 17:06:47
  * @Version      : V1.0
  * @Last Changed : 2018/4/11 10:51:31
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
    usart_debug_init(baudRate, cliReceiveOneCharCallBack);
}

/**
  * @brief  : commond line serial print info enable
  * @param  : the pointer of print info complete callback function
  * @retval : None
  */
void serial_cli_print_init(void (* const cliPrintOverCallBack)(void))
{
    usart_debug_DMA_init(cliPrintOverCallBack);
}

/**
  * @brief  : commond line serial receive enable
  * @param  : None
  * @retval : None
  */
void serial_cli_receive_enable(void)
{
    usart_debug_RX_ISR_enable();
}

/**
  * @brief  : print info
  * @param  : pointer of buf: data buffer
  * @param  : length: data buffer length
  * @retval : None
  */
void serial_cli_print(uint8_t *buf, uint8_t length)
{
    usart_send_data_by_DMA(debug_usart, buf, length);
}

/**
  * @}
  */

/************************** Coopyright (C) Jungleeee 2018 *******END OF FILE*******/

