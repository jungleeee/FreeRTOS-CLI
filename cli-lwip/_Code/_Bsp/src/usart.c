/**
  ********************************************************************************
  * @File Name    : usart.c
  * @Author       : Jungle
  * @Mail         : Mail
  * @Created Time : 2018年3月19日 14:34:42
  * @Version      : V1.0
  * @Last Changed : 2018/4/11 10:51:37
  * @Brief        : brief
  ********************************************************************************
  */

/* Inlcude ---------------------------------------------------------------------*/
#include "stdio.h"

#include "bsp_priority.h"
#include <usart.h>

/** @addtogroup Cli_Project
  * @{
  */

/* Private typedef -------------------------------------------------------------*/
/* Private constants define ----------------------------------------------------*/
/* Private macro define --------------------------------------------------------*/
#define     usart_debug_IRQHandler           USART1_IRQHandler
#define     usart_debug_DMA_IRQHandler       DMA2_Stream7_IRQHandler

/* Private variables -----------------------------------------------------------*/
/* Private function declaration ------------------------------------------------*/
static volatile void (* usart_debug_recv_one_byte_callback_isr)(uint16_t) = NULL;
static volatile void (* usart_debug_DMA_send_over_callback_isr)(void) = NULL;

/* Private functions -----------------------------------------------------------*/
/**
  * @brief  : debug usart init: 8bits & 1 stop bit
  * @param  : baud Rate
  * @param  : pointer of usart receive interrupt
  * @retval : None
  */
void usart_debug_init(int baudRate, void (* const debugResvOneByteCallBack)(uint16_t))
{
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(debug_usart_RX_gpio_port_clk, ENABLE);
    RCC_AHB1PeriphClockCmd(debug_usart_TX_gpio_port_clk, ENABLE);
    RCC_APB2PeriphClockCmd(debug_usart_clk, ENABLE);

    GPIO_InitStructure.GPIO_Pin = debug_usart_TX_pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(debug_usart_RX_gpio_port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = debug_usart_RX_pin;
	GPIO_Init(debug_usart_RX_gpio_port, &GPIO_InitStructure);

	GPIO_PinAFConfig(debug_usart_RX_gpio_port, GPIO_PinSource10, GPIO_AF_USART1);
	GPIO_PinAFConfig(debug_usart_TX_gpio_port, GPIO_PinSource9,  GPIO_AF_USART1);

	USART_InitStructure.USART_BaudRate = baudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(debug_usart, &USART_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = debug_usart_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = usart_debug_RX_COMPLETE_ISR_priority;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_IRQChannel_Sub_priority;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

    if(debugResvOneByteCallBack != NULL) usart_debug_recv_one_byte_callback_isr = (void volatile(*)(uint16_t))debugResvOneByteCallBack;

    USART_Cmd(debug_usart, ENABLE);
    USART_ITConfig(debug_usart, USART_IT_TC, DISABLE);
    USART_ITConfig(debug_usart, USART_IT_RXNE, DISABLE);
	USART_ClearFlag(debug_usart, USART_FLAG_TC | USART_FLAG_RXNE);
}

/**
  * @brief  : debug usart DMA init
  * @param  : pointer of DMA transmit complete callback function
  * @retval : None
  */
void usart_debug_DMA_init(void (* const debugPrintOverCallBack)(void))
{
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);                        /* Enable DMA2 CLK */

    /* CH4，DMA Uart send */
    DMA_DeInit(debug_usart_DMA_TX_Str);						                    /* DMA2 */
    while(DMA_GetCmdStatus(debug_usart_DMA_TX_Str) != DISABLE);

    DMA_InitStructure.DMA_Channel = debug_usart_DMA_TX_Chl;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART1->DR);	        ///* Peripheral address */
    DMA_InitStructure.DMA_Memory0BaseAddr = NULL;	                            ///* Memory address */
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;				        ///* DMA dir Memory To Peripheral */
    DMA_InitStructure.DMA_BufferSize = 0;			                            ///* buf size */
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		    ///* Peripheral address add, disable */
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;			            ///* Memory address add, enable */
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	    ///* Peripheral data length */
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	    	    ///* Memory data length */
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;				                ///* no circle */
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;				        ///* Priority */
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;                      ///* Direct mode */
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;                 ///* Single */
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;         ///* Single */
    DMA_Init(debug_usart_DMA_TX_Str, &DMA_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = debug_usart_DMA_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = usart_debug_DMA_TX_COMPLETE_ISR_priority;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_IRQChannel_Sub_priority;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

    if(debugPrintOverCallBack != NULL) usart_debug_DMA_send_over_callback_isr = (void volatile(*)(void))debugPrintOverCallBack;

    DMA_ITConfig(debug_usart_DMA_TX_Str, DMA_IT_TC, ENABLE);
    DMA_ClearFlag(debug_usart_DMA_TX_Str, DMA_FLAG_TCIF7);
    /* Must DISABLE
       Otherwise, It will trigger a DMA completion interrupt, generating semaphore. */
    DMA_Cmd(debug_usart_DMA_TX_Str, DISABLE);
    USART_DMACmd(debug_usart, USART_DMAReq_Tx, ENABLE);                          /* enable USART1 transe DMA */
}

/**
  * @brief  enable USART1 receive interrupt:
  * @param  None
  * @retval None
  */
void usart_debug_RX_ISR_enable(void)
{
    USART_ITConfig(debug_usart, USART_IT_RXNE, ENABLE);//open receive
}

/**
  * @brief  debug_usart_IRQHandler:
  * @param  None
  * @retval None
  */
void usart_debug_IRQHandler(void)
{
	if(USART_GetITStatus(debug_usart, USART_IT_RXNE) != RESET) {
        if(usart_debug_recv_one_byte_callback_isr != NULL) {
            usart_debug_recv_one_byte_callback_isr(USART_ReceiveData(debug_usart));
        }
        USART_ClearITPendingBit(debug_usart, USART_IT_RXNE);
    }
	else if(USART_GetITStatus(debug_usart, USART_IT_TC) != RESET) {
        USART_ClearITPendingBit(debug_usart, USART_IT_TC);
    }
}

/**
  * @brief  usart_debug_DMA_IRQHandler:
  * @param  None
  * @retval None
  */
void usart_debug_DMA_IRQHandler(void)
{
    if(DMA_GetITStatus(debug_usart_DMA_TX_Str, DMA_IT_TCIF7) != RESET) {
        if(usart_debug_DMA_send_over_callback_isr != NULL) {
            usart_debug_DMA_send_over_callback_isr();
        }
        DMA_ClearITPendingBit(debug_usart_DMA_TX_Str, DMA_IT_TCIF7);
    }
}

/**
  * @brief  USART_SendBufByDMA
  * @param  None
  * @retval None
  */
void usart_send_data_by_DMA(const USART_TypeDef* USARTx, uint8_t *buf, uint8_t length)
{
    if(USARTx == debug_usart) {
        debug_usart_DMA_TX_Str->CR &= ((uint32_t)0xFFFFFFFE);    /* Disable DMA */
        debug_usart_DMA_TX_Str->M0AR = (uint32_t)buf;            /* set memory addr */
        debug_usart_DMA_TX_Str->NDTR = length;
        debug_usart_DMA_TX_Str->CR |= ((uint32_t)0x00000001);    /* Enable DMA */
    }
}

/**
  * @brief  ues printf to debug
  * @param  None
  * @retval None
  */
#if 1
#pragma import(__use_no_semihosting) /* 标准库需要的支持函数 */
struct __FILE
{
	int handle;
};
FILE __stdout;

void _sys_exit(int x)                /* 定义_sys_exit()以避免使用半主机模式 */
{
	x = x;
}

int fputc(int ch, FILE *f)           /* 重定向fputc函数 */
{
	USART_SendData(debug_usart, (uint8_t)ch);
    while(USART_GetFlagStatus(debug_usart, USART_FLAG_TXE) == RESET);    /* 循环发送,直到发送完毕 */

	return ch;
}
#endif

/**
  * @}
  */

/************************** Coopyright (C) Jungleeee 2018 *******END OF FILE*******/

