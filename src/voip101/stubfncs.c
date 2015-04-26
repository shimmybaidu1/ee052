/****************************************************************************/
/*                                                                          */
/*                                STUBFNCS                                  */
/*                              Stub Functions                              */
/*                          VoIP Telephone Project                          */
/*                                EE/CS 52                                  */
/*                                                                          */
/****************************************************************************/

/*
   This file contains stub functions for the hardware interfacing code.  The
   file is meant to allow linking of the main code without necessarily having
   all the low-level functions.  The functions included are:
      call_halt           - halt audio capture and playing for a call
      call_start          - start audio capture for a call
      display_IP          - display the passed IP number
      display_memory_addr - display the passed memory location number
      display_status      - display the passed status
      getkey              - get a key
      key_available       - check if a key is available
      update_rx           - check if ready for a receive (recording) update
      update_tx           - check if ready for a transmit (playing) update
      elapsed_time        - get the time since the last call to this function
      ether_init          - initialize the ethernet interface
      ether_transmit      - transmit a packet over the ethernet interface
      ether_rx_available  - whether a received packet is available
      ether_receive       - receives a packet from the ethernet interface

   The local functions included are:
      none

   The locally global variable definitions included are:
      none


   Revision History
      6/3/06   Glen George       Initial revision.
      6/5/06   Glen George       Removed the "far" keyword from the pointers
	                         in the function definitions.
      6/9/08   Glen George       Added new timing and ethernet functions.
      2/28/11  Glen George       Updated call_start, update_rx, and update_tx
	                         to match the new specification.
*/



/* library include files */
    /* none */

/* local include files */
#include  "voipdefs.h"




/* update functions */

unsigned char  update_rx(short int *p)
{
    return  FALSE;
}

unsigned char  update_tx(short int *p)
{
    return  FALSE;
}



/* keypad functions */

unsigned char  key_available()
{
    return  FALSE;
}

int  getkey()
{
    return  KEY_ILLEGAL;
}



/* display functions  */

void  display_IP(unsigned long int ip)
{
    return;
}

void  display_memory_addr(unsigned int a)
{
    return;
}

void  display_status(unsigned int s)
{
    return;
}



/* audio functions */

void  call_start(short int *p)
{
    return;
}

void  call_halt()
{
    return;
}



/* timing function */

int  elapsed_time()
{
    return  0;
}



/* networking functions */

char  ether_init()
{
    return  TRUE;
}

char  ether_transmit(struct pbuf *p)
{
    return  TRUE;
}

char  ether_rx_available()
{
    return  FALSE;
}

struct pbuf  *ether_receive()
{
    return  NULL;
}
