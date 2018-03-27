/**
  ********************************************************************************
  * @File Name    : cli_task.c
  * @Author       : Jungle
  * @Mail         : Mail
  * @Created Time : 2018年3月20日 11:18:51
  * @Version      : V1.0
  * @Last Changed : 2018年3月27日 13:13:33
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
#include <cli_task.h>
#include <cli_register.h>

#include <serial.h>

#include "stm32f4xx.h"

/** @addtogroup Cli_Project
  * @{
  */

/* Private typedef -------------------------------------------------------------*/
/* Private constants define ----------------------------------------------------*/
/* DEL acts as a backspace */
#define     cli_cmdASCII_DEL		    (0x7F)
#define     cli_cmdASCII_BACKSPACE      (0x08)

#define     cli_MAX_TX_QUEUE_DEPTH      20

#define     cli_MAX_RX_QUEUE_LEN        100         /* Define receive queue parameters. */
#define     cli_MAX_RX_INPUT_SIZE		50

#define     cli_MAX_RX_QUEUE_WAIT       0

#define     cli_MAX_TX_QUEUE_WAIT       0
#define     cli_MAX_TX_BINARY_DMA_WAIT  20

#define     cli_MAX_MUTEX_WAIT          200

/* Private macro define --------------------------------------------------------*/
#define     cliCommondInfo(...)         {\
    sprintf(cCliOutputString, "\r * %s ->", pcMenuNameString);strcat(cCliOutputString, pcNULLLineString);cli_print_info();\
    sprintf(cCliOutputString, "\r * %s ->", pcMenuNameString);strcat(cCliOutputString, __VA_ARGS__);cli_print_info();\
}

#define     cliNewLineInfo              {\
    sprintf(cCliOutputString, "%s\r * %s ->", pcNewLineString, pcMenuNameString);cli_print_info();\
}

/* Private variables -----------------------------------------------------------*/
static xQueueHandle xSerialRecvCharsQueue = NULL;
static xQueueHandle xSerialSendCharsQueue = NULL;
static xSemaphoreHandle xSerialSendCompleteBinary = NULL;
static xSemaphoreHandle xSerialSendMutex = NULL;

static const char * const pcWelcomeMessageString = "\r\n Command line tool server.\r\n Intput 'help' to press enter to view the help information.\r\n\r";
static const char * const pcNULLLineString = "                                                  ";//length: cli_MAX_RX_INPUT_SIZE
static const char * const pcNewLineString  = "\r\n";

const char * pcMenuNameString = NULL;
char cCliOutputString[cli_MAX_TX_QUEUE_LEN];
volatile CLI_DEBUG_PEINT_STATUS xCliDebugStatus;//CLI,SYS: default open;

/* Private function declaration ------------------------------------------------*/
/* Private functions -----------------------------------------------------------*/
/**
  * @brief  : Enter the command processing task.
  * @param  : None
  * @retval : None
  */
static void cli_task_receive(void *pvParameters)
{
    static signed char cRxedChar;
    static char cInputString[cli_MAX_RX_INPUT_SIZE], cLastInputString[cli_MAX_RX_INPUT_SIZE] = {"cd /"};
    uint8_t xInputStringIndex = 0;

    debugPrintInfo(SYS_isDebugOn, pcWelcomeMessageString);
    pcMenuNameString = pcCmdLineString;
    cliNewLineInfo;

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//open receive

	for( ;; ) {
        if(xQueueReceive(xSerialRecvCharsQueue, &cRxedChar, cli_MAX_RX_QUEUE_WAIT) == pdTRUE) {
            /* Ensure exclusive access to the UART Tx. */
    		if(xSemaphoreTake(xSerialSendMutex, cli_MAX_MUTEX_WAIT) == pdPASS) {
                if(cRxedChar == '\n' || cRxedChar == '\r') {

    				/* See if the command is empty, indicating that the last command is to be executed again. */
    				if(xInputStringIndex == 0) {

    					/* Copy the last command back into the input string. */
    					strcpy(cInputString, cLastInputString);
    				}

                    cli_interpret_command(cInputString, cCliOutputString, cli_MAX_TX_QUEUE_LEN);

    				strcpy(cLastInputString, cInputString);
    				xInputStringIndex = 0;
    				memset(cInputString, 0x00, cli_MAX_RX_INPUT_SIZE);

                    cliNewLineInfo;
                }
                else {
                    if( cRxedChar == '\r' ) {/* Ignore the character. */}
                    else if((cRxedChar == cli_cmdASCII_DEL) || (cRxedChar == cli_cmdASCII_BACKSPACE)) {

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
    						if(xInputStringIndex < (cli_MAX_RX_INPUT_SIZE - 1)) {
    							cInputString[xInputStringIndex] = cRxedChar;
    							xInputStringIndex++;
    						}
    					}
    				}

                    cliCommondInfo((char *)cInputString);
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
static void cli_task_send(void *pvParameters)
{
    static uint8_t cOutputQueuePull[cli_MAX_TX_QUEUE_LEN];

	for( ;; ) {
        if(xQueueReceive(xSerialSendCharsQueue, &cOutputQueuePull, cli_MAX_TX_QUEUE_WAIT) == pdTRUE) {
            serial_cli_print_info(cOutputQueuePull, strlen((const char *)cOutputQueuePull));
            xSemaphoreTake(xSerialSendCompleteBinary, cli_MAX_TX_BINARY_DMA_WAIT);/* Wait the DMA send over & Start next */
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
    xQueueSendFromISR(xSerialRecvCharsQueue, &cRxedChar, &xHigherPriorityTaskWoken);
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
    xSemaphoreGiveFromISR(xSerialSendCompleteBinary, &xHigherPriorityTaskWoken);
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
    xQueueSendFromISR(xSerialSendCharsQueue, cCliOutputString, &xHigherPriorityTaskWoken);
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
    if(xSerialRecvCharsQueue == NULL) {
        xSerialRecvCharsQueue = xQueueCreate(cli_MAX_RX_QUEUE_LEN, sizeof(uint8_t));
        configASSERT(xSerialRecvCharsQueue);
    }

    if(xSerialSendCharsQueue == NULL) {
        xSerialSendCharsQueue = xQueueCreate(cli_MAX_TX_QUEUE_DEPTH, sizeof(uint8_t) * cli_MAX_TX_QUEUE_LEN);
        configASSERT(xSerialSendCharsQueue);
    }

    if(xSerialSendCompleteBinary == NULL) {
    	xSerialSendCompleteBinary = xSemaphoreCreateBinary();
    	configASSERT(xSerialSendCompleteBinary);
    }

    if(xSerialSendMutex == NULL) {
    	xSerialSendMutex = xSemaphoreCreateMutex();
    	configASSERT(xSerialSendMutex);
    }

    serial_cli_init(115200, cli_receive_char_callback);
    serial_cli_printInfo_init(cli_send_frame_over_callback);

    xCliDebugStatus.cliDebugStatus = cli_DEBUG_DEFAULT_STATUS;

    xTaskCreate(cli_task_send,    (const char *)"cliSendTask",    128, NULL, 1, NULL);
    xTaskCreate(cli_task_receive, (const char *)"cliReceiveTask", 256, NULL, 1, NULL);
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

