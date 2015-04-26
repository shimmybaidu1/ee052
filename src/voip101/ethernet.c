/****************************************************************************/
/*                                                                          */
/*                                 ETHERNET                                 */
/*                       Ethernet Interface Functions                       */
/*                          VoIP Telephone Project                          */
/*                                 EE/CS 52                                 */
/*                                                                          */
/****************************************************************************/

/*
   This file contains the ethernet interface low-level driver functions for
   the VoIP Telephone Project.  These functions are used to interface the
   VoIP code with the lwIP code.  This file should only be compiled into the
   project if lwIP is being used.  The functions included are:
      ethernetif_init   - initialize the ethernet interface
      ethernetif_input  - get input from the ethernet interface
      ethernetif_output - send output to the ethernet interface
      init_networking   - initialize the ethernet interface and library

   The local functions included are:
      none

   The global variable definitions included are:
      et0 - the network interface to use for the phone


   Revision History
      6/11/09  Glen George       Initial revision (based on lwIP ethernetif.c
                                 file).
      6/13/09  Glen George       Fixed polarity problems with return values
                                 for ether_init and ether_transmit.
      3/8/11   Glen George       Get the MAC address from interfac.h instead
                                 of hard coding it.
*/



/* library include files */
#include  "lwip/opt.h"
#include  "lwip/ip_addr.h"
#include  "lwip/mem.h"
#include  "lwip/memp.h"
#include  "lwip/pbuf.h"
#include  "lwip/netif.h"
#include  "netif/etharp.h"
#include  "lwip/stats.h"
#include  "ipv4/lwip/ip.h"
#include  "lwip/err.h"

/* local include files */
#include  "voipdefs.h"
#include  "ethernet.h"
#include  "interfac.h"




/* locally global variables */
static struct netif  et0;	/* the network interface */




/*
   init_ethernet

   Description:      This function takes care of all of the initialization for
                     the ethernet interface.  It first sets up the lwIP code.
                     Then it initializes the ethernet hardware.

   Operation:        The lwIP library is initialized.  Then the network
                     interface is initialized with default addresses.  And
		     finally the hardware is setup.

   Arguments:        None.
   Return Value:     None.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: et0 - the network interface for the system.

   Author:           Glen George
   Last Modified:    June 9, 2009

*/

void  init_networking()
{
    /* variables */
    struct ip_addr  gateway;		/* default gateway */
    struct ip_addr  netmask;		/* default network mask */
    struct ip_addr  default_addr;	/* default IP address */



    /* setup the lwIP library */
    mem_init();
    memp_init();
    pbuf_init();
    netif_init();
    etharp_init();
    stats_init();


    /* setup the default gateway, net mask, and IP address */
    IP4_ADDR(&gateway, 192, 168, 1, 1);
    IP4_ADDR(&netmask, 255, 255, 255, 0);
    IP4_ADDR(&default_addr, 192, 168, 1, 21);


    /* tell lwIP about the network interface */
    netif_add(&et0, &default_addr, &netmask, &gateway, NULL, ethernetif_init, ip_input);

    /* set et0 as the default interface and set up the interface */
    netif_set_default(&et0);
    netif_set_up(&et0);


    /* done initializing the ethernet interface, return */
    return;

}




/*
   ethernetif_init

   Description:      This function handles initialization of the ethernet
                     interface itself.  It is meant to be passed as a
		     parameter to netif_add().

   Operation:        The passed network interface is initialized and then the
                     hardware is initialized.

   Arguments:        etx (struct netif *) - pointer to the network interface
                                            whose hardware is to be
					    initialized.
   Return Value:     (err_t) - ERR_OK if the interface is initialized, or
                        ERR_IF if there is an error initializing the hardware.

   Input:            None.
   Output:           None.

   Error Handling:   If there is an error initializing the hardware, ERR_IF is
                     returned.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: None.

   Author:           Glen George
   Last Modified:    March 8, 2011

*/

err_t  ethernetif_init(struct netif *etx)
{
    /* variables */
    char  err;		/* whether or not there is an error */



    /* make sure the argument is reasonable */
    LWIP_ASSERT("etx != NULL", (etx != NULL));


    /* setup the interface host name */
    #if LWIP_NETIF_HOSTNAME
        etx->hostname = "lwip";
    #endif

    /* no state */
    etx->state = NULL;

    /* set the interface name */
    etx->name[0] = 'e';
    etx->name[1] = 't';

    /* use etharp_output() to do output (saves a function call) */
    etx->output = etharp_output;
    etx->linkoutput = ethernetif_output;


    /* set MAC hardware address length */
    etx->hwaddr_len = ETHARP_HWADDR_LEN;

    /* and set MAC hardware address itself */
    etx->hwaddr[0] = (MAC_ADDR_L         & 0xFF);
    etx->hwaddr[1] = ((MAC_ADDR_L >> 8)  & 0xFF);
    etx->hwaddr[2] = ((MAC_ADDR_L >> 16) & 0xFF);
    etx->hwaddr[3] = ((MAC_ADDR_L >> 24) & 0xFF);
    etx->hwaddr[4] = (MAC_ADDR_H        & 0xFF);
    etx->hwaddr[5] = ((MAC_ADDR_H >> 8) & 0xFF);


    /* set the maximum transfer unit */
    etx->mtu = 1500;
  
    /* device capabilities */
    /* can broadcast, can do ARP, and there is an active link */
    etx->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP |
                 NETIF_FLAG_LINK_UP;
  
  
    /* initialize the hardware */
    err = !ether_init();


    /* return whether or not there was an error */
    if (err)
        /* had an error - return ERR_IF */
        return ERR_IF;
    else
        /* no error - return ERR_OK */
        return ERR_OK;

}




/*
   ethernetif_input

   Description:      This function gets input from the passed network
                     interface and processes it.  Only ARP and IP packets are
		     processed.  For efficiency it should only be called when
                     packets are available.

   Operation:        The passed network interface is initialized and then the
                     hardware is initialized.

   Arguments:        None.
   Return Value:     None.

   Input:            None.
   Output:           None.

   Error Handling:   If there is no packet available, the function returns.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: None.

   Author:           Glen George
   Last Modified:    June 9, 2009

*/

void  ethernetif_input()
{
    /* variables */
    struct pbuf     *p;		/* pointer to a received packet */

    struct eth_hdr  *ethhdr;	/* pointer to the packet header */


    /* get the received packet (may be a buffer chain) */ 
    p = ether_receive();


    /* only process the packet if there was one */
    if (p != NULL)  {

        /* have a packet - figure out what it is from the header */
        ethhdr = p->payload;

	/* what kind of packet was this */
        switch (htons(ethhdr->type))  {

            case ETHTYPE_IP:	/* a standard IP packet */
	    			/* update the ARP table if we can */
				etharp_ip_input(&et0, p);
				/* remove the ethernet header (to get to IP) */
				pbuf_header(p, -((s16_t) sizeof(struct eth_hdr)));
				/* now pass the packet to IP processing which will free it */
				ip_input(p, &et0);
	    			break;

            case ETHTYPE_ARP:	/* an ARP packet - process it */
                                etharp_arp_input(&et0, (struct eth_addr *) &(et0.hwaddr), p);
				/* buffer is freed by etharp_arp_input() */
	    			break;

	    default:		/* some other kind of packet */
	    			/* ignore it - just free the memory */
				pbuf_free(p);
				p = NULL;	/* no packet */
				break;
        }
    }


    /* done processing the input packet (if there was one) - return */
    return;

}




/*
   ethernetif_output

   Description:      This function outputs the passed packet over the ethernet
                     interface and then frees the memory for the packet.  Note
                     that the passed packet could be in a pbuf chain, not in a
		     single pbuf.

   Operation:        The passed packet is output via the low-level output
                     function and then the packet is freed.

   Arguments:        etx (struct netif *) - pointer to the network interface
                                            over which the packet is to be
					    output (ignored).
                     b (struct pbuf *)    - pointer to the packet (chain) to
                                            output.
   Return Value:     (err_t) - ERR_OK if the packet is successfully output, or
                        ERR_IF if there is an error outputing the packet.

   Input:            None.
   Output:           None.

   Error Handling:   If there is an error outputing the packet, ERR_IF is
                     returned and the passed buffer is still freed.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: None.

   Author:           Glen George
   Last Modified:    June 9, 2009

*/

err_t  ethernetif_output(struct netif *etx, struct pbuf *b)
{
    /* variables */
    char  err;		/* whether or not there was an error */



    /* send the packet watching for an error */
    err = !ether_transmit(b);

    /* error or not, free the packet (if there was one) */
    if (b != NULL)
        pbuf_free(b);


    /* return whether or not there was an error */
    if (err)
        /* had an error - return ERR_IF */
        return ERR_IF;
    else
        /* no error - return ERR_OK */
        return ERR_OK;

}
