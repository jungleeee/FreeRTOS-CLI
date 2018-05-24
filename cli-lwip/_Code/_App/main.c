/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/main.c
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"

#include "stm32f4xx.h"
#include "core_cmFunc.h"

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void init(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
    /* Infinite */
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x08000000);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    __set_FAULTMASK(1);                 /* close all interrupt */

    init();

    vTaskStartScheduler();              /* systerm run to start */
    /* Infinite loop */
    while (1)
    {
    }
}

/**
  * @brief  Inserts a delay time.
  * @param  nCount: number of Ticks to delay.
  * @retval None
  */
void Delay(uint32_t ms)
{
  vTaskDelay(ms);
}

/**
  * @brief  __soft_reboot__
  * @param  None
  * @retval None
  */
void __soft_reboot__(void)
{
    __set_FAULTMASK(1);                 /* close interrupt */
    NVIC_SystemReset();                 /* systerm restart */
}


/**
  * @brief  init
  * @param  None
  * @retval None
  */
static void init(void)
{
    extern void cli_task_create(void);
    extern void led_task_create(void);
    extern void tcp_task_create(void);
    //extern void network_init(void);

    cli_task_create();
    led_task_create();
    tcp_task_create();
    //network_init();
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1)
    {
    }
}
#endif

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
