/****************************************************************************/
/*                                                                          */
/*                                BUFFERS.H                                 */
/*                           General Definitions                            */
/*                               Include File                               */
/*                          VoIP Telphone Project                           */
/*                                 EE/CS 52                                 */
/*                                                                          */
/****************************************************************************/

/*
   This file contains the constant and structure definitions and function
   declarations for the buffer management functions for the VoIP Telephone
   defined in buffers.c.


   Revision History:
      6/6/06   Glen George       Initial revision.
      5/26/08  Glen George       Updated return types of buffer functions.
      2/28/11  Glen George       Changed buffers to be short ints (16-bits)
	                         instead of unsigned chars (8-bits).
      3/9/11   Glen George       Added prototypes for get_xmit_next_ptr() and
	                         get_rcv_next_ptr().
*/



#ifndef  I__BUFFERS_H__
    #define  I__BUFFERS_H__


/* library include files */
  /* none */

/* local include files */
#include  "interfac.h"




/* constants */

/* number of transmit and receive buffers */
#define  NUM_RX_BUFFERS  20	/* must be at least 4, more is better */
#define  NUM_TX_BUFFERS  20	/* must be at least 4, more is better */




/* structures, unions, and typedefs */
    /* none */




/* function declarations */

/* initialization functions */
void  init_buffers(void);	/* initialize the buffer system */
void  reset_rx_buffer(void);	/* reset the receive buffer to empty */
void  reset_tx_buffer(void);	/* reset the transmit buffer to empty */

/* status functions */
int  rx_available(void);	/* there is a buffer available for mic data */
int  xmit_available(void);	/* there is a buffer ready to send over ethernet */
int  tx_available(void);	/* there is a buffer available to play */
int  rcv_available(void);	/* there is a buffer ready to fill from ethernet */

/* blocking functions for getting buffers */
short int  *get_rx_buffer(void);	/* get a buffer to fill with mic data */
short int  *get_rx_next_ptr(void);	/* get the pointer to next buffer w/o allocating it */
short int  *get_xmit_buffer(void);	/* get a buffer to send over ethernet */
short int  *get_xmit_next_ptr(void);	/* get the pointer to next ethernet buffer w/o allocating it */
short int  *get_tx_buffer(void);	/* get a buffer to send to the speaker */
short int  *get_tx_next_ptr(void);	/* get the pointer to next buffer w/o allocating it */
short int  *get_rcv_buffer(void);	/* get a buffer to fill with ethernet data */
short int  *get_rcv_next_ptr(void);	/* get the buffer to fill with ethernet w/o allocating it */


#endif
