/****************************************************************************/
/*                                                                          */
/*                                KEYPROC.H                                 */
/*                         Key Processing Functions                         */
/*                               Include File                               */
/*                          VoIP Telphone Project                           */
/*                                EE/CS  52                                 */
/*                                                                          */
/****************************************************************************/

/*
   This file contains the constants and function prototypes for the key
   processing functions defined in ipproc.c, callproc.c, memproc.c, and
   keyproc.c.


   Revision History:
      6/3/06   Glen George       Initial revision.
*/



#ifndef  I__KEYPROC_H__
    #define  I__KEYPROC_H__


/* library include files */
  /* none */

/* local include files */
#include  "voipdefs.h"




/* constants */
    /* none */




/* structures, unions, and typedefs */
    /* none */




/* function declarations */

enum status  no_action(enum status, int);	/* nothing to do */
enum status  reset_input(enum status, int);	/* reset input state */

enum status  start_IPEntry(enum status, int);	/* start entering IP address */
enum status  clear_IPAddr(enum status, int);	/* clear the IP address */
enum status  add_IPDigit(enum status, int);	/* add a digit to the IP address */
enum status  del_IPDigit(enum status, int);	/* delete a digit from the IP address */
enum status  set_IP(enum status, int);		/* set the IP address */
enum status  set_gateway(enum status, int);	/* set the gateway address */
enum status  set_subnet(enum status, int);	/* set the subnet mask */

enum status  start_memLoc(enum status, int);	/* starting entering a memory location */
enum status  clear_memLoc(enum status, int);	/* clear the memory address */
enum status  add_memDigit(enum status, int);	/* add a digit to the memory address */
enum status  del_memDigit(enum status, int);	/* delete a digit from the memory address */
enum status  recall_mem(enum status, int);	/* recall an address from memory */
enum status  save_mem(enum status, int);	/* save an address to memory */

enum status  do_answer(enum status, int);	/* answer an incoming call */
enum status  do_call(enum status, int);		/* start an outgoing call */
enum status  end_call(enum status, int);	/* end a call */


#endif
