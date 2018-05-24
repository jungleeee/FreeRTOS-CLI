/**
  ********************************************************************************
  * @File Name    : network.c
  * @Author       : Jungle
  * @Mail         : Mail
  * @Created Time : 2018/4/19 9:41:27
  * @Version      : V1.0
  * @Last Changed : 2018/4/19 9:41:27
  * @Brief        : brief
  ********************************************************************************
  */

/* Inlcude ---------------------------------------------------------------------*/
#include "stdio.h"
#include "stdint.h"
#include "string.h"

#include "tcpip.h"
#include "ethernetif.h"
#include "stm32f4x7_eth_bsp.h"

#include <lwip/sockets.h>
#include <lwip/err.h>
#include <lwip/sys.h>

/** @addtogroup Cli_Project
  * @{
  */

/* Private typedef -------------------------------------------------------------*/
/* Private constants define ----------------------------------------------------*/
/* Static IP ADDRESS */
#define IP_ADDR0        192
#define IP_ADDR1        168
#define IP_ADDR2        11
#define IP_ADDR3        241

/* NETMASK */
#define NETMASK_ADDR0   255
#define NETMASK_ADDR1   255
#define NETMASK_ADDR2   255
#define NETMASK_ADDR3   0

/* Gateway Address */
#define GW_ADDR0        192
#define GW_ADDR1        168
#define GW_ADDR2        11
#define GW_ADDR3        1

/* Private macro define --------------------------------------------------------*/
/* Private variables -----------------------------------------------------------*/
struct netif xnetif;

/* Private function declaration ------------------------------------------------*/
/* Private functions -----------------------------------------------------------*/
/**
  * @brief  Initializes the lwIP stack
  * @param  None
  * @retval None
  */
void network_lwip_init(void)
{
    struct ip4_addr ipaddr;
    struct ip4_addr netmask;
    struct ip4_addr gw;

#ifndef USE_DHCP
//    uint8_t iptab[4] = {0};
//    uint8_t iptxt[20];
#endif

    /* Create tcp_ip stack thread */
    tcpip_init( NULL, NULL );

    /* IP address setting */
#ifdef USE_DHCP
    ipaddr.addr = 0;
    netmask.addr = 0;
    gw.addr = 0;
#else
    IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
    IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1 , NETMASK_ADDR2, NETMASK_ADDR3);
    IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
#endif

    /* - netif_add(struct netif *netif, struct ip_addr *ipaddr,
    struct ip_addr *netmask, struct ip_addr *gw,
    void *state, err_t (* init)(struct netif *netif),
    err_t (* input)(struct pbuf *p, struct netif *netif))

    Adds your network interface to the netif_list. Allocate a struct
    netif and pass a pointer to this structure as the first argument.
    Give pointers to cleared ip_addr structures when using DHCP,
    or fill them with sane numbers otherwise. The state pointer may be NULL.

    The init function pointer must point to a initialization function for
    your ethernet netif interface. The following code illustrates it's use.*/
    netif_add(&xnetif, &ipaddr, &netmask, &gw, NULL, ethernetif_init, tcpip_input);

    /*  Registers the default network interface.*/
    netif_set_default(&xnetif);

    /* When the netif is fully configured this function must be called.*/
    netif_set_up(&xnetif);

#if 0
    if (EthStatus == (ETH_INIT_FLAG | ETH_LINK_FLAG)) {
        /* Set Ethernet link flag */
        xnetif.flags |= NETIF_FLAG_LINK_UP;

        /* When the netif is fully configured this function must be called.*/
        netif_set_up(&xnetif);
#ifdef USE_DHCP
        DHCP_state = DHCP_START;
#endif /* USE_DHCP */
    }
    else {
        /*  When the netif link is down this function must be called.*/
        netif_set_down(&xnetif);
#ifdef USE_DHCP
        DHCP_state = DHCP_LINK_DOWN;
#endif /* USE_DHCP */
    }

    /* Set the link callback function, this function is called on change of link status*/
    netif_set_link_callback(&xnetif, ETH_link_callback);
#endif
}

/**
  * @brief  :
  * @param  : None
  * @retval : None
  */
void network_init(void)
{
    /* configure ethernet (GPIOs, clocks, MAC, DMA) */
    ETH_BSP_Config();

    network_lwip_init();
}

/**
  * @}
  */

/************************** Coopyright (C) Jungleeee 2018 *******END OF FILE*******/

