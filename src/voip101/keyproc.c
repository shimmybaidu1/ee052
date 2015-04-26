/****************************************************************************/
/*                                                                          */
/*                                 KEYPROC                                  */
/*                  Miscellaneous Key Processing Functions                  */
/*                          VoIP Telephone Project                          */
/*                                EE/CS  52                                 */
/*                                                                          */
/****************************************************************************/

/*
   This file contains the general key processing functions for the VoIP
   Telephone Project.  These functions are called by the main loop of the
   system.  The functions included are:
      no_action   - nothing to do
      reset_input - reset the input state

   The local functions included are:
      none

   The global variable definitions included are:
      none


   Revision History
      6/3/06   Glen George       Initial revision.
*/



/* library include files */
  /* none */

/* local include files */
#include  "voipdefs.h"
#include  "keyproc.h"
#include  "callutil.h"




/*
   no_action

   Description:      This function handles a key when there is nothing to be
                     done.  It just returns.  It is needed to fill in the key
                     processing table.

   Arguments:        cur_status (enum status) - the current system status.
                     key_value (int)          - value of the key that was
                                                input (ignored).
   Return Value:     (enum status) - the new status (same as current status).

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Global Variables: None.

   Author:           Glen George
   Last Modified:    June 3, 2006

*/

enum status  no_action(enum status cur_status, int key_value)
{
    /* variables */
      /* none */



    /* return the current status */
    return  cur_status;

}




/*
   reset_input

   Description:      This function handles keys which cause the input state to
                     be reset.  The function resets the calling IP address and
                     returns idle as the new state.

   Arguments:        cur_status (enum status) - the current system status.
                     key_value (int)          - value of the key that was
                                                input (ignored).
   Return Value:     (enum status) - the new status (always STAT_IDLE).

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Global Variables: None.

   Author:           Glen George
   Last Modified:    June 3, 2006

*/

enum status  reset_input(enum status cur_status, int key_value)
{
    /* variables */
      /* none */



    /* reset the calling IP address */
    set_calling_IP(0L);


    /* now return with the idle state */
    return  STAT_IDLE;

}
