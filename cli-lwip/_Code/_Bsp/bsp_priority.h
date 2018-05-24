/**
  ********************************************************************************
  * @File Name    : bsp_priority.h
  * @Author       : Jungle
  * @Mail         : Mail
  * @Created Time : 2018/4/10 15:42:57
  * @Version      : V1.0
  * @Last Changed : 2018/4/10 15:42:57
  * @Brief        : brief
  ********************************************************************************
  */

/* Define to prevent recursive inclusion ---------------------------------------*/
#ifndef __BSP_PRIORITY_H_
#define __BSP_PRIORITY_H_

/* Inlcude ---------------------------------------------------------------------*/

/* Exported typedef ------------------------------------------------------------*/
/* Exported constants define ---------------------------------------------------*/
#define     usart_debug_RX_COMPLETE_ISR_priority            14
#define     usart_debug_DMA_TX_COMPLETE_ISR_priority        15


/* Exported macro define -------------------------------------------------------*/
#define     NVIC_IRQChannel_Sub_priority                    0

/* Exported variables ----------------------------------------------------------*/
/* Exported functions ----------------------------------------------------------*/

#endif /* __BSP_PRIORITY_H_ */

/************************** Coopyright (C) Jungleeee 2018 *******END OF FILE*******/

