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
#include "task.h"

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

#define     cli_MAX_RX_QUEUE_DEPTH      25         /* Define receive queue parameters. */
#define     cli_MAX_RX_QUEUE_LEN        4          /* Minimum: 4. */

#define     cli_MAX_RX_INPUT_SIZE		50

#define     cli_MAX_RX_QUEUE_WAIT       0

#define     cli_MAX_TX_QUEUE_WAIT       0
#define     cli_MAX_TX_BINARY_DMA_WAIT  20

/* Private macro define --------------------------------------------------------*/
#define     cliCommondInfo(...)         { \
    if(cli_print_malloc() == TRUE) {sprintf(pcCliOutputString, "\r * %s ->", pcMenuNameString);strcat(pcCliOutputString, pcNULLLineString);cli_print_info();}\
    if(cli_print_malloc() == TRUE) {sprintf(pcCliOutputString, "\r * %s ->", pcMenuNameString);strcat(pcCliOutputString, __VA_ARGS__);cli_print_info();}\
}

#define     cliNewLineInfo              { \
    if(cli_print_malloc() == TRUE) {sprintf(pcCliOutputString, "%s\r * %s ->", pcNewLineString, pcMenuNameString);cli_print_info();}\
}

/* Private variables -----------------------------------------------------------*/
static OS_MEM xSerialSendCharsQueueMem;
static char   xSerialSendCharsQueue[cli_MAX_TX_QUEUE_DEPTH][cli_MAX_TX_QUEUE_LEN];

static OS_MEM xSerialRecvCharsQueueMem;
static char   xSerialRecvCharsQueue[cli_MAX_RX_QUEUE_DEPTH][cli_MAX_RX_QUEUE_LEN];

static const char * const pcWelcomeMessageString = "\r\n Command line tool server.\r\n Intput 'help' to press enter to view the help information.\r\n\r";
static const char * const pcNULLLineString = "                                                  ";//length: cli_MAX_RX_INPUT_SIZE
static const char * const pcNewLineString  = "\r\n";

const char * pcMenuNameString = NULL;
char * pcCliOutputString = NULL;

volatile CLI_DEBUG_PEINT_STATUS xCliDebugStatus;//CLI,SYS: default open;

/* Private function declaration ------------------------------------------------*/
/* Private functions -----------------------------------------------------------*/
/**
  * @brief  : Enter the command processing task.
  * @param  : None
  * @retval : None
  */
static void cliReceiveTask(void *p_arg)
{
    char *pcRxedCharPop, cRxedChar;
    OS_MSG_SIZE xRecvMsgSize;
    char cInputString[cli_MAX_RX_INPUT_SIZE], cLastInputString[cli_MAX_RX_INPUT_SIZE] = {"cd /"};
    uint8_t xInputStringIndex = 0;

    OS_ERR err;

    debugPrintInfo(SYS_isDebugOn, pcWelcomeMessageString);
    pcMenuNameString = pcCmdLineString;
    cliNewLineInfo;

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//open receive

	for( ;; ) {
        pcRxedCharPop = OSTaskQPend((OS_TICK)cli_MAX_RX_QUEUE_WAIT, /* received the character by usart isr. */
                        (OS_OPT)OS_OPT_PEND_BLOCKING,
                        (OS_MSG_SIZE *)&xRecvMsgSize,
                        (CPU_TS *)NULL,
                        (OS_ERR *)&err);

        if(err == OS_ERR_NONE) {
            cRxedChar = *pcRxedCharPop;
            /* Ensure exclusive access to the UART Tx. */
            if(cRxedChar == '\n' || cRxedChar == '\r') {

				/* See if the command is empty, indicating that the last command is to be executed again. */
				if(xInputStringIndex == 0) {

					/* Copy the last command back into the input string. */
					strcpy(cInputString, cLastInputString);
				}

                cli_interpret_command(cInputString, NULL, cli_MAX_TX_QUEUE_LEN);

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
        }

        OSMemPut((OS_MEM *)&xSerialRecvCharsQueueMem, (void * )pcRxedCharPop, (OS_ERR *)&err);
    }
}

/**
  * @brief  : Fetch the message from the queue to the serial port.
  * @param  : None
  * @retval : None
  */
static void cliSendTask(void *p_arg)
{
    OS_MSG_SIZE xSendMsgSize;
    unsigned char *cOutputQueuePop;
    OS_ERR err;

	for( ;; ) {
        cOutputQueuePop = (unsigned char *)OSTaskQPend((OS_TICK)cli_MAX_TX_QUEUE_WAIT,
                                           (OS_OPT)OS_OPT_PEND_BLOCKING,
                                           (OS_MSG_SIZE *)&xSendMsgSize,
                                           (CPU_TS *)NULL,
                                           (OS_ERR *)&err);

        if(err == OS_ERR_NONE) {
            serial_cli_print_info(cOutputQueuePop, xSendMsgSize);

            OSTaskSemPend((OS_TICK)cli_MAX_TX_BINARY_DMA_WAIT,
                          (OS_OPT)OS_OPT_PEND_BLOCKING,
                          (CPU_TS *)NULL,
                          (OS_ERR *)&err);/* Wait the DMA send over & Start next. */
        }

        OSMemPut((OS_MEM *)&xSerialSendCharsQueueMem, (void * )cOutputQueuePop, (OS_ERR *)&err);
    }
}

/**
  * @brief  : The serial port receives the character callback.
  * @param  : None
  * @retval : None
  */
static void cli_receive_char_callback(uint16_t ch)
{
    OSIntEnter();

    OS_ERR err;

    unsigned char *ptr = (unsigned char *)OSMemGet((OS_MEM *)&xSerialRecvCharsQueueMem, (OS_ERR *)&err);

    if(err == OS_ERR_NONE) {
        *ptr = (unsigned char)ch;
        OSTaskQPost((OS_TCB *)&cliReceiveTaskTCB,   /* Send the character. */
                    (void *)ptr,
                    (OS_MSG_SIZE)1,
                    (OS_OPT)OS_OPT_POST_FIFO,
                    (OS_ERR *)&err);
    }
    else {
        OSMemPut((OS_MEM *)&xSerialRecvCharsQueueMem, (void * )ptr, (OS_ERR *)&err);
    }

    OSIntExit();
}

/**
  * @brief  : The serial port output completes the callback.
  * @param  : None
  * @retval : None
  */
static void cli_send_frame_over_callback(void)
{
    OSIntEnter();

    OS_ERR err;

    OSTaskSemPost((OS_TCB *)&cliSendTaskTCB,    /* DMA send complete. */
                  (OS_OPT)OS_OPT_POST_NONE,
                  (OS_ERR *)&err);

    OSIntExit();
}

/**
  * @brief  : Allocate memory for strings that need to be printed.
  * @param  : None
  * @retval : None
  */
unsigned int cli_print_malloc(void)
{
    OS_ERR err;
    char *ptr = (char *)OSMemGet((OS_MEM *)&xSerialSendCharsQueueMem, (OS_ERR *)&err);

    /* Gets the free memory area in the queue. */
    if(err == OS_ERR_NONE) { pcCliOutputString = ptr; return TRUE; }
    else OSMemPut((OS_MEM *)&xSerialSendCharsQueueMem, (void * )ptr, (OS_ERR *)&err);

    return FALSE;
}

/**
  * @brief  : Adds an output message to the queue.
  * @param  : None
  * @retval : None
  */
void cli_print_info(void)
{
    OS_ERR err;

    OSTaskQPost((OS_TCB *)&cliSendTaskTCB,              /* Cache into the queue. */
                (void *)pcCliOutputString,
                (OS_MSG_SIZE)strlen(pcCliOutputString),
                (OS_OPT)OS_OPT_POST_FIFO,
                (OS_ERR *)&err);
}

/**
  * @brief  : Create commond line receive & send task
  * @param  : None
  * @retval : None
  */
void cli_task_create(void)
{
    OS_ERR err;

    OSMemCreate((OS_MEM *)&xSerialSendCharsQueueMem,
                (CPU_CHAR *)"xSerialSendCharsQueue",
                (void *)xSerialSendCharsQueue,
                (OS_MEM_QTY)cli_MAX_TX_QUEUE_DEPTH,
                (OS_MEM_SIZE)cli_MAX_TX_QUEUE_LEN,
                (OS_ERR *)&err);

    OSMemCreate((OS_MEM *)&xSerialRecvCharsQueueMem,
                (CPU_CHAR *)"xSerialRecvCharsQueue",
                (void *)xSerialRecvCharsQueue,
                (OS_MEM_QTY)cli_MAX_RX_QUEUE_DEPTH,
                (OS_MEM_SIZE)cli_MAX_RX_QUEUE_LEN,
                (OS_ERR *)&err);

    serial_cli_init(115200, cli_receive_char_callback);
    serial_cli_printInfo_init(cli_send_frame_over_callback);

    xCliDebugStatus.cliDebugStatus = cli_DEBUG_DEFAULT_STATUS;

    OSTaskCreate((OS_TCB       *)&cliSendTaskTCB,              /* Create the task */
                 (CPU_CHAR     *)"cliSendTask",
                 (OS_TASK_PTR   )cliSendTask,
                 (void         *)0u,
                 (OS_PRIO       )TASK_CFG_CLI_SEND_TASK_PRIO,
                 (CPU_STK      *)&cliSendTaskStk[0u],
                 (CPU_STK_SIZE  )cliSendTaskStk[TASK_CFG_CLI_SEND_TASK_STK_SIZE / 10u],
                 (CPU_STK_SIZE  )TASK_CFG_CLI_SEND_TASK_STK_SIZE,
                 (OS_MSG_QTY    )cli_MAX_TX_QUEUE_DEPTH,
                 (OS_TICK       )0u,
                 (void         *)0u,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);

    OSTaskCreate((OS_TCB       *)&cliReceiveTaskTCB,          /* Create the task */
                 (CPU_CHAR     *)"cliReceiveTask",
                 (OS_TASK_PTR   )cliReceiveTask,
                 (void         *)0u,
                 (OS_PRIO       )TASK_CFG_CLI_RECEIVE_TASK_PRIO,
                 (CPU_STK      *)&cliReceiveTaskStk[0u],
                 (CPU_STK_SIZE  )cliReceiveTaskStk[TASK_CFG_CLI_RECEIVE_TASK_STK_SIZE / 10u],
                 (CPU_STK_SIZE  )TASK_CFG_CLI_RECEIVE_TASK_STK_SIZE,
                 (OS_MSG_QTY    )cli_MAX_RX_QUEUE_DEPTH,
                 (OS_TICK       )0u,
                 (void         *)0u,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);
}

/**
  * @}
  */

/************************** Coopyright (C) Jungleeee 2018 *******END OF FILE*******/

