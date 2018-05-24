/**
  ********************************************************************************
  * @File Name    : led_task.c
  * @Author       : Jungle
  * @Mail         : Mail
  * @Created Time : 2018/4/10 16:00:08
  * @Version      : V1.0
  * @Last Changed : 2018/4/10 16:00:08
  * @Brief        : brief
  ********************************************************************************
  */

/* Inlcude ---------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"

#include <common.h>

#include <app_task.h>
#include <led_task.h>

#include <led.h>

/** @addtogroup Cli_Project
  * @{
  */

/* Private typedef -------------------------------------------------------------*/
/* Private constants define ----------------------------------------------------*/
/* Private macro define --------------------------------------------------------*/
#define     IS_ON_MAINBOARD_LED          led_get_SYS_status()
#define     SET_ON_MAINBOARD_LED         led_set_SYS_status(ON)
#define     SET_OFF_MAINBOARD_LED        led_set_SYS_status(OFF)

/* Private variables -----------------------------------------------------------*/
/* Private function declaration ------------------------------------------------*/
static void led_mainBoard_blink(void);

/* Private functions -----------------------------------------------------------*/
/**
  * @brief  : led task
  * @param  : None
  * @retval : None
  */
static void led_task(void *p)
{
    const TickType_t kFrequency = 100;
    TickType_t xLastWakeMoment = xTaskGetTickCount();

    for( ;; ) {
        led_mainBoard_blink();

        vTaskDelayUntil(&xLastWakeMoment, kFrequency);
    }
}

/**
  * @brief  : mainBoard Led blink
  * @param  : None
  * @retval : None
  */
static void led_mainBoard_blink(void)
{
    if(IS_ON_MAINBOARD_LED) SET_OFF_MAINBOARD_LED;
    else SET_ON_MAINBOARD_LED;
}

/**
  * @brief  : create led task
  * @param  : None
  * @retval : None
  */
void led_task_create(void)
{
    led_set_SYS_init();

    xTaskCreate(led_task, (const char *)"ledTask", LED_TASK_STACK_DEPTH, NULL, LED_TASK_PRIORITY, NULL);
}

/**
  * @}
  */

/************************** Coopyright (C) Jungleeee 2018 *******END OF FILE*******/

