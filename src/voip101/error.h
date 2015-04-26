/****************************************************************************/
/*                                                                          */
/*                                 ERROR.H                                  */
/*                             Error Processing                             */
/*                               Include File                               */
/*                          VoIP Telphone Project                           */
/*                                EE/CS  52                                 */
/*                                                                          */
/****************************************************************************/

/*
   This file contains the error processing definitions for the VoIP Telephone
   Project.  This includes constant and enum definitions along with function
   declarations for the error processing functions defined in error.c.


   Revision History:
      6/3/06   Glen George       Initial revision.
      6/8/06   Glen George       Added error value (UNKNOWN_ETHER_NAME).
      3/10/11  Glen George       Added a number of error values related to
	                         TCP communication.
*/



#ifndef  I__ERROR_H__
    #define  I__ERROR_H__


/* library include files */
  /* none */

/* local include files */
  /* none */




/* constants */

  /* none */




/* structures, unions, and typedefs */

/* error types */
enum error_type  {
    UNKNOWN_KEYCODE_INIT,	/* unknown keycode in memory or IP initialization */
    IP_ADDRESS_OVERFLOW,	/* overflow in entered IP address */
    IP_ADDRESS_UNDERFLOW,	/* underflow in entered IP address */
    MEMORY_ADDRESS_OVERFLOW,	/* overflow in entered memory location */
    MEMORY_ADDRESS_UNDERFLOW,	/* underflow in entered memory location */
    BAD_MEMORY_ADDRESS,		/* bad memory address on save/recall */
    UNKNOWN_ETHER_NAME,		/* unknown ethernet interface name */
    UNKNOWN_TCP_STATUS,		/* unknown TCP status returned */
    MULTIPLE_CONNECTIONS,	/* multiple connections attempted */
    NETERR_NOLISTEN,		/* can't setup listener */
    NETERR_NOBIND,		/* can't bind the port */
    NETERR_SEND,		/* error sending a packet */
    NETERR_CLOSE,		/* error closing a connection */
    NETERR_NOCONNECT,		/* error setting up a connection */
    NETERR_UNKNOWN_PACKET,	/* unknown packet type received */
    NETERR_GENERAL		/* general networking error */
};




/* function declarations */

void  process_error(enum error_type);	/* process an error */


#endif
