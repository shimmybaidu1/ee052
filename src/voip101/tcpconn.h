/****************************************************************************/
/*                                                                          */
/*                               TCPCONN.H                                  */
/*                        TCP Interface Functions                           */
/*                              Include File                                */
/*                         VoIP Telephone Project                           */
/*                                EE/CS 52                                  */
/*                                                                          */
/****************************************************************************/

/*
   This file contains the constants, structures, and function prototypes for
   the TCP interface functions for the VoIP Telephone Project which are
   defined in tcpconn.c.


   Revision History
      3/10/11  Glen George       Initial revision.
*/




#ifndef  I__TCPCONN_H__
    #define  I__TCPCONN_H__


/* library include files */
  /* none */

/* local include files */
  /* none */




/* constants */
    /* none */




/* structures, unions, and typedefs */

/* status of the TCP connection */
enum tcp_conn_status  {
    CALL_NO_CONNECTION,		/* no connecton */
    CALL_CONNECTING,		/* trying to setup a full connection */
    CALL_RINGING,		/* have connection, it's ringing */
    CALL_BUSY,			/* have connection, it's busy */
    CALL_CONNECTED		/* have connection, can talk */
};




/* function declarations */

/* status functions */
char                  have_tcp_connection(void);	/* have incoming connection */
enum tcp_conn_status  tcp_connection_status(void);	/* get status */


/* connection functions */
void  tcp_connection_answer(void);		/* answer an incoming connection */
void  tcp_connection_close(void);		/* close the connection */
void  tcp_connection_connect(unsigned long int);/* connect to an IP address */
void  tcp_connection_init(void);		/* initialize connection */
void  tcp_connection_restart(void);		/* restart a connection */


/* receive/transmit functions */
char  tcp_connection_rx(short int *, int);	/* try to receive data */
char  tcp_connection_tx(short int *, int);	/* try to transmit data */


#endif
