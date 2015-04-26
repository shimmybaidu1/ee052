/****************************************************************************/
/*                                                                          */
/*                                TCPCONN                                   */
/*                        TCP Interface Functions                           */
/*                         VoIP Telephone Project                           */
/*                                EE/CS 52                                  */
/*                                                                          */
/****************************************************************************/

/*
   This file contains the functions for managing the TCP interface for the
   VoIP Telephone Project.  The functions included are:
      have_tcp_connection    - have an incoming connection
      tcp_connection_answer  - answer an incoming connection
      tcp_connection_close   - close the connection
      tcp_connection_connect - connect to an IP address
      tcp_connection_init    - initialize the connection
      tcp_connection_restart - restart the TCP connections
      tcp_connection_rx      - attempt to receive TCP data
      tcp_connection_status  - get the connection status
      tcp_connection_tx      - attempt to transmit TCP data

   The local functions included are:
      accept_connection - accept a TCP connection being made (callback)
      busy_sent         - the busy packet has been sent (callback)
      check_for_close   - check if the connection is closed and handle it
      error_handler     - handle errors for TCP connections (callback)
      handle_connection - handle a TCP connection being made (callback)
      receive_data      - receive TCP data (callback)

   The locally global (shared) variable definitions included are:
      call_pcb     - protocol control block for a connected call
      cur_status   - status of the TCP connection
      have_rx_data - flag indicating received data is available
      listener     - the listener for incoming connections
      rx_data      - the data from a received data packet


   Revision History
      3/10/11  Glen George       Initial revision.
*/



/* library include files */
#include  "lwip/opt.h"
#include  "lwip/ip_addr.h"
#include  "lwip/pbuf.h"
#include  "ipv4/lwip/ip.h"
#include  "lwip/err.h"
#include  "lwip/tcp.h"

/* local include files */
#include  "interfac.h"
#include  "voipdefs.h"
#include  "tcpconn.h"
#include  "callutil.h"
#include  "error.h"




/* local function declarations */
static err_t  accept_connection(void *, struct tcp_pcb *, err_t);
static err_t  busy_sent(void *, struct tcp_pcb *, u16_t);
static void   check_for_close(void);
static void   error_handler(void *, err_t);
static err_t  handle_connection(void *, struct tcp_pcb *, err_t);
static err_t  receive_data(void *, struct tcp_pcb *, struct pbuf *, err_t);




/* locally global (shared) variables */

/* status of the TCP connection */
static enum tcp_conn_status  cur_status;

/* protocol control block for a connected call */
static struct tcp_pcb  *call_pcb;

/* the listener for incoming connections */
static struct tcp_pcb  *listener;

/* a received data packet */
static short int  rx_data[AUDIO_BUFLEN];
/* flag indicating data is available */
static int        have_rx_data;




/* status functions */


/*
   have_tcp_connection

   Description:      This function returns TRUE if there is a valid TCP
   		     connection.

   Operation:        The connection is first checked to see if it is closed.
   		     Then TRUE is returned if the shared variable call_pcb is
                     not NULL and FALSE is returned if it is NULL.

   Arguments:        None.
   Return Value:     (char) - TRUE is returned if there is a valid TCP
                        connection and FALSE is returned otherwise.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: call_pcb (accessed) - checked for NULL.

   Author:           Glen George
   Last Modified:    March 10, 2011

*/

char  have_tcp_connection()
{
    /* variables */
      /* none */



    /* check if the connection is closed */
    check_for_close();


    /* there is a connection if call_pcb is not NULL */
    return  (call_pcb != NULL);

}




/*
   tcp_connection_status

   Description:      This function returns the current status of the TCP
   		     connection.

   Operation:        First the connection is checked to see if it has been
                     closed.  Then the value of the shared variable cur_status
		     is returned.

   Arguments:        None.
   Return Value:     (enum tcp_conn_status) - current status of the TCP
                        connection.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: cur_status (accessed) - value to return.

   Author:           Glen George
   Last Modified:    March 10, 2011

*/

enum tcp_conn_status  tcp_connection_status()
{
    /* variables */
      /* none */



    /* check if the connection has been closed */
    check_for_close();

    /* and just return the value of cur_status */
    return  cur_status;

}




/* connection functions */


/*
   tcp_connection_init

   Description:      This function initializes the TCP connection software.

   Operation:        The shared variables are initialized and the LwIP TCP
	   	     interface is initialized.  Then the interface is
		     restarted.

   Arguments:        None.
   Return Value:     None.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: call_pcb (changed)   - set to NULL.
                     cur_status (changed) - set to CALL_NO_CONNECTION.
                     listener (changed)   - initialized.

   Author:           Glen George
   Last Modified:    March 10, 2011

*/

void  tcp_connection_init()
{
    /* variables */
      /* none */



    /* currently there is no connection */
    cur_status = CALL_NO_CONNECTION;
    call_pcb = NULL;

    /* no listener either */
    listener = NULL;


    /* initialize the LwIP code */
    tcp_init();


    /* just restart the connection */
    tcp_connection_restart();


    /* done with initialization, return */
    return;

}




/*
   tcp_connection_restart

   Description:      This function restarts the TCP connection.  This is
                     useful when the link has changed.  For example, when the
		     IP address is changed.

   Operation:        Any open connections are closed.  Then the status is
	   	     reset and a listener is setup to listen for incoming
		     calls.

   Arguments:        None.
   Return Value:     None.

   Input:            None.
   Output:           None.

   Error Handling:   If there is an error setting up the listener the
   		     process_error function is called with either a
		     NETERR_NOLISTEN or a NETERR_NOBIND error code, depending
		     on the exact error.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: call_pcb (changed)     - set to NULL.
                     cur_status (changed)   - set to CALL_NO_CONNECTION.
                     have_rx_data (changed) - set to FALSE.
                     listener (changed)     - initialized.

   Author:           Glen George
   Last Modified:    March 10, 2011

*/

void  tcp_connection_restart()
{
    /* variables */
    struct tcp_pcb  *new_listener;	/* new listener on a port */



    /* check if there is a call in progress */
    if (call_pcb != NULL)
        /* have a call in progress, need to close it */
        tcp_abort(call_pcb);

    /* check if currently listening */
    if (listener != NULL)
        /* are listening, need to close the port */
        tcp_abort(listener);


    /* now there is no connection */
    cur_status = CALL_NO_CONNECTION;
    call_pcb = NULL;

    /* and no data available */
    have_rx_data = FALSE;
    

    /* create a protocol control block for listening for an incoming connection */
    listener = tcp_new();

    /* setup to listen on port CALL_LISTEN_PORT using the set IP address */
    if (tcp_bind(listener, IP_ADDR_ANY, CALL_LISTEN_PORT) == ERR_OK)  {

        /* bound the port, now listen for a connection */
        new_listener = tcp_listen(listener);
        /* if have a listener, update the pointer (old pointer is bad now) */
        if (new_listener != NULL)  {

	    /* have a listener - set it up */
            listener = new_listener;

            /* setup the callback function for accepting connections */
            tcp_accept(listener, accept_connection);

 	    /* no argument in callbacks */
	    tcp_arg(listener, NULL);
            /* need a callback for the error handler */
            tcp_err(listener, error_handler);
        }
        else  {

            /* error getting a listener, report it */
            process_error(NETERR_NOLISTEN);
        }
    }
    else  {

        /* error binding to the port, report it */
        process_error(NETERR_NOBIND);
    }


    /* done restarting the interface, return */
    return;

}




/*
   tcp_connection_connect

   Description:      This function tries to connect to a remote device whose
                     IP address is passed.

   Operation:        If there is currently no connection a connection is
	   	     started.

   Arguments:        ip (unsigned long int) - the IP address to connect to.
   Return Value:     None.

   Input:            None.
   Output:           None.

   Error Handling:   If there is already a connection, the process_error
                     function is called with the error MULTIPLE_CONNECTIONS.
                     If there is an error setting up the connection the
		     process_error function is called with the error
		     NETERR_NOCONNECT.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: call_pcb (accessed)  - checked to see if there is already
                                            a connection.

   Author:           Glen George
   Last Modified:    March 10, 2011

*/

void  tcp_connection_connect(unsigned long int ip)
{
    /* variables */
    struct tcp_pcb  *outgoing;		/* outgoing TCP connection */
    struct ip_addr   ipaddr;		/* IP address to connect to */



    /* check if there is already a connection */
    if (call_pcb == NULL)  {

        /* no connection yet, try to start one */
        /* create the protocol control block for the outgoing connection */
        outgoing = tcp_new();

	/* no argument in callbacks */
	tcp_arg(outgoing, NULL);
	/* need a callback for the error handler */
	tcp_err(outgoing, error_handler);

	/* setup the IP address structure */
	ipaddr.addr = htonl(ip);

        /* attempt to connect to the remote phone */
        if (tcp_connect(outgoing, &ipaddr, CALL_LISTEN_PORT, handle_connection) != ERR_OK)
            /* error setting up the connection, report it */
            process_error(NETERR_NOCONNECT);
    }
    else  {

        /* already have a connection - that's an error */
	process_error(MULTIPLE_CONNECTIONS);
    }


    /* done setting up the connection, return */
    return;

}




/*
   tcp_connection_answer

   Description:      This function answers an incoming call whose connection
                     was already established.

   Operation:        An answer packet is sent over the connection and the
	   	     connection is setup to receive data.

   Arguments:        None.
   Return Value:     None.

   Input:            None.
   Output:           None.

   Error Handling:   If there is an error sending the packet, the
                     process_error function is called with the error
                     NETERR_SEND.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: call_pcb (accessed)    - used to send the answer packet.
                     cur_status (changed)   - updated to reflect the new
                                              connection status (connected).
                     have_rx_data (changed) - set to FALSE.

   Author:           Glen George
   Last Modified:    March 10, 2011

*/

void  tcp_connection_answer(void)
{
    /* variables */
      /* none */



    /* setup the function to receive any data for the call */
    tcp_recv(call_pcb, receive_data);

    /* no data yet */
    have_rx_data = FALSE;


    /* send an answer packet */
    if (tcp_write(call_pcb, "A", 1, 1) != ERR_OK)
        /* error sending the packet - report it */
        process_error(NETERR_SEND);

    /* and the connection is now established */
    cur_status = CALL_CONNECTED;


    /* done answering the call, return */
    return;

}




/*
   tcp_connection_close

   Description:      This function closes the current connection.

   Operation:        If there is a connection, it is closed and the shared
   		     variable call_pcb is set to NULL and the connection
		     status is set to no connection.

   Arguments:        None.
   Return Value:     None.

   Input:            None.
   Output:           None.

   Error Handling:   If there is an error closing the connection, the
                     process_error function is called with the error
		     NETERR_CLOSE.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: call_pcb (changed)     - set to NULL.
                     cur_status (changed)   - set to CALL_NO_CONNECTION.
                     have_rx_data (changed) - set to FALSE.

   Author:           Glen George
   Last Modified:    March 10, 2011

*/

void  tcp_connection_close()
{
    /* variables */
      /* none */



    /* if there is a connection, close it, watching for errors */
    if ((call_pcb != NULL) && (tcp_close(call_pcb) != ERR_OK))
        /* error closing the connection - report it */
        process_error(NETERR_CLOSE);


    /* there is no longer a connection */
    cur_status = CALL_NO_CONNECTION;
    call_pcb = NULL;
    /* and no received data */
    have_rx_data = FALSE;


    /* done closing the connection, return */
    return;

}




/*
   tcp_connection_rx

   Description:      This function attempts to get received data and returns
                     it in the passed buffer.

   Operation:        If there is received data it is copied to the passed
	   	     buffer up to the total data available and the buffer
		     length and TRUE is returned.  Otherwise FALSE is
		     returned.

   Arguments:        buf (short int *) - pointer to the buffer for the
                                         received data.
                     len (int)         - length of the buffer.
   Return Value:     (char) - TRUE if there was data and the buffer was
                        filled and FALSE otherwise.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: have_rx_data (changed) - checked and set to FALSE.
                     rx_data (accessed)     - copied to the passed buffer.

   Author:           Glen George
   Last Modified:    March 10, 2011

*/

char  tcp_connection_rx(short int *buf, int len)
{
    /* variables */
    char  status;		/* return status */

    int   i;			/* general loop index */



    /* check if data is available */
    if (have_rx_data)  {

        /* have data, copy it into the buffer */
	for (i = 0; ((i < len) && (i < AUDIO_BUFLEN)); i++)
	    buf[i] = rx_data[i];

        /* no longer have data (already copied) */
	have_rx_data = FALSE;

        /* and the data was successfully copied */
	status = TRUE;
    }
    else  {

        /* no data to copy, return FALSE */
	status = FALSE;
    }


    /* return whether or not any data was actually received */
    return  status;

}




/*
   tcp_connection_tx

   Description:      This function attempts to send the passed data over the
                     TCP connection.

   Operation:        A packet is created for the passed data and it is sent
	   	     over the connection.  If there is an error sending the
		     packet, the error handler is called and FALSE is
		     returned.

   Arguments:        buf (short int *) - pointer to the buffer with the data
                                         to be sent (16-bit values).
                     len (int)         - length of the buffer (number of
		     			 16-bit values).
   Return Value:     (char) - TRUE if the passed data was successfully sent
                        and FALSE otherwise.

   Input:            None.
   Output:           None.

   Error Handling:   If there is an error sending the packet, FALSE is
                     returned and the process_error function is called with
                     the error NETERR_SEND.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: call_pcb (accessed) - used to send the data packet.

   Author:           Glen George
   Last Modified:    March 10, 2011

*/

char  tcp_connection_tx(short int *buf, int len)
{
    /* variables */
    char  packet[AUDIO_BUFLEN * 2 + 1];		/* packet to send */

    char  status;				/* return status */

    int   i;					/* general loop index */



    /* create the packet */

    /* packet type is data (D) */
    packet[0] = 'D';

    /* copy the data from the passed buffer */
    for (i = 0; ((i < len) && (i < AUDIO_BUFLEN)); i++)  {
        /* break the passed 16-bit values into bytes */
	packet[2 * i + 1] = (buf[i] >> 8) & 0xFF;
	packet[2 * i + 2] = buf[i] & 0xFF ;
    }


    /* now send the packet, watching for errors */
    if (tcp_write(call_pcb, packet, sizeof(packet), 1) != ERR_OK)  {
        /* error sending the packet - report it */
        process_error(NETERR_SEND);
	/* remember that there was an error */
	status = FALSE;
    }
    else  {

        /* no error */
	status = TRUE;
    }


    /* done sending the data, return whether we were successful or not */
    return  status;

}




/* utility functions */


/*
   check_for_close

   Description:      This function checks if the call connection has been
                     closed (most likely by the remote system) and, if so,
		     cleans up.

   Operation:        The state of the TCP connection is checked and if it is
	   	     closed, the connection is closed at this end and the
		     shared variables are reset to no call.

   Arguments:        None.
   Return Value:     None.

   Input:            None.
   Output:           None.

   Error Handling:   If there is an error closing the connection, the
                     process_error function is called with the error
		     NETERR_CLOSE.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: call_pcb (changed)     - checked and possibly set to
                                              NULL.
                     cur_status (changed)   - possibly reset to no connection.
                     have_rx_data (changed) - set to FALSE.

   Author:           Glen George
   Last Modified:    March 10, 2011

*/
void  check_for_close()
{
    /* variables */
      /* none */



    /* check if there is a connection and it is now closed */
    if ((call_pcb != NULL) && (call_pcb->state == CLOSED))  {

        /* have a closed connection, close it on this end too */
        if (tcp_close(call_pcb) != ERR_OK)
            /* error closing the connection - report it */
            process_error(NETERR_CLOSE);

        /* there is no longer a connection */
        cur_status = CALL_NO_CONNECTION;
        call_pcb = NULL;
        /* and no data */
        have_rx_data = FALSE;
    }


    /* done checking if the connection is closed, return */
    return;

}




/* callback functions */


/*
   handle_connection

   Description:      This function handles a connection being successfully
                     made.

   Operation:        The status is changed to reflect we are trying to connect
	   	     (actual call, not TCP connection) and the receive data
		     handler is setup for the connection.

   Arguments:        arg (void *)            - not used.
   		     conn (struct tcp_pcb *) - protocol control block for the
                                               connection.
		     err (err_t)             - error code for the connection.
   Return Value:     (err_t) - error status, the passed error code.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: call_pcb (changed)     - set to the passed connection.
                     cur_status (changed)   - changed to indicate trying to
                                              establish a call.
                     have_rx_data (changed) - set to FALSE.

   Author:           Glen George
   Last Modified:    March 10, 2011

*/

static err_t  handle_connection(void *arg, struct tcp_pcb *conn, err_t err)
{
    /* variables */
      /* none */



    /* have a connection, this is the calling protocol control block */
    call_pcb = conn;

    /* setup the callbacks */

    /* no argument in callbacks */
    tcp_arg(conn, NULL);
    /* need a callback for the error handler */
    tcp_err(conn, error_handler);
    /* and setup the data processing callback */
    tcp_recv(conn, receive_data);


    /* the status is now that we are trying to establish a call */
    cur_status = CALL_CONNECTING;

    /* no data yet */
    have_rx_data = FALSE;


    /* return the passed error code */
    return  err;

}




/*
   accept_connection

   Description:      This function handles accepting a connection.

   Operation:        If there is currently no call in progress the connection
	   	     is accepted and a ringing packet is sent.  If there is a
		     call in progress the connection is accepted and the busy
		     packet is sent.

   Arguments:        arg (void *)            - not used.
   		     conn (struct tcp_pcb *) - protocol control block for the
                                               connection.
		     err (err_t)             - error code for the connection.
   Return Value:     (err_t) - error status, always the passed error code,

   Input:            None.
   Output:           None.

   Error Handling:   If there is an error sending the busy or ringing packet,
                     the process_error function is called with the error
		     NETERR_SEND.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: call_pcb (changed)   - checked if NULL and set to the
                                            incoming connection if so.
                     cur_status (changed) - changed to CALL_CONNECTING if are
                                            trying to establish a connection.

   Author:           Glen George
   Last Modified:    March 10, 2011

*/
static err_t  accept_connection(void *arg, struct tcp_pcb *conn, err_t err)
{
    /* variables */
      /* none */



    /* accept the incoming connection */
    tcp_accepted(conn);

    /* no argument in callbacks */
    tcp_arg(conn, NULL);
    /* need a callback for the error handler */
    tcp_err(conn, error_handler);


    /* is there already a call in progress */
    if (call_pcb == NULL)  {

        /* no call in progress - remember the incoming connection */
        call_pcb = conn;
	/* set the calling IP number */
	set_calling_IP(ntohl(conn->remote_ip.addr));

	/* send a ringing packet */
	if (tcp_write(conn, "R", 1, 1) != ERR_OK)
	    /* error sending the packet - report it */
	    process_error(NETERR_SEND);

        /* and the status is now that we are trying to connect */
	cur_status = CALL_CONNECTING;
    }
    else  {

        /* already have a call in progress - send back a busy signal */
	/* need callback for data sent */
	tcp_sent(conn, busy_sent);

	/* send the busy packet */
	if (tcp_write(conn, "B", 1, 1) != ERR_OK)
	    /* error sending the packet - report it */
	    process_error(NETERR_SEND);
    }


    /* nothing else to do, return with the passed error code */
    return  err;

}




/*
   receive_data

   Description:      This function handles receiving data over a TCP
                     connection.

   Operation:        If there is currently no call in progress the connection
	   	     is accepted and a ringing packet is sent.  If there is a
		     call in progress the connection is accepted and the busy
		     packet is sent.

   Arguments:        arg (void *)            - not used.
   		     conn (struct tcp_pcb *) - protocol control block for the
                                               connection.
		     packet (struct pbuf *)  - pointer to the packet holding
		     			       the data received.
		     err (err_t)             - error code for the connection.
   Return Value:     (err_t) - error status, always the passed error code,

   Input:            None.
   Output:           None.

   Error Handling:   If the packet type is unknown, the process_error function
                     is called with the error NETERR_UNKNOWN_PACKET.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: cur_status (changed)   - possibly updated based on actual
                                              packets received.
                     have_rx_data (changed) - set to TRUE if it is a data
                                              packet.
                     rx_data (changed)      - filled with packet data if it is
                                              a data packet.

   Author:           Glen George
   Last Modified:    March 10, 2011

*/
static err_t  receive_data(void *arg, struct tcp_pcb *conn,
                           struct pbuf *packet, err_t err)
{
    /* variables */
    int  i;		/* general loop index */



    /* acknowledge that we've gotten the payload */
    tcp_recved(conn, packet->len);


    /* figure out what kind of packet this was */
    switch(((char *)(packet->payload))[0])  {

        case 'A':   /* answer packet */
		    /* the other side is answering, change status */
		    cur_status = CALL_CONNECTED;
		    break;

	case 'R':   /* ringing packet */
		    /* the phone is ringing on the other end */
		    cur_status = CALL_RINGING;
		    break;

	case 'B':   /* busy packet */
		    /* the phone is busy on the other end */
		    cur_status = CALL_BUSY;
		    break;

        case 'D':   /* data packet */
		    /* have data, copy it into the buffer */
		    for (i = 0; i < AUDIO_BUFLEN; i++)  {
		        /* copy a word into the data buffer */
			rx_data[i] = ((((unsigned char *)(packet->payload))[2 * i + 1]) << 8) |
			             ((((unsigned char *)(packet->payload))[2 * i + 2]) & 0xFF);
		    }
		    /* have data now */
		    have_rx_data = TRUE;
		    break;

        default:    /* unknown packet */
		    /* report an error, this shouldn't happen */
		    process_error(NETERR_UNKNOWN_PACKET);
		    break;
    }


    /* done with the packet, release it */
    pbuf_free(packet);


    /* have processed the received data, return with the passed error code */
    return  err;

}




/*
   busy_sent

   Description:      This function handles the busy packet being successfully
                     sent.

   Operation:        The connection is closed while watching for an error.

   Arguments:        arg (void *)            - not used.
   		     conn (struct tcp_pcb *) - protocol control block for the
                                               connection over which the busy
					       packet was sent.
		     len (u16_t)             - number of bytes sent (ignored).
   Return Value:     (err_t) - error code for handling the data being sent,
                        always ERR_OK.

   Input:            None.
   Output:           None.

   Error Handling:   If there is an error closing the connection, the
                     process_error function is called with the error
		     NETERR_CLOSE.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: None.

   Author:           Glen George
   Last Modified:    March 10, 2011

*/
static err_t  busy_sent(void *arg, struct tcp_pcb *conn, u16_t len)
{
    /* variables */
      /* none */



    /* the busy packet has been sent, close the connection */
    if (tcp_close(conn) != ERR_OK)
        /* error closing the connection - report it */
	process_error(NETERR_CLOSE);


    /* nothing else to do, return with no error */
    return  ERR_OK;

}




/*
   error_handler

   Description:      This function handles any errors from a TCP connection.

   Operation:        The process_error function is called with NETERR_GENERAL.

   Arguments:        arg (void *) - not used.
		     err (err_t)  - error code for the error (ignored).
   Return Value:     None.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: None.

   Author:           Glen George
   Last Modified:    March 10, 2011

*/
static void  error_handler(void *arg, err_t err)
{
    /* variables */
      /* none */



    /* inform the system there was a general network error */
    process_error(NETERR_GENERAL);


    /* nothing else to do, return */
    return;

}
