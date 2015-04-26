/****************************************************************************/
/*                                                                          */
/*                                ETHERNET.H                                */
/*                       Ethernet Interface Functions                       */
/*                               Include File                               */
/*                          VoIP Telephone Project                          */
/*                                 EE/CS 52                                 */
/*                                                                          */
/****************************************************************************/

/*
   This file contains the constant and structure definitions and the function
   declarations for the ethernet interface low-level driver functions for the
   VoIP Telephone Project.


   Revision History:
      6/11/09  Glen George       Initial revision.
      6/12/09  Glen George       Fixed directory for netif.h.
      3/10/11  Glen George       Fixed value of TCP_TIMEOUT.
*/




/* make sure the file isn't already included */
#ifndef  I__ETHERNET_H__
    #define  I__ETHERNET_H__




/* library include files */
#include  "lwip/pbuf.h"
#include  "lwip/netif.h"
#include  "lwip/err.h"

/* local include files */
    /* none */




/* constants */

#define  TCP_TIMEOUT   250	/* TCP timeout in ms */




/* structures, unions, and typedefs */
    /* none */




/* function declarations */

void   ethernetif_input(void);		/* driver input */
err_t  ethernetif_init(struct netif *);	/* driver initialization */
err_t  ethernetif_output(struct netif *, struct pbuf *);  /* driver output */
void   init_networking(void);		/* networking system initialization */


#endif
