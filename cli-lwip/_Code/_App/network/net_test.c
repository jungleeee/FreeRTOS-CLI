/**
  ********************************************************************************
  * @File Name    : net_test.c
  * @Author       : Jungle
  * @Mail         : Mail
  * @Created Time : 2018/4/20 15:45:50
  * @Version      : V1.0
  * @Last Changed : 2018/5/23 9:42:17
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

/** @addtogroup Cli_Project
  * @{
  */

/* Private typedef -------------------------------------------------------------*/
/* Private constants define ----------------------------------------------------*/
/* Private macro define --------------------------------------------------------*/
/* Private variables -----------------------------------------------------------*/
static uint8_t data_buffer[200]= {0};

/* Private function declaration ------------------------------------------------*/
/* Private functions -----------------------------------------------------------*/
/**
  * @brief  :
  * @param  : None
  * @retval : None
  */
static void tcp_server_thread(void *p_arg)
{
    struct sockaddr_in server_addr;
    struct sockaddr_in conn_addr;
    int sock_fd;            /* server socked */
    int sock_conn;          /* request socked */
    socklen_t addr_len;
    int err;
    int length;

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        printf("failed to create sock_fd!\n");
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr =htonl(INADDR_ANY);
    server_addr.sin_port = htons(8888);

    err = bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (err < 0) {
        printf("bind failed!\n");
    }

    err = listen(sock_fd, 3);
    if (err < 0) {
        printf("bind failed!\n");
    }

    addr_len = sizeof(struct sockaddr_in);

    printf("before accept!\n");
    sock_conn = accept(sock_fd, (struct sockaddr *)&conn_addr, &addr_len);
    printf("after accept!\n");

    while (1) {
        memset(data_buffer, 0, sizeof(data_buffer));

        length = recv(sock_conn, (unsigned int *)data_buffer, 20, 0);

        printf("length received %d\r\n", length);
        printf("received string: %s\r\n", data_buffer);

        //send(sock_conn, "good", 5, 0);
    }
}

void tcp_task_create(void)
{
    sys_thread_new("tcp_server_thread",  tcp_server_thread, NULL, 256, 5);
}

/**
  * @}
  */

/************************** Coopyright (C) Jungleeee 2018 *******END OF FILE*******/

