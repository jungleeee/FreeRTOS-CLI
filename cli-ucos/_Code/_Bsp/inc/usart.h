/**
  ********************************************************************************
  * @File Name    : usart.h
  * @Author       : Jungle
  * @Mail         : Mail
  * @Created Time : 2018年3月19日 14:35:19
  * @Version      : V1.0
  * @Last Changed : 2018年3月19日 17:04:06
  * @Brief        : brief
  ********************************************************************************
  */

/* Define to prevent recursive inclusion ---------------------------------------*/
#ifndef __USART_H_
#define __USART_H_

/* Inlcude ---------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Exported typedef ------------------------------------------------------------*/
/* Exported constants define ---------------------------------------------------*/
/* Exported macro define -------------------------------------------------------*/
#define     debug_uart_RX_pin               GPIO_Pin_10
#define     debug_uart_RX_gpio_port         GPIOA
#define     debug_uart_RX_gpio_port_clk     RCC_AHB1Periph_GPIOA
#define     debug_uart_TX_pin               GPIO_Pin_9
#define     debug_uart_TX_gpio_port         GPIOA
#define     debug_uart_TX_gpio_port_clk     RCC_AHB1Periph_GPIOA
#define     debug_uart                      USART1
#define     debug_uart_clk                  RCC_APB2Periph_USART1
#define     debug_uart_IRQn                 USART1_IRQn
#define     debug_uart_DMA_TX_Chl           DMA_Channel_4
#define     debug_uart_DMA_TX_Str           DMA2_Stream7
#define     debug_uart_DMA_IRQn             DMA2_Stream7_IRQn

/* Exported variables ----------------------------------------------------------*/
/* Exported functions ----------------------------------------------------------*/
void uart_debug_init(int baudRate, void (* const debugResvOneByteCallBack)(uint16_t));
void uart_debug_DMA_init(void (* const debugPrintOverCallBack)(void));

void uart_send_data_by_DMA(const USART_TypeDef* USARTx, uint8_t *buf, uint8_t length);

#endif /* __USART_H_ */

/************************** Coopyright (C) Jungleeee 2018 *******END OF FILE*******/

