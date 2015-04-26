/****************************************************************************/
/*                                                                          */
/*                                   ERROR                                  */
/*                        Error Processing Functions                        */
/*                          VoIP Telephone Project                          */
/*                                 EE/CS 52                                 */
/*                                                                          */
/****************************************************************************/

/*
   This file contains the erro processing functions for the VoIP Telephone
   Project.  These functions are called whenever an error occurs.  Currently
   they do nothing, but they exist to allow error handling to be added to the
   system.  The functions included are:
      process_error - process the passed error code

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
#include  "error.h"




/*
   process_error

   Description:      This function processes the passed error.  The error is
                     indicated by the value of the passed enumerated type.
                     Currently the function does nothing, but is here to allow
                     for error handling to be added in the future.

   Arguments:        e (enum error_type) - type of the error that occurred.
   Return Value:     None.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: None.

   Author:           Glen George
   Last Modified:    May 31, 2006

*/

void  process_error(enum error_type e)
{
    /* variables */
      /* none */



    /* do nothing for now, just return */
    return;

}
