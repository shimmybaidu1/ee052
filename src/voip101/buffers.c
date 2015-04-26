/****************************************************************************/
/*                                                                          */
/*                                 BUFFERS                                  */
/*                       Buffer Management Functions                        */
/*                         VoIP Telephone Project                           */
/*                                EE/CS 52                                  */
/*                                                                          */
/****************************************************************************/

/*
   This file contains the buffer management functions for dealing with
   incoming and outging data for the VoIP Telephone Project.  The circular
   arrays of buffers are also maintained in this file.  The functions all
   assume the ethernet data is singly buffered and the microphone and speaker
   data is doubly buffered.  The functions included are:
      get_rcv_buffer    - get a buffer to fill with ethernet data
      get_rcv_next_ptr  - get next buffer for ethernet data w/o allocating it
      get_rx_buffer     - get a new buffer to fill with microphone data
      get_rx_next_ptr   - get pointer to next buffer to fill w/o allocating it
      get_tx_buffer     - get a buffer to send to the speaker
      get_tx_next_ptr   - get pointer to next buffer to play w/o freeing it
      get_xmit_buffer   - get a buffer to send over the ethernet
      get_xmit_next_ptr - get next ethernet buffer to send w/o freeing it
      init_buffers      - initialize the buffer functions
      rcv_available     - there is a buffer ready to fill from the ethernet
      reset_rx_buffer   - reset the receive buffers to empty
      reset_tx_buffer   - reset the transmit buffers to empty
      rx_available      - there is a buffer available for microphone data
      tx_available      - there is a buffer available to play
      xmit_available    - there is a buffer ready to send over the ethernet

   The local functions included are:
      none

   The locally global (shared) variable definitions included are:
      rx_buffers - the receive buffer pointers
      rx_current - buffer being filled with microphone data
      rx_next    - next buffer to fill with microphone data
      rx_xmit    - buffer being sent over the ethernet
      tx_buffers - the transmit buffer pointers
      tx_current - buffer being played over the speaker
      tx_next    - next buffer to play over the speaker
      tx_rcv     - buffer being filled with ethernet data


   Revision History
      6/6/06   Glen George       Initial revision.
      5/26/08  Glen George       Updated return types of buffer functions and
	                         types of buffer pointers.
      2/28/11  Glen George       Changed buffers to be short ints (16-bits)
	                         instead of unsigned chars (8-bits).
      3/9/11   Glen George       Added functions get_xmit_next_ptr() and
	                         get_rcv_next_ptr().
*/



/* library include files */
  /* none */

/* local include files */
#include  "interfac.h"
#include  "voipdefs.h"
#include  "buffers.h"




/* locally global (shared) variables */

/* receive buffers */
static short int  *rx_buffers[NUM_RX_BUFFERS];	/* the receive buffers */
static int  rx_current;		/* index of buffer being filled with microphone data */
static int  rx_next;		/* index of next buffer to be filled with microphone data */
static int  rx_xmit;		/* index of buffer being being sent over ethernet */

/* transmit buffers */
static short int  *tx_buffers[NUM_TX_BUFFERS];	/* the buffers */
static int  tx_current;		/* index of buffer being played */
static int  tx_next;		/* index of next buffer to be played */
static int  tx_rcv;		/* index of buffer being filled with ethernet data */




/* initialization functions */


/*
   init_buffers

   Description:      This function initializes the transmit and receive
   		     buffers to point to the actual DRAM buffers.  The buffer
		     indices are also reset to the empty state.

   Operation:        The array of buffer pointers is initialized to point to
   		     buffers at the start of DRAM.  The array indices are
		     initialized to no buffers allocated.

   Arguments:        None.
   Return Value:     None.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: rx_buffers (changed) - set to point to DRAM.
                     tx_buffers (changed) - set to point to DRAM.

   Author:           Glen George
   Last Modified:    February 28, 2011

*/

void  init_buffers()
{
    /* variables */
    short int  *buffer;		/* location of a buffer in DRAM */

    int         i;		/* loop index */



    /* initialize the receive buffers to point at dram */
    buffer = (short int *) DRAM_START;

    /* initialize all of the receive buffers */
    for (i = 0; i < NUM_RX_BUFFERS; i++)  {
        /* setup the pointer */
        rx_buffers[i] = buffer;
	/* point to the next buffer */
	buffer += AUDIO_BUFLEN;
    }

    /* setup the receive buffer indices for nothing in the buffers */
    reset_rx_buffer();


    /* initialize all of the transmit buffers */
    for (i = 0; i < NUM_TX_BUFFERS; i++)  {
        /* setup the pointer */
        tx_buffers[i] = buffer;
	/* point to the next buffer */
	buffer += AUDIO_BUFLEN;
    }

    /* setup the transmit buffer indices for nothing in the buffers */
    reset_tx_buffer();


    /* done with initialization - return */
    return;

}




/*
   reset_rx_buffer

   Description:      This function resets the receive buffer indices to
   		     indicate there is nothing in the buffers.  The actual
		     buffer pointers are unaffected.

   Operation:        The array indices are initialized to nothing in the
		     buffers.

   Arguments:        None.
   Return Value:     None.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: rx_current (changed) - set to NUM_RX_BUFFERS - 2.
		     rx_next (changed)    - set to NUM_RX_BUFFERS - 1.
		     rx_xmit (changed)    - set to -3.

   Author:           Glen George
   Last Modified:    June 6, 2006

*/

void  reset_rx_buffer()
{
    /* variables */
      /* none */



    /* setup the receive buffer indices for nothing in the buffers */
    rx_current = NUM_RX_BUFFERS - 2;
    rx_next = NUM_RX_BUFFERS - 1;
    rx_xmit = -3;


    /* done with initialization - return */
    return;

}




/*
   reset_tx_buffer

   Description:      This function resets the transmit buffer indices to
   		     indicate there is nothing in the buffers.  The actual
		     buffer pointers are unaffected.

   Operation:        The array indices are initialized to nothing in the
		     buffers.

   Arguments:        None.
   Return Value:     None.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: tx_current (changed) - set to NUM_TX_BUFFERS - 2.
		     tx_next (changed)    - set to NUM_TX_BUFFERS - 1.
		     tx_rcv (changed)     - set to NUM_TX_BUFFERS - 1.

   Author:           Glen George
   Last Modified:    June 6, 2006

*/

void  reset_tx_buffer()
{
    /* variables */
      /* none */



    /* setup the transmit buffer indices for nothing in the buffers */
    tx_current = NUM_TX_BUFFERS - 2;
    tx_next = NUM_TX_BUFFERS - 1;
    tx_rcv = NUM_TX_BUFFERS - 1;


    /* done with initialization - return */
    return;

}




/* receive buffer functions */


/*
   rx_available

   Description:      This function returns whether or not there is a buffer
   		     available for filling with microphone data.  If there is
		     a buffer available, TRUE is returned.  A buffer is
		     available if the next buffer (rx_next + 1) is not being
		     trasmitted over the ethernet interface (rx_xmit).

   Operation:        The buffer indices are checked to see if there is a
                     buffer available for filling with microphone data.

   Arguments:        None.
   Return Value:     (int) - TRUE (non-zero) if a buffer is available for
                        recording and FALSE (zero) otherwise.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: rx_next (accessed) - compared with rx_xmit to check for
   					  available buffers.
		     rx_xmit (accessed) - compared with tx_next to assess
		     			  buffer availability.

   Author:           Glen George
   Last Modified:    June 6, 2006

*/

int  rx_available()
{
    /* variables */
    int  next_index;		/* index of next buffer to be recorded to */



    /* find the next buffer we will record to */
    next_index = rx_next + 1;
    /* take care of any wrapping */
    if (next_index >= NUM_RX_BUFFERS)
	next_index = 0;


    /* return whether or not there is a buffer available */
    return  (next_index != rx_xmit);

}




/*
   xmit_available

   Description:      This function returns whether or not there is a buffer
   		     containing data ready to be transmitted over the ethernet
		     interface.  If there is a buffer available, TRUE is
		     returned.  A buffer is available if the next buffer to be
		     sent (rx_xmit + 1) is not being filled (rx_current).

   Operation:        The buffer pointers are checked to see if there is a
                     buffer with data to transmit over the ethernet interface.

   Arguments:        None.
   Return Value:     (int) - TRUE (non-zero) if a buffer is available for
                        transmission over the ethernet interface and FALSE
		        (zero) otherwise.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: rx_current (accessed) - compared with rx_xmit to check
   					     for available buffers.
		     rx_xmit (accessed)    - compared with rx_current to
		     			     assess buffer availability.

   Author:           Glen George
   Last Modified:    June 6, 2006

*/

int  xmit_available()
{
    /* variables */
    int  next_index;		/* index of the next buffer to transmit */



    /* index of next buffer of fill */
    next_index = rx_xmit + 1;
    if (next_index >= NUM_RX_BUFFERS)
	next_index = 0;


    /* return whether or not there is a buffer available */
    /* need to check for rx_xmit non-negative to handle initial state */
    /*   (rx_xmit is negative until there are two buffers being filled) */
    return  ((next_index >= 0) && (next_index != rx_current));

}




/*
   get_rx_next_ptr

   Description:      This function returns the pointer to the next available
   		     buffer for getting data from the microphone but does not
		     allocate that buffer.  If there is no available buffer,
		     NULL is returned.

   Operation:        If there is a buffer available, the pointer to that
                     buffer is found and returned.  Otherwise NULL is
                     returned.

   Arguments:        None.
   Return Value:     (short int *) - pointer to next available buffer for
                        recording data from the microphone.

   Input:            None.
   Output:           None.

   Error Handling:   If there is no available buffer, NULL is returned.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: rx_buffers (accessed) - retrieve the next pointer.
		     rx_next (accessed)    - to access next pointer in
		     			     rx_buffers array.

   Author:           Glen George
   Last Modified:    February 28, 2011

*/

short int  *get_rx_next_ptr()
{
    /* variables */
    short int  *next_buffer;	/* pointer to next available buffer */

    int         next_index;	/* index of the next available buffer */



    /* is there a buffer available */
    if (rx_available())  {

        /* get the index to the next available buffer (watch for wrap) */
	next_index = rx_next + 1;
	if (next_index >= NUM_RX_BUFFERS)
	    next_index = 0;

        /* get the pointer to the next buffer */
	next_buffer = rx_buffers[next_index];

    }
    else  {

        /* no buffer available - return NULL */
	next_buffer = NULL;
    }


    /* done - return with the pointer to the next buffer */
    return  next_buffer;

}




/*
   get_rx_buffer

   Description:      This function returns the pointer to the next available
   		     buffer for getting data from the microphone and allocates
		     that buffer.  In allocating that buffer it is assumed
		     that the data is double buffered and thus the buffer "two
		     back" must now be ready for transmitting over the
                     ethernet (thus rx_current is updated).  If there is no
		     available buffer, NULL is returned.

   Operation:        If there is a buffer available, the current buffer index
                     is incremented and the pointer to available buffer is
                     found and returned.  The other buffer indices which track
		     the ethernet and free buffers are also updated.  If there
		     is no buffer available the indices aren't changed and
		     NULL is returned.

   Arguments:        None.
   Return Value:     (short int *) - pointer to next available buffer for
                        recording data from the microphone.

   Input:            None.
   Output:           None.

   Error Handling:   If there is no available buffer, NULL is returned.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: rx_buffers (accessed) - retrieve the pointer from here.
                     rx_current (changed)  - updated to reflect a full buffer
		     			     must be available for tranmission
					     now that a new one is being
					     requested.
		     rx_next (changed)     - updated to move to the next
		     			     available buffer.
		     rx_xmit (changed)     - updated if in the initial state
		     			     where the index is negative.

   Author:           Glen George
   Last Modified:    February 28, 2011

*/

short int  *get_rx_buffer()
{
    /* variables */
    short int  *new_buffer;	    /* pointer to new buffer to allocate */



    /* is there a buffer available */
    if (rx_available())  {

	/* update next buffer index, watching for wrap */
	if (++rx_next >= NUM_RX_BUFFERS)
	    rx_next = 0;

        /* get the pointer to the buffer to allocate */
	new_buffer = rx_buffers[rx_next];

	/* also update the current buffer since we assume it is always */
	/* "one back" (always double buffered) - watch for wrap too */
	if (++rx_current >= NUM_RX_BUFFERS)
	    rx_current = 0;

	/* if the xmit index is negative that means we are in the initial */
	/* state and it needs to track the other indices until it gets to -1 */
	if (rx_xmit < -1)
	    rx_xmit++;
    }
    else  {

        /* no buffer available - return NULL */
	new_buffer = NULL;
    }


    /* done - return with the pointer to the buffer */
    return  new_buffer;

}




/*
   get_xmit_next_ptr

   Description:      This function returns the pointer to the next available
   		     buffer with data to be sent over the ethernet connection
		     but does not free that buffer.  If there is no available
		     buffer, NULL is returned.

   Operation:        If there is a buffer with data available, the pointer to
                     that buffer is found and returned.  Otherwise NULL is
                     returned.

   Arguments:        None.
   Return Value:     (short int *) - pointer to next buffer containing data
                        to be sent over the ethernet interface.

   Input:            None.
   Output:           None.

   Error Handling:   If there is no available buffer, NULL is returned.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: rx_buffers (accessed) - retrieve the next pointer.
		     rx_xmit (accessed)    - to access next pointer in
		     			     rx_buffers array.

   Author:           Glen George
   Last Modified:    March 9, 2011

*/

short int  *get_xmit_next_ptr()
{
    /* variables */
    short int  *next_buffer;	/* pointer to next buffer to send */

    int         next_index;	/* index of the next buffer to send */



    /* is there a buffer available */
    if (xmit_available())  {

        /* get the index to the next available buffer (watch for wrap) */
	next_index = rx_xmit + 1;
	if (next_index >= NUM_RX_BUFFERS)
	    next_index = 0;

        /* get the pointer to the next buffer */
	next_buffer = rx_buffers[next_index];

    }
    else  {

        /* no buffer available - return NULL */
	next_buffer = NULL;
    }


    /* done - return with the pointer to the next buffer */
    return  next_buffer;

}




/*
   get_xmit_buffer

   Description:      This function returns the pointer to the next available
   		     buffer with data ready to be sent over the ethernet
		     connection.  In allocating that buffer it is assumed that
		     the data is single buffered and thus the previously
		     transmitted buffer must now be free.  If there is no
		     buffer with data to be sent, NULL is returned.

   Operation:        If there is a buffer available, the current buffer index
                     is incremented and the pointer to available buffer is
                     found and returned.  Otherwise NULL is returned.

   Arguments:        None.
   Return Value:     (short int *) - pointer to next buffer containing data to
                        be sent over the ethernet interface.

   Input:            None.
   Output:           None.

   Error Handling:   If there is no buffer available with data, NULL is
                        returned.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: rx_buffers (accessed) - retrieve the pointer from here.
		     rx_xmit (changed)     - updated to point to the next
		     			     buffer with data.

   Author:           Glen George
   Last Modified:    February 28, 2011

*/

short int  *get_xmit_buffer()
{
    /* variables */
    short int  *new_buffer;	    /* pointer to new buffer to allocate */



    /* is there a buffer available */
    if (xmit_available())  {

	/* update transmit buffer index, watching for wrap */
	if (++rx_xmit >= NUM_RX_BUFFERS)
	    rx_xmit = 0;

        /* get the pointer to the buffer to be sent */
	new_buffer = rx_buffers[rx_xmit];
    }
    else  {

        /* no buffer available - return NULL */
	new_buffer = NULL;
    }


    /* done - return with the pointer to the buffer to transmit */
    return  new_buffer;

}




/* transmit buffer functions */


/*
   tx_available

   Description:      This function returns whether or not there is a buffer
   		     available with data for playing.  If there is a buffer
		     available, TRUE is returned.  A buffer is available if
		     the next buffer is not being filled.  This is indicated
		     by a distance of at least 2 between the next buffer
		     pointer (tx_next) and the buffer currently being filled
		     (tx_rcv).

   Operation:        The buffer indices are checked to see if there is a
   		     buffer available with data for playing.

   Arguments:        None.
   Return Value:     (int) - TRUE (non-zero) if a buffer is available for
                        playing and FALSE (zero) otherwise.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: tx_next (accessed) - compared with tx_rcv to check for
   					  available buffers.
		     tx_rcv (accessed)  - compared with tx_current to assess
		     			  buffer availability.

   Author:           Glen George
   Last Modified:    June 6, 2006

*/

int  tx_available()
{
    /* variables */
    int  index_diff;		/* difference between indices */



    /* compute the difference between the receive and next indices */
    index_diff = tx_rcv - tx_next;
    /* do modulo arithmetic */
    if (index_diff < 0)
	index_diff += NUM_TX_BUFFERS;


    /* return whether or not there is a buffer available */
    return  (index_diff >= 2);

}




/*
   rcv_available

   Description:      This function returns whether or not there is a buffer
   		     available for filling from the ethernet interface.  If
		     there is a buffer available, TRUE is returned.  A buffer
		     is available if the next buffer (tx_rcv + 1) is not being
		     played (tx_current).

   Operation:        The buffer indices are checked to see if there is a
   		     buffer available for filling with data from the ethernet
		     interface.

   Arguments:        None.
   Return Value:     (int) - TRUE (non-zero) if a buffer is available for
                        filling from the ethernet interface and FALSE (zero)
		        otherwise.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: tx_current (accessed) - compared with tx_rcv to check for
   					     available buffers.
		     tx_rcv (accessed)     - compared with tx_current to
		     			     assess buffer availability.

   Author:           Glen George
   Last Modified:    June 6, 2006

*/

int  rcv_available()
{
    /* variables */
    int  next_index;		/* index of the next buffer to fill */



    /* index of next buffer of fill */
    next_index = tx_rcv + 1;
    if (next_index >= NUM_TX_BUFFERS)
	next_index = 0;


    /* return whether or not there is a buffer available */
    return  (next_index != tx_current);

}




/*
   get_tx_next_ptr

   Description:      This function returns the pointer to the next available
   		     buffer for playing data, but does not free any buffers.
		     If there is no buffer available with data, NULL is
		     returned.

   Operation:        If there is a buffer with data available, the pointer to
                     that buffer is found and returned.  Otherwise NULL is
                     returned.

   Arguments:        None.
   Return Value:     (short int *) - pointer to next buffer containing data to
                        be played (from the ethernet interface).

   Input:            None.
   Output:           None.

   Error Handling:   If there is no available buffer, NULL is returned.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: tx_buffers (accessed) - retrieve the next pointer.
		     tx_next (accessed)    - to access next pointer in
		     			     tx_buffers array.

   Author:           Glen George
   Last Modified:    February 28, 2011

*/

short int  *get_tx_next_ptr()
{
    /* variables */
    short int  *next_buffer;	/* pointer to next available buffer */

    int         next_index;	/* index of the next available buffer */



    /* is there a buffer available */
    if (tx_available())  {

        /* get the index to the next available buffer (watch for wrap) */
	next_index = tx_next + 1;
	if (next_index >= NUM_TX_BUFFERS)
	    next_index = 0;

        /* get the pointer to the next buffer containing data */
	next_buffer = tx_buffers[next_index];

    }
    else  {

        /* no buffer available - return NULL */
	next_buffer = NULL;
    }


    /* done - return with the pointer to the next buffer with data ready */
    return  next_buffer;

}




/*
   get_tx_buffer

   Description:      This function returns the pointer to the next available
   		     buffer containing data ready to be played.  When this
		     buffer is retrieved it is assumed that the data is double
		     buffered and thus the buffer "two back" must now be free
		     (thus tx_current is updated).  If there is no available
		     buffer, NULL is returned.

   Operation:        If there is a buffer available, the current buffer index
                     is incremented and the pointer to available buffer is
                     found and returned.  Otherwise NULL is returned.

   Arguments:        None.
   Return Value:     (short int *) - pointer to next available buffer
                        containing data to be played (from the ethernet
		        interface).

   Input:            None.
   Output:           None.

   Error Handling:   If there is no available buffer, NULL is returned.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: tx_buffers (accessed) - retrieve the pointer from here.
                     tx_current (changed)  - updated to reflect a buffer must
		     			     be free now that a new one is
					     being requested.
		     tx_next (changed)     - updated to move to the next
		     			     available buffer.

   Author:           Glen George
   Last Modified:    February 28, 2011

*/

short int  *get_tx_buffer()
{
    /* variables */
    short int  *new_buffer;	    /* pointer to new buffer to allocate */



    /* is there a buffer available */
    if (tx_available())  {

	/* update next buffer index, watching for wrap */
	if (++tx_next >= NUM_TX_BUFFERS)
	    tx_next = 0;

        /* get the pointer to the buffer to allocate */
	new_buffer = tx_buffers[tx_next];

	/* also update the current buffer since we assume it is always */
	/* "one back" (always double buffered) - watch for wrap too */
	if (++tx_current >= NUM_TX_BUFFERS)
	    tx_current = 0;
    }
    else  {

        /* no buffer available - return NULL */
	new_buffer = NULL;
    }


    /* done - return with the pointer to the buffer */
    return  new_buffer;

}




/*
   get_rcv_next_ptr

   Description:      This function returns the pointer to the next available
   		     buffer that is ready to receive data from the ethernet
		     interface, but does not allocate any buffers.  If there
		     is no buffer available with data, NULL is returned.

   Operation:        If there is a buffer ready to be filled with data, the
                     pointer to that buffer is found and returned.  Otherwise
                     NULL is returned.

   Arguments:        None.
   Return Value:     (short int *) - pointer to next buffer ready to be filled
                        with data (by the ethernet interface).

   Input:            None.
   Output:           None.

   Error Handling:   If there is no available buffer, NULL is returned.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: tx_buffers (accessed) - retrieve the next pointer.
		     tx_rcv (accessed)     - to access next pointer in the
		     			     tx_buffers array.

   Author:           Glen George
   Last Modified:    March 9, 2011

*/

short int  *get_rcv_next_ptr()
{
    /* variables */
    short int  *next_buffer;	/* pointer to next available buffer */

    int         next_index;	/* index of the next available buffer */



    /* is there a buffer available */
    if (rcv_available())  {

        /* get the index to the next available buffer (watch for wrap) */
	next_index = tx_rcv + 1;
	if (next_index >= NUM_TX_BUFFERS)
	    next_index = 0;

        /* get the pointer to the next buffer waiting for data */
	next_buffer = tx_buffers[next_index];

    }
    else  {

        /* no buffer available - return NULL */
	next_buffer = NULL;
    }


    /* done - return with the pointer to the next buffer waiting for data */
    return  next_buffer;

}




/*
   get_rcv_buffer

   Description:      This function returns the pointer to the next available
   		     buffer that is ready to receive data from the ethernet
		     connection.  In allocating that buffer it is assumed that
		     the data is single buffered and thus the previously
		     received buffer must now be full and ready to play.  If
		     there is no buffer with data to be sent, NULL is
		     returned.

   Operation:        If there is a buffer available, the current buffer index
                     is incremented and the pointer to available buffer is
                     found and returned.  Otherwise NULL is returned.

   Arguments:        None.
   Return Value:     (short int *) - pointer to next available buffer for data
                        from the ethernet interface.

   Input:            None.
   Output:           None.

   Error Handling:   If there is no free buffer available, NULL is returned.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: tx_buffers (accessed) - retrieve the pointer from here.
		     tx_rcv (changed)      - updated to point to the buffer
		     			     now being filled with ethernet
					     data.

   Author:           Glen George
   Last Modified:    February 28, 2011

*/

short int  *get_rcv_buffer()
{
    /* variables */
    short int  *new_buffer;	    /* pointer to new buffer to allocate */



    /* is there a buffer available */
    if (rcv_available())  {

	/* update receive buffer index, watching for wrap */
	if (++tx_rcv >= NUM_TX_BUFFERS)
	    tx_rcv = 0;

        /* get the pointer to the buffer to be filled with ethernet data */
	new_buffer = tx_buffers[tx_rcv];
    }
    else  {

        /* no buffer available - return NULL */
	new_buffer = NULL;
    }


    /* done - return with the pointer to the buffer to transmit */
    return  new_buffer;

}
