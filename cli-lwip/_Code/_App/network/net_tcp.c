/**
  ********************************************************************************
  * @File Name    : net_tcp.c
  * @Author       : Jungle
  * @Mail         : Mail
  * @Created Time : 2018/5/16 11:00:38
  * @Version      : V1.0
  * @Last Changed : 2018/5/16 11:00:38
  * @Brief        : brief
  ********************************************************************************
  */

/* Inlcude ---------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"

#include "sockets.h"
#include <lwip/sys.h>

#include "stdint.h"
#include "stdio.h"
#include "string.h"

#include <common.h>
#include <cli_common.h>
#include <network.h>

/** @addtogroup Cli_Project
  * @{
  */

/* Private function declaration ------------------------------------------------*/
/* Private typedef -------------------------------------------------------------*/
typedef int (* pfTCP_SOCKET_HANDLE_CALLBACK)(void *, int);

typedef enum {
    eCliServer = 0,
    eTcpServerNumber
} eTcpServerList;

typedef union sCLIENT_STATUS {
    struct {
        unsigned int xSockfdClose       :1;//LSB
        unsigned int xOTHER             :31;
    }m;
    unsigned int xBits;
} Client_Status_Definition_u;

typedef struct sTCP_CLIENT_DATAS {
    int xSockfd;
    int xIdNumber;

    char cIpAddress[16];//TCP_CLIENT_IP_ADDRESS_LENGTH

    uint32_t xTimestamp;

    xSemaphoreHandle xMutex;

    Client_Status_Definition_u  xStatus;

    pfTCP_SOCKET_HANDLE_CALLBACK pfNewsHandle;
    pfTCP_SOCKET_HANDLE_CALLBACK pfCloseHandle;
} Tcp_Client_Definition_t;

typedef struct sTCP_SERVER_CONTROL {
    const char * const cServerName;
    const char * const cReceiveTaskName;
    const char * const cSendTaskName;

	xQueueHandle xSendQueue;
    const int xSendQueueSize;
    const int xSendQueueLength;

    const int xPortNumber;
    int xPortStatus;

    int xSockfd;
    int xMaxSockfd;

    int xLinkedClientCount;

    Tcp_Client_Definition_t sClient[3];//TCP_CLIENT_MAX_NUMBERS
} Tcp_Server_Definition_t;

/* Private constants define ----------------------------------------------------*/
#define TCP_CLIENT_IP_ADDRESS_LENGTH        16
#define TCP_CLIENT_MAX_NUMBERS              3

#define TCP_SERVER_CLI_PORT_NUMBER          2001
#define TCP_SERVER_CLI_INPUT_BUFFER_SIZE    50

/* Private macro define --------------------------------------------------------*/
#define NET_IS_ON_DEBUG                     xCliDebugStatus.Bits.NET

#define TCP_SOCKET_HANDLE_CALLBACK_NULL     (pfTCP_SOCKET_HANDLE_CALLBACK)NULL

#define TCP_CLIENT_DEFINITION_T_DEFAULT     {\
                                                {-1, -1, {0}, 0, NULL, {0}, TCP_SOCKET_HANDLE_CALLBACK_NULL, TCP_SOCKET_HANDLE_CALLBACK_NULL},\
                                                {-1, -1, {0}, 0, NULL, {0}, TCP_SOCKET_HANDLE_CALLBACK_NULL, TCP_SOCKET_HANDLE_CALLBACK_NULL},\
                                                {-1, -1, {0}, 0, NULL, {0}, TCP_SOCKET_HANDLE_CALLBACK_NULL, TCP_SOCKET_HANDLE_CALLBACK_NULL}\
                                            }

/* Private variables -----------------------------------------------------------*/
extern xQueueHandle xClilRecvCharsQueue;

static char cCliServerInputBuffer[TCP_SERVER_CLI_INPUT_BUFFER_SIZE] = {0};

static Tcp_Server_Definition_t xRegisteredTcpServer[eTcpServerNumber] =
{   /* ServerName, ReceiveTaskName,  SendTaskName,     Queue, Size, Length Netwok Port,                Status, Sockfd, MaxSockfd, ClientCount, Client Struct */
    {"CliServer",  "NetCliRecvTask", "NetCliSendTask", NULL,  200,  20,   TCP_SERVER_CLI_PORT_NUMBER, 0,      0,      0,         0,           TCP_CLIENT_DEFINITION_T_DEFAULT},
};

//static Tcp_Server_Definition_t *psTcpServer = xRegisteredTcpServer;

/* Private functions -----------------------------------------------------------*/
/**
  * @brief  :
  * @param  : None
  * @retval : None
  */
static int tcp_news_receive(int xClientSockfd, char *pcBuffer, int xSize)
{
	int xReceivedSize = 0;

	xReceivedSize = recv(xClientSockfd, pcBuffer, xSize, MSG_MORE);

	return xReceivedSize;
}

/**
  * @brief  :
  * @param  : None
  * @retval : None
  */
static int tcp_news_send(int xClientSockfd, char *pcBuffer, int xSize)
{
	int size = -1, sentSize = 0;

	while(sentSize < xSize)
	{
		size = send(xClientSockfd, (pcBuffer + sentSize), (xSize - sentSize), 0);

		if(size < 1) return size;
		sentSize += size;
	}

	return sentSize;
}

/**
  * @brief  :
  * @param  : None
  * @retval : None
  */
static int tcp_client_close_handle(void *p, int id)
{
    Tcp_Server_Definition_t *psTcpServer = (Tcp_Server_Definition_t *)p;

    if((id < 0) || (id >= TCP_CLIENT_MAX_NUMBERS)) {

        return -1;
    }

    shutdown(psTcpServer->sClient[id].xSockfd, SHUT_RDWR);
    closesocket(psTcpServer->sClient[id].xSockfd);

    psTcpServer->sClient[id].xSockfd = -1;
    psTcpServer->sClient[id].xIdNumber = -1;
    memset(psTcpServer->sClient[id].cIpAddress, 0, TCP_CLIENT_IP_ADDRESS_LENGTH);
    memset(&psTcpServer->sClient[id].xStatus, 0, sizeof(int));

    psTcpServer->xLinkedClientCount--;
    psTcpServer->xMaxSockfd = -1;

    for(int i = 0;i < TCP_CLIENT_MAX_NUMBERS;i++) {

        if(psTcpServer->sClient[i].xSockfd != -1) {

            psTcpServer->xMaxSockfd = max(psTcpServer->xMaxSockfd, psTcpServer->sClient[i].xSockfd);
        }
    }

    return 0;
}

/**
  * @brief  :
  * @param  : None
  * @retval : None
  */
static int tcp_client_add_handle(Tcp_Server_Definition_t *psTcpServer, int xSockfd, char *cClientAddr, pfTCP_SOCKET_HANDLE_CALLBACK pfHandle)
{
    if(psTcpServer->xLinkedClientCount >= TCP_CLIENT_MAX_NUMBERS) {

        shutdown(xSockfd, SHUT_RDWR);
        closesocket(xSockfd);

        return -2;
    }

    for(int j = 0; j < TCP_CLIENT_MAX_NUMBERS; j++) {

        if(psTcpServer->sClient[j].xSockfd == -1) {

            psTcpServer->sClient[j].xSockfd = xSockfd;
            psTcpServer->sClient[j].xIdNumber = j;
            psTcpServer->sClient[j].xTimestamp = xTaskGetTickCount();
            psTcpServer->sClient[j].pfNewsHandle = pfHandle;
            psTcpServer->sClient[j].pfCloseHandle = tcp_client_close_handle;
            memcpy(psTcpServer->sClient[j].cIpAddress, cClientAddr, TCP_CLIENT_IP_ADDRESS_LENGTH);

            psTcpServer->xLinkedClientCount++;
            psTcpServer->xMaxSockfd = max(psTcpServer->xMaxSockfd, xSockfd);

            DEBUG_PRINT_INFO(NET_IS_ON_DEBUG, " * %s Add Client (Number: %d), IP %s. \r\n", psTcpServer->cServerName, j, psTcpServer->sClient[j].cIpAddress);

            return 0;
        }
    }

    shutdown(xSockfd, SHUT_RDWR);
    closesocket(xSockfd);

    return -1;
}

/**
  * @brief  :
  * @param  : None
  * @retval : None
  */
static int tcp_client_news_handle(void *p, int id)
{
    Tcp_Server_Definition_t *psTcpServer = (Tcp_Server_Definition_t *)p;
    int ret, xLength = 0;

    if(psTcpServer->xPortNumber == TCP_SERVER_CLI_PORT_NUMBER) {

        ret = tcp_news_receive(psTcpServer->sClient[id].xSockfd, cCliServerInputBuffer, TCP_SERVER_CLI_INPUT_BUFFER_SIZE);
        if(ret < 0) {

            psTcpServer->sClient[id].xStatus.m.xSockfdClose = TRUE;
            return -1;
        }

        xLength = ret;
        DEBUG_PRINT_INFO(NET_IS_ON_DEBUG, " * Cli Server Received : %s. Length %d .\r\n", cCliServerInputBuffer, xLength);
        for(int i = 0;i < xLength - 1;i++) {
            xQueueSendToBack(xClilRecvCharsQueue, &cCliServerInputBuffer[i], 100);
        }
    }
    else {


    }

    return 0;
}

/**
  * @brief  :
  * @param  : None
  * @retval : None
  */
static int tcp_client_connect_handle(void *p, int id)
{
    Tcp_Server_Definition_t *psTcpServer = (Tcp_Server_Definition_t *)p;

    int xClientSockfd;
    int optval;

    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    xClientSockfd = accept(psTcpServer->sClient[id].xSockfd, (struct sockaddr*)&addr, &addr_len);
    if(xClientSockfd < 0) {

        DEBUG_PRINT_INFO(NET_IS_ON_DEBUG, " * Server Accept Failed. %s Accept Error. \r\n", psTcpServer->cServerName);
        return -1;
    }
    else {

        optval = 1;
        setsockopt(xClientSockfd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
        optval = 5000;
        setsockopt(xClientSockfd, SOL_SOCKET, SO_SNDTIMEO, &optval, sizeof(optval));
        setsockopt(xClientSockfd, SOL_SOCKET, SO_RCVTIMEO, &optval, sizeof(optval));

        return tcp_client_add_handle(psTcpServer, xClientSockfd, inet_ntoa(addr.sin_addr), tcp_client_news_handle);
    }
}

/**
  * @brief  :
  * @param  : None
  * @retval : None
  */
static int tcp_server_connect_init(Tcp_Server_Definition_t *psTcpServer)
{
    int sockfd = -1;
    struct sockaddr_in addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        DEBUG_PRINT_INFO(SYS_IS_ON_DEBUG, " * Net Init Failed. %s Socket Error. \r\n", psTcpServer->cServerName);
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(psTcpServer->xPortNumber);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        DEBUG_PRINT_INFO(SYS_IS_ON_DEBUG, " * Net Init Failed. %s Bind Error. \r\n", psTcpServer->cServerName);
        return -1;
    }

    if(listen(sockfd, TCP_CLIENT_MAX_NUMBERS) < 0) {
        DEBUG_PRINT_INFO(SYS_IS_ON_DEBUG, " * Net Init Failed. %s Listen Error. \r\n", psTcpServer->cServerName);
        return -1;
    }

    psTcpServer->xSockfd = sockfd;

    if(tcp_client_add_handle(psTcpServer, sockfd, "127.0.0.1", tcp_client_connect_handle) != 0) {
        DEBUG_PRINT_INFO(SYS_IS_ON_DEBUG, " * Net Init Failed. %s Connect Init Error. \r\n", psTcpServer->cServerName);
        return -1;
    }

    return 0;
}

/**
  * @brief  :
  * @param  : None
  * @retval : None
  */
static void tcp_server_data_init(void)
{
    for(int i = 0; i < eTcpServerNumber; i++) {

        for(int j = 0; j < TCP_CLIENT_MAX_NUMBERS; j++) {

            xRegisteredTcpServer[i].sClient[j].xMutex = xSemaphoreCreateBinary();
            if(xRegisteredTcpServer[i].sClient[j].xMutex == NULL) {
                DEBUG_PRINT_INFO(SYS_IS_ON_DEBUG, " * Net Init Failed. %s Client(%d) Create Mutex Error. \r\n", xRegisteredTcpServer[i].cServerName, j);
            }
            xSemaphoreGive(xRegisteredTcpServer[i].sClient[j].xMutex);
        }

        xRegisteredTcpServer[i].xSendQueue = xQueueCreate(xRegisteredTcpServer[i].xSendQueueLength, xRegisteredTcpServer[i].xSendQueueSize);
        if(xRegisteredTcpServer[i].xSendQueue == NULL) {
            DEBUG_PRINT_INFO(SYS_IS_ON_DEBUG, " * Net Init Failed. %s Create Queue Error. \r\n", xRegisteredTcpServer[i].cServerName);
        }
    }
}

/**
  * @brief  :
  * @param  : None
  * @retval : None
  */
static void tcp_server_news_add(Tcp_Server_Definition_t *psTcpServer, char *xBuffer)
{
   xQueueSend(psTcpServer->xSendQueue, xBuffer, 1000);
}

/**
  * @brief  :
  * @param  : None
  * @retval : None
  */
static void tcp_server_receive_task(void *p)
{
    Tcp_Server_Definition_t *psTcpServer = (Tcp_Server_Definition_t *)p;

    static struct timeval timeout;
    static fd_set readfds;

    static int i = 0, ret = -1;

    for(;;)
    {
        FD_ZERO(&readfds);
        for(i = 0;i < TCP_CLIENT_MAX_NUMBERS;i++) {

            if(psTcpServer->sClient[i].xSockfd != -1) {

                FD_SET(psTcpServer->sClient[i].xSockfd, &readfds);
            }

        }

        timeout.tv_sec = 2;
        timeout.tv_usec = 0;
        ret = select(psTcpServer->xMaxSockfd + 1, &readfds, NULL, NULL, &timeout);
        if(!ret) {
            continue;
        }

        for(i = 0;i < TCP_CLIENT_MAX_NUMBERS;i++) {

            if(FD_ISSET(psTcpServer->sClient[i].xSockfd, &readfds)) {

                if(psTcpServer->sClient[i].pfNewsHandle != NULL) {

                    xSemaphoreTake(psTcpServer->sClient[i].xMutex, portMAX_DELAY);
                    DEBUG_PRINT_INFO(NET_IS_ON_DEBUG, " * %s Client(%d) Select Readfs. \r\n", psTcpServer->cServerName, i);
                    psTcpServer->sClient[i].pfNewsHandle(psTcpServer, i);
                    xSemaphoreGive(psTcpServer->sClient[i].xMutex);
                }

                if((psTcpServer->sClient[i].xStatus.m.xSockfdClose) && (psTcpServer->sClient[i].pfCloseHandle != NULL)) {
                    psTcpServer->sClient[i].pfCloseHandle(psTcpServer, i);
                }
            }
        }
    }
}

/**
  * @brief  :
  * @param  : None
  * @retval : None
  */
static void tcp_server_send_task(void *p)
{
    Tcp_Server_Definition_t *psTcpServer = (Tcp_Server_Definition_t *)p;

    static char xBuffer[200] = {0};
    static int i = 0, ret = -1;

    for(;;)
    {
        if(xQueueReceive(psTcpServer->xSendQueue, xBuffer, 1000) == pdTRUE) {

            for(i = 1;i < TCP_CLIENT_MAX_NUMBERS;i++) {

                if((psTcpServer->sClient[i].xSockfd != -1) && (psTcpServer->sClient[i].xStatus.m.xSockfdClose == FALSE)) {

                    xSemaphoreTake(psTcpServer->sClient[i].xMutex, portMAX_DELAY);
                    ret = tcp_news_send(psTcpServer->sClient[i].xSockfd, xBuffer, strlen(xBuffer));
                    xSemaphoreGive(psTcpServer->sClient[i].xMutex);

                    if(ret < 0) {

                        DEBUG_PRINT_INFO(NET_IS_ON_DEBUG, " * %s Client(%d) Send Error. \r\n", psTcpServer->cServerName, i);
                        psTcpServer->sClient[i].xStatus.m.xSockfdClose = TRUE;
                    }
                }

                if((psTcpServer->sClient[i].xStatus.m.xSockfdClose) && (psTcpServer->sClient[i].pfCloseHandle != NULL)) {
                    psTcpServer->sClient[i].pfCloseHandle(psTcpServer, i);
                }

            }

            memset(xBuffer, 0, 200);
        }
        else {


        }
    }
}

/**
  * @brief  :
  * @param  : None
  * @retval : None
  */
void tcp_task_create(void)
{
    network_init();

    tcp_server_data_init();

    for(int i = 0;i < eTcpServerNumber; i++) {

        if(!tcp_server_connect_init(&xRegisteredTcpServer[i])) {

            sys_thread_new(xRegisteredTcpServer[i].cReceiveTaskName, tcp_server_receive_task, &xRegisteredTcpServer[i], 256, 8);
            sys_thread_new(xRegisteredTcpServer[i].cSendTaskName,    tcp_server_send_task,    &xRegisteredTcpServer[i], 256, 8);
        }

    }
}

/**
  * @brief  :
  * @param  : None
  * @retval : None
  */
void tcp_cli_print(char *xBuffer)
{
   tcp_server_news_add(&xRegisteredTcpServer[eCliServer], xBuffer);
}

/**
  * @}
  */

/************************** Coopyright (C) Jungleeee 2018 *******END OF FILE*****/

