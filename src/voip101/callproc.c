/****************************************************************************/
/*                                                                          */
/*                                 CALLPROC                                 */
/*                        Call Processing Functions                         */
/*                          VoIP Telephone Project                          */
/*                                 EE/CS 52                                 */
/*                                                                          */
/****************************************************************************/

/*
   This file contains the key processing functions for initiating and ending
   calls for the VoIP Telephone Project.  These functions are called by the
   main loop of the system.  The functions included are:
      do_answer - handle picking up an incoming call
      do_call   - initiate an outgoing call
      end_call  - end a call

   The local functions included are:
      none

   The global variable definitions included are:
      none


   Revision History
      6/3/06   Glen George       Initial revision.
      3/8/11   Glen George       Updated comments.
*/



/* library include files */
  /* none */

/* local include files */
#include  "interfac.h"
#include  "voipdefs.h"
#include  "keyproc.h"
#include  "error.h"
#include  "callutil.h"




/*
   do_answer

   Description:      This function handles answering a phone call.  It is
                     assumed that there is an incoming call and this function
                     is called when the phone goes "off hook".
			     
   Operation:        The function initiates the call by calling the function
		     connect_incoming() and then returns the status
		     STAT_CONNECTED.

   Arguments:        cur_status (enum status) - the current system status
                                                (ignored).
                     key_value (int)          - value of the key that was
                                                input (ignored).
   Return Value:     (enum status) - the new status (always STAT_CONNECTED).

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: None.

   Author:           Glen George
   Last Modified:    June 3, 2006

*/

enum status  do_answer(enum status cur_status, int key_value)
{
    /* variables */
      /* none */



    /* connect to the incoming call */
    connect_incoming();


    /* and return that we are connected now */
    return  STAT_CONNECTED;

}




/*
   do_call

   Description:      This function handles the <Send> key to start an outgoing
                     call.

   Operation:        It just starts the call by calling initiate_outgoing()
                     and then returns the status STAT_CONNECTING.

   Arguments:        cur_status (enum status) - the current system status
   						(ignored).
                     key_value (int)          - value of the key that was
                                                input (ignored).
   Return Value:     (enum status) - the new status (always STAT_CONNECTING).

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: None.

   Author:           Glen George
   Last Modified:    June 3, 2006

*/

enum status  do_call(enum status cur_status, int key_value)
{
    /* variables */
      /* none */



    /* start the outgoing call */
    initiate_outgoing();


    /* and return the new status - STAT_CONNECTING */
    return  STAT_CONNECTING;

}




/*
   end_call

   Description:      This function handles the end of a call, when the user
                     hangs up.

   Operation:        It disconnects the call by calling disconnect_call() and
		     returns the status STAT_IDLE.

   Arguments:        cur_status (enum status) - the current system status
   						(ignored).
                     key_value (int)          - value of the key that was
                                                input (ignored).
   Return Value:     (enum status) - the new status (always STAT_IDLE).

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: None.

   Author:           Glen George
   Last Modified:    June 3, 2006

*/

enum status  end_call(enum status cur_status, int key_value)
{
    /* variables */
      /* none */



    /* disconnect the call */
    disconnect_call();


    /* and return with the new status - idle */
    return  STAT_IDLE;

}
