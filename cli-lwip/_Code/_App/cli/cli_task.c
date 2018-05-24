/**
  ********************************************************************************
  * @File Name    : cli_task.c
  * @Author       : Jungle
  * @Mail         : Mail
  * @Created Time : 2018Äê3ÔÂ20ÈÕ 11:18:51
  * @Version      : V1.0
  * @Last Changed : 2018/4/11 10:52:03
  * @Brief        : brief
  ********************************************************************************
  */

/* Inlcude ---------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include <common.h>

#include <app_task.h>

#include <cli_task.h>
#include <cli_register.h>
#include <cli_common.h>

#include <net_tcp.h>

#include <serial.h>

#include "stm32f4xx.h"

/** @addtogroup Cli_Project
  * @{
  */

/* Private typedef -------------------------------------------------------------*/
/* Private constants define ----------------------------------------------------*/
/* DEL acts as a backspace */
#define     CLI_cmdASCII_DEL		    (0x7F)
#define     CLI_cmdASCII_BACKSPACE      (0x08)

#define     CLI_MAX_TX_QUEUE_DEPTH      20

#define     CLI_MAX_RX_QUEUE_LEN        100         /* Define receive queue parameters. */
#define     CLI_MAX_RX_INPUT_SIZE		50

#define     CLI_MAX_RX_QUEUE_WAIT       100

#define     CLI_MAX_TX_QUEUE_WAIT       0
#define     CLI_MAX_TX_BINARY_DMA_WAIT  20

#define     CLI_MAX_MUTEX_WAIT          200

/* Private macro define --------------------------------------------------------*/
#define     CLI_COMMOND_INFO(...)       {\
    sprintf(cCliOutputString, "\r * %s ->", pcMenuNameString);strcat(cCliOutputString, pcNULLLineString);printf("%s", cCliOutputString);\
    sprintf(cCliOutputString, "\r * %s ->", pcMenuNameString);strcat(cCliOutputString, __VA_ARGS__);printf("%s", cCliOutputString);;\
}

#define     CLI_NEW_LINE_INFO           {\
    sprintf(cCliOutputString, "%s\r * %s ->", pcNewLineString, pcMenuNameString);cli_print_info();\
}

/* Private variables -----------------------------------------------------------*/
xQueueHandle xClilRecvCharsQueue = NULL;
static xQueueHandle xCliSendCharsQueue = NULL;
static xSemaphoreHandle xCliSerialSendCompleteBinary = NULL;
static xSemaphoreHandle xSerialSendMutex = NULL;

static const char * const pcWelcomeMessageString = "\r\n Command line tool server.\r\n Intput 'help' to press enter to view the help information.\r\n\r";
static const char * const pcNULLLineString = "                                                  ";//length: CLI_MAX_RX_INPUT_SIZE
static const char * const pcNewLineString  = "\r\n";

const char * pcMenuNameString = NULL;
char cCliOutputString[CLI_MAX_TX_QUEUE_LEN];
volatile CLI_DEBUG_PEINT_STATUS xCliDebugStatus;//CLI,SYS: default open;

/* Private function declaration ------------------------------------------------*/
/* Private functions -----------------------------------------------------------*/
/**
  * @brief  : Enter the command processing task.
  * @param  : None
  * @retval : None
  */
static void cli_task_receive(void *p)
{
    static signed char cRxedChar;
    static char cInputString[CLI_MAX_RX_INPUT_SIZE], cLastInputString[CLI_MAX_RX_INPUT_SIZE] = {"cd /"};
    static uint8_t xInputStringIndex = 0;

    DEBUG_PRINT_INFO(SYS_IS_ON_DEBUG, pcWelcomeMessageString);
    pcMenuNameString = pcCmdLineString;
    CLI_NEW_LINE_INFO;

    serial_cli_receive_enable();//open receive

	for( ;; ) {
        if(xQueueReceive(xClilRecvCharsQueue, &cRxedChar, CLI_MAX_RX_QUEUE_WAIT) == pdTRUE) {
            /* Ensure exclusive access to the UART Tx. */
    		if(xSemaphoreTake(xSerialSendMutex, CLI_MAX_MUTEX_WAIT) == pdPASS) {
                if(cRxedChar == '\n' || cRxedChar == '\r') {

    				/* See if the command is empty, indicating that the last command is to be executed again. */
    				if(xInputStringIndex == 0) {

    					/* Copy the last command back into the input string. */
    					strcpy(cInputString, cLastInputString);
    				}

                    cli_interpret_command(cInputString);

    				strcpy(cLastInputString, cInputString);
    				xInputStringIndex = 0;
    				memset(cInputString, 0x00, CLI_MAX_RX_INPUT_SIZE);

                    CLI_NEW_LINE_INFO;
                }
                else {
                    if( cRxedChar == '\r' ) {/* Ignore the character. */}
                    else if((cRxedChar == CLI_cmdASCII_DEL) || (cRxedChar == CLI_cmdASCII_BACKSPACE)) {

    					/* Backspace was pressed. Erase the last character in the string - if any. */
    					if(xInputStringIndex > 0) {
    						xInputStringIndex--;
    						cInputString[xInputStringIndex] = '\0';
    					}
                    }
    				else {

    					/* A character was entered.  Add it to the string entered so far.
    					When a \n is entered the complete string will be passed to the command interpreter. */
    					if((cRxedChar >= ' ') && (cRxedChar <= '~')) {
    						if(xInputStringIndex < (CLI_MAX_RX_INPUT_SIZE - 1)) {
    							cInputString[xInputStringIndex] = cRxedChar;
    							xInputStringIndex++;
    						}
    					}
    				}

                    CLI_COMMOND_INFO((char *)cInputString);
                }

    			xSemaphoreGive(xSerialSendMutex);
            }
        }
    }
}

/**
  * @brief  : Fetch the message from the queue to the serial port.
  * @param  : None
  * @retval : None
  */
static void cli_task_send(void *p)
{
    static char cOutputQueuePop[CLI_MAX_TX_QUEUE_LEN];

	for( ;; ) {
        if(xQueueReceive(xCliSendCharsQueue, &cOutputQueuePop, CLI_MAX_TX_QUEUE_WAIT) == pdTRUE) {
#if NETWORK_CLI_FUNCTION
            tcp_cli_print(cOutputQueuePop);
#endif
            serial_cli_print((uint8_t *)cOutputQueuePop, strlen((const char *)cOutputQueuePop));
            xSemaphoreTake(xCliSerialSendCompleteBinary, CLI_MAX_TX_BINARY_DMA_WAIT);/* Wait the DMA send over & Start next */
        }
    }
}

/**
  * @brief  : The serial port receives the character callback.
  * @param  : None
  * @retval : None
  */
static void cli_receive_char_callback(uint16_t ch)
{
    unsigned char cRxedChar = (uint8_t)ch;

    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR(xClilRecvCharsQueue, &cRxedChar, &xHigherPriorityTaskWoken);
    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

/**
  * @brief  : The serial port output completes the callback.
  * @param  : None
  * @retval : None
  */
static void cli_send_frame_over_callback(void)
{
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(xCliSerialSendCompleteBinary, &xHigherPriorityTaskWoken);
    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

/**
  * @brief  : Adds an output message to the queue.
  * @param  : None
  * @retval : None
  */
void cli_print_info(void)
{
    UBaseType_t uxSavedInterruptStatus;//Interrupted critical area
    uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();//Interrupted critical area

    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR(xCliSendCharsQueue, cCliOutputString, &xHigherPriorityTaskWoken);
    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);

    taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus);//Interrupted critical area
}

//static void Test1(void *pvParameters)
//{
//	for( ;; ) {
//            vTaskDelay(100);/* solve bug */
//    }
//}
//static void Test2(void *pvParameters)
//{
//	for( ;; ) {
//            vTaskDelay(100);/* solve bug */
//    }
//}
//static void Test3(void *pvParameters)
//{
//	for( ;; ) {
//            vTaskDelay(100);/* solve bug */
//    }
//}
//static void Test4(void *pvParameters)
//{
//	for( ;; ) {
//            vTaskDelay(100);/* solve bug */
//    }
//}
//static void Test5(void *pvParameters)
//{
//	for( ;; ) {
//            vTaskDelay(100);/* solve bug */
//    }
//}
//static void Test6(void *pvParameters)
//{
//	for( ;; ) {
//            vTaskDelay(100);/* solve bug */
//    }
//}
//static void Test7(void *pvParameters)
//{
//	for( ;; ) {
//            vTaskDelay(100);/* solve bug */
//    }
//}
//static void Test8(void *pvParameters)
//{
//	for( ;; ) {
//            vTaskDelay(100);/* solve bug */
//    }
//}
/**
  * @brief  : Create commond line receive & send task
  * @param  : None
  * @retval : None
  */
void cli_task_create(void)
{
    if(xClilRecvCharsQueue == NULL) {
        xClilRecvCharsQueue = xQueueCreate(CLI_MAX_RX_QUEUE_LEN, sizeof(uint8_t));
        configASSERT(xClilRecvCharsQueue);
    }

    if(xCliSendCharsQueue == NULL) {
        xCliSendCharsQueue = xQueueCreate(CLI_MAX_TX_QUEUE_DEPTH, sizeof(uint8_t) * CLI_MAX_TX_QUEUE_LEN);
        configASSERT(xCliSendCharsQueue);
    }

    if(xCliSerialSendCompleteBinary == NULL) {
    	xCliSerialSendCompleteBinary = xSemaphoreCreateBinary();
    	configASSERT(xCliSerialSendCompleteBinary);
    }

    if(xSerialSendMutex == NULL) {
    	xSerialSendMutex = xSemaphoreCreateMutex();
    	configASSERT(xSerialSendMutex);
    }

    serial_cli_init(115200, cli_receive_char_callback);
    serial_cli_print_init(cli_send_frame_over_callback);

    xCliDebugStatus.cliDebugStatus = CLI_DEBUG_DEFAULT_STATUS;

    xTaskCreate(cli_task_send,    (const char *)"cliSendTask",    CLI_TASK_SEND_STACK_DEPTH,    NULL, CLI_TASK_SEND_PRIORITY, NULL);
    xTaskCreate(cli_task_receive, (const char *)"cliReceiveTask", CLI_TASK_RECEIVE_STACK_DEPTH, NULL, CLI_TASK_RECEIVE_PRIORITY, NULL);
//    xTaskCreate(Test1,            (const char *)"Test1",          128, NULL, 1, NULL);
//    xTaskCreate(Test2,            (const char *)"Test2",          128, NULL, 1, NULL);
//    xTaskCreate(Test3,            (const char *)"Test3",          128, NULL, 1, NULL);
//    xTaskCreate(Test4,            (const char *)"Test4",          128, NULL, 1, NULL);
//    xTaskCreate(Test5,            (const char *)"Test5",          128, NULL, 1, NULL);
//    xTaskCreate(Test6,            (const char *)"Test6",          128, NULL, 1, NULL);
//    xTaskCreate(Test7,            (const char *)"Test7",          128, NULL, 1, NULL);
//    xTaskCreate(Test8,            (const char *)"Test8",          128, NULL, 1, NULL);
}

/**
  * @}
  */

/************************** Coopyright (C) Jungleeee 2018 *******END OF FILE*******/

