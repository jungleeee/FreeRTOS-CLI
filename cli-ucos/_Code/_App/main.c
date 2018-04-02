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
#include <task.h>

#include "stm32f4xx.h"

#include <cli_task.h>
#include <led.h>

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define     SYS_init     led_set_SYS_init();
#define     CLI_init     cli_task_create();

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void SysTick_Init(void);

/* Private functions ---------------------------------------------------------*/
static void startTask(void *p_arg)
{
    OS_ERR err;

    SYS_init;
    CLI_init;

    for( ;; ) {
        if(led_get_SYS_status())
            led_set_SYS_status(1);
        else
            led_set_SYS_status(0);

        OSTimeDlyHMSM(0u, 0u, 0u, 100u,
                      OS_OPT_TIME_HMSM_STRICT,
                      &err);
    }
}

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
    /* Infinite */
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x08010000);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    //__set_FAULTMASK(1);                 /* close all interrupt */
    OS_ERR err;

    CPU_Init();
    SysTick_Init();

    OSInit(&err);
    if(err != OS_ERR_NONE) {
        /* Something didn¡¯t get initialized correctly ... */
        /* ... check os.h for the meaning of the error code, see OS_ERR_xxxx */
    }

    OSTaskCreate((OS_TCB       *)&startTaskTCB,              /* Create the start task */
                 (CPU_CHAR     *)"startTask",
                 (OS_TASK_PTR   )startTask,
                 (void         *)0u,
                 (OS_PRIO       )TASK_CFG_START_TASK_PRIO,
                 (CPU_STK      *)&startTaskStk[0u],
                 (CPU_STK_SIZE  )startTaskStk[TASK_CFG_START_TASK_STK_SIZE / 10u],
                 (CPU_STK_SIZE  )TASK_CFG_START_TASK_STK_SIZE,
                 (OS_MSG_QTY    )0u,
                 (OS_TICK       )0u,
                 (void         *)0u,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);

    if(err != OS_ERR_NONE) {
        ;/* The task didn¡¯t get created. Lookup the value of the error code ... */
        /* ... in os.h for the meaning of the error */
    }

    OSStart(&err);                                             /* Start multitasking (i.e. give control to uC/OS-III). */
    if(err != OS_ERR_NONE) {
        ;/* Your code is NEVER supposed to come back to this point. */
    }

    (void)&err;

    /* Infinite loop */
    while (1)
    {
    }
}

/**
  * @brief  SysTick timer init
  * @param  None
  * @retval None
  */
static void SysTick_Init(void)
{
    RCC_ClocksTypeDef  rcc_clocks;
    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;

    RCC_GetClocksFreq(&rcc_clocks);
    cpu_clk_freq = (CPU_INT32U)rcc_clocks.HCLK_Frequency;       /* Determine SysTick reference freq.                    */

#if (OS_VERSION >= 30000u)
    cnts = (cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz);      /* Determine nbr SysTick increments                     */
#else
    cnts = (cpu_clk_freq / (CPU_INT32U)OS_TICKS_PER_SEC);       /* Determine nbr SysTick increments.                    */
#endif

    OS_CPU_SysTickInit(cnts);
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
