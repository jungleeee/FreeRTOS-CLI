/**
  ********************************************************************************
  * @File Name    : usart.h
  * @Author       : Jungle
  * @Mail         : Mail
  * @Created Time : 2018Äê3ÔÂ19ÈÕ 14:35:19
  * @Version      : V1.0
  * @Last Changed : 2018/4/11 10:51:39
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
#define     debug_usart_RX_pin               GPIO_Pin_10
#define     debug_usart_RX_gpio_port         GPIOA
#define     debug_usart_RX_gpio_port_clk     RCC_AHB1Periph_GPIOA
#define     debug_usart_TX_pin               GPIO_Pin_9
#define     debug_usart_TX_gpio_port         GPIOA
#define     debug_usart_TX_gpio_port_clk     RCC_AHB1Periph_GPIOA
#define     debug_usart                      USART1
#define     debug_usart_clk                  RCC_APB2Periph_USART1
#define     debug_usart_IRQn                 USART1_IRQn
#define     debug_usart_DMA_TX_Chl           DMA_Channel_4
#define     debug_usart_DMA_TX_Str           DMA2_Stream7
#define     debug_usart_DMA_IRQn             DMA2_Stream7_IRQn

/* Exported variables ----------------------------------------------------------*/
/* Exported functions ----------------------------------------------------------*/
void usart_debug_init(int baudRate, void (* const debugResvOneByteCallBack)(uint16_t));
void usart_debug_DMA_init(void (* const debugPrintOverCallBack)(void));

void usart_debug_RX_ISR_enable(void);

void usart_send_data_by_DMA(const USART_TypeDef* USARTx, uint8_t *buf, uint8_t length);

#endif /* __USART_H_ */

/************************** Coopyright (C) Jungleeee 2018 *******END OF FILE*******/

