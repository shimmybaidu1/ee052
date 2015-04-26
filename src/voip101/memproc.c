/****************************************************************************/
/*                                                                          */
/*                                 MEMPROC                                  */
/*                       Memory Processing Functions                        */
/*                          VoIP Telephone Project                          */
/*                                EE/CS  52                                 */
/*                                                                          */
/****************************************************************************/

/*
   This file contains the key processing and general functions for memory
   operations for the VoIP Telephone Project.  These functions are called by
   the main loop of the system.  The functions included are:
      add_memDigit - add a digit to the memory location number
      clear_memLoc - clear the memory location number
      del_memDigit - delete a digit from the memory location number
      init_memory  - initialize the memory system
      recall_mem   - recall the IP address from the current memory location
      save_mem     - save the current IP address to current memory location
      start_memLoc - start entering a memory location number

   The local functions included are:
      none

   The global variable definitions included are:
      memloc     - current memory location
      saved_IPs  - array of saved IP memory addresses
      old_status - status of system before a save or recall operation

   Revision History
      6/3/06   Glen George       Initial revision.
      6/7/06   Glen George       Changed start_memLoc, recall_mem, and
      				 save_mem to remember the state the phone was
				 in before the IP address was saved or
				 recalled and restore that state if necessary.
*/



/* library include files */
  /* none */

/* local include files */
#include  "interfac.h"
#include  "voipdefs.h"
#include  "keyproc.h"
#include  "callutil.h"
#include  "error.h"




/* locally global variables */
static unsigned int       memloc;			/* the current memory location */
static unsigned long int  saved_IPs[MEMORY_SIZE + 1];	/* array of saved IP addresses */

static enum status        old_status;			/* status before a memory operation */




/*
   init_memory

   Description:      This function initializes the memory system.  It first
                     checks if location 0 has the "magic" value in it.  If so
                     the system is already initialized and there is nothing
                     to do.  If the "magic" value is not at location 0 then it
		     is written there and the other saved memory values are
		     set to 0.  This is done so the memory is maintained
		     between resets, as long as power doesn't fail.

   Arguments:        None.
   Return Value:     None.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: saved_IPs (changed) - initialized to all 0's.

   Author:           Glen George
   Last Modified:    June 3, 2006

*/

void  init_memory()
{
    /* variables */
    int  i;		/* general loop index */



    /* check if memory is already initialized */
    if (saved_IPs[0] != MAGIC_IP)  {

        /* memory is not initialized - need to initialize it */
	/* first save the "magic" value */
	saved_IPs[0] = MAGIC_IP;

	/* now set all other IPs to 0 */
	for (i = 1; i <= MEMORY_SIZE; i++)
	    saved_IPs[i] = 0;
    }


    /* done with memory initialization - return */
    return;

}




/*
   start_memLoc

   Description:      This function handles the start of entering a memory
                     location number.  It first clears the current memory
                     location, then sets the new system status based on the
                     passed key value and returns that status.

   Arguments:        cur_status (enum status) - the current system status.
                     key_value (int)          - value of the input key.
   Return Value:     (enum status) - the new status.

   Input:            None.
   Output:           None.

   Error Handling:   If there is an unexpected passed key value, the error
                     handler is called with the error UNKNOWN_KEYCODE_INIT.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: memloc (changed) - set to 0.

   Author:           Glen George
   Last Modified:    June 7, 2006

*/

enum status  start_memLoc(enum status cur_status, int key_value)
{
    /* variables */
      /* none */



    /* clear the memory location */
    memloc = 0;

    /* and display it */
    display_memory_addr(memloc);


    /* save the current status so can go back to it after saving/restoring */
    old_status = cur_status;


    /* figure out the new system status */
    switch (key_value)  {

        case  KEYCODE_MEMSAVE:	/* <Memory Save> key was seen */
				/* new status is saving an IP to memory */
				cur_status = STAT_MEMSAVE;
				break;

        case  KEYCODE_MEMRECALL:/* <Memory Recall> key was seen */
				/* new status is recalling an IP from memory */
				cur_status = STAT_MEMRECALL;
				break;

        default:		/* some other key was seen */
				/* generate an error and leave status unchanged */
				process_error(UNKNOWN_KEYCODE_INIT);
				break;
    }


    /* and return the new status */
    return  cur_status;

}




/*
   clear_memLoc

   Description:      This function handles the clear key when a memory
                     location number is being input.  It clears the current
		     memory location number, displays the now cleared number,
		     and returns with the system status it was passed.

   Arguments:        cur_status (enum status) - the current system status.
                     key_value (int)          - value of the key that was
                                                input (ignored).
   Return Value:     (enum status) - the new status (same as current status).

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: memloc (changed) - set to 0.

   Author:           Glen George
   Last Modified:    June 3, 2006

*/

enum status  clear_memLoc(enum status cur_status, int key_value)
{
    /* variables */
      /* none */



    /* clear the current memory location number */
    memloc = 0;

    /* display the new memory address */
    display_memory_addr(memloc);


    /* and return the current status */
    return  cur_status;

}




/*
   add_memDigit

   Description:      This function handles a digit key when a memory location
                     number is being input.  It adds the passed key value to
		     the current value of the memory location number.  If
		     there is an overflow (value larger than MEMORY_SIZE), the
                     error handler is called and the key is ignored.  The
		     function returns with the system status it was passed.

   Arguments:        cur_status (enum status) - the current system status.
                     key_value (int)          - value of the input key.
   Return Value:     (enum status) - the new status (same as current status).

   Input:            None.
   Output:           None.

   Error Handling:   If the input causes the memory location number to be to
                     large (greater than MEMORY_SIZE), the error handler is
		     called with the error MEMORY_ADDRESS_OVERFLOW and the key
		     is ignored.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: memloc (changed) - updated to new memory location number
                                        based on the passed key value.

   Limitations:      Does not work for memory sizes greater than 6553.

   Author:           Glen George
   Last Modified:    June 3, 2006

*/

enum status  add_memDigit(enum status cur_status, int key_value)
{
    /* variables */
      /* none */



    /* check if there is room for the digit */
    if (((10 * memloc) + key_value) <= MEMORY_SIZE)  {

        /* have room for the digit - add it in to the current location */
	memloc = (10 * memloc) + key_value;
    }
    else  {

        /* too big of a value - call the error handler */
	process_error(MEMORY_ADDRESS_OVERFLOW);
    }


    /* display the new memory location number */
    display_memory_addr(memloc);


    /* all done adding the digit, return the passed status */
    return  cur_status;

}




/*
   del_memDigit

   Description:      This function handles a backspace key when a memory
                     location number is being input.  It removes the last
		     input digit from the current value of the memory location
		     number.  If the current location is zero, it is assumed
		     that there are no more digits in the location number and
		     the error handler is called and the key is ignored.  The
		     function returns with the system status it was passed.

   Arguments:        cur_status (enum status) - the current system status.
                     key_value (int)          - value of the key that was
                                                input (ignored).
   Return Value:     (enum status) - the new status (same as current status).

   Input:            None.
   Output:           None.

   Error Handling:   If the current location number is zero then there are no
		     digits to delete in the location number and the key is
		     ignored and the error handler is called with the error
		     MEMORY_ADDRESS_UNDERFLOW.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: memloc (changed) - updated to the new memory location
                                        number by deleting the lowest digit.

   Author:           Glen George
   Last Modified:    June 3, 2006

*/

enum status  del_memDigit(enum status cur_status, int key_value)
{
    /* variables */
      /* none */



    /* are there any digits to delete? */
    if (memloc != 0)  {

        /* looks like there should be a digit to delete */
	/* remove the last digit - just divide by 10 */
	memloc /= 10;
    }
    else  {

        /* no digits to delete - call the error handler */
	process_error(MEMORY_ADDRESS_UNDERFLOW);
    }


    /* display the new memory location number */
    display_memory_addr(memloc);


    /* all done deleting the digit, return the passed status */
    return  cur_status;

}




/*
   save_mem

   Description:      This function handles the <Send/Enter> key when a memory
                     location is being saved.  If the memory location number
		     is valid, it saves the current IP address (even if it is
		     zero) at that memory location.  If the memory location
		     number is out of range (0 or greater than MEMORY_SIZE),
		     the error handler is called.  The function always returns
		     with the status it had when the save state was entered if
		     there was no error and with the passed state if there was
		     an error.

   Arguments:        cur_status (enum status) - the current system status.
                     key_value (int)          - value of the key that was
                                                input (ignored).
   Return Value:     (enum status) - the new status.

   Input:            None.
   Output:           None.

   Error Handling:   If the current location number is zero or greater than
		     MEMORY_SIZE the key is ignored and the error handler is
		     called with the error BAD_MEMORY_ADDRESS.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: memloc (accessed)   - used to index the saved_IPs array.
                     saved_IPs (changed) - current IP address is written to
                                           this array.

   Author:           Glen George
   Last Modified:    June 7, 2006

*/

enum status  save_mem(enum status cur_status, int key_value)
{
    /* variables */
      /* none */



    /* is the memory location number valid */
    if ((memloc > 0) && (memloc <= MEMORY_SIZE))  {

        /* valid memory location number - save the IP address */
	saved_IPs[memloc] = get_calling_IP();

	/* and restore the system status */
	cur_status = old_status;

	/* restore the old IP address too */
	display_IP(get_calling_IP());
    }
    else  {

        /* bad memory location number - call the error handler */
	process_error(BAD_MEMORY_ADDRESS);
    }


    /* all done saving an address, return the possibly new status */
    return  cur_status;

}




/*
   recall_mem

   Description:      This function handles the <Send/Enter> key when a memory
                     location is being recalled.  If the memory location
		     number is valid, it sets the current IP address to the
		     address saved at that memory location.  If the memory
		     location number is out of range (0 or greater than
		     MEMORY_SIZE), the error handler is called.  The function
		     returns with the STAT_RECALLED state if there was no
		     no error and it wasn't in the off-hook state before.  It
		     returns with the passed state if there was an error and
		     with the off-hook state if that's the state it was in
		     before.

   Arguments:        cur_status (enum status) - the current system status.
                     key_value (int)          - value of the key that was
                                                input (ignored).
   Return Value:     (enum status) - the new status.

   Input:            None.
   Output:           None.

   Error Handling:   If the current location number is zero or greater than
		     MEMORY_SIZE the key is ignored and the error handler is
		     called with the error BAD_MEMORY_ADDRESS.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: memloc (accessed)    - used to index the saved_IPs array.
                     saved_IPs (accessed) - recalled IP address is read from
                                            this array.

   Author:           Glen George
   Last Modified:    June 7, 2006

*/

enum status  recall_mem(enum status cur_status, int key_value)
{
    /* variables */
      /* none */



    /* is the memory location number valid */
    if ((memloc > 0) && (memloc <= MEMORY_SIZE))  {

        /* valid memory location number - get the saved IP address */
	set_calling_IP(saved_IPs[memloc]);

	/* display the recalled IP address */
	display_IP(saved_IPs[memloc]);

	/* and change to the recalled memory state if we weren't off-hook */
	if (old_status != STAT_OFFHOOK)
	    cur_status = STAT_RECALLED;
	else
	    /* were off hook, so go back to that state */
	    cur_status = STAT_OFFHOOK;
    }
    else  {

        /* bad memory location number - call the error handler */
	process_error(BAD_MEMORY_ADDRESS);
    }


    /* all done recalling memory, return the possibly new status */
    return  cur_status;

}
