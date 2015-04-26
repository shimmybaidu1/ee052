/****************************************************************************/
/*                                                                          */
/*                                CALLUTIL                                  */
/*                        Calling Utility Functions                         */
/*                         VoIP Telephone Project                           */
/*                                EE/CS 52                                  */
/*                                                                          */
/****************************************************************************/

/*
   This file contains the utility functions for dealing with calls for the
   VoIP Telephone Project.  The IP number of the "other end" of the call is
   also defined in this file (locally).  The functions included are:
      call_connected    - has the other end connected with us
      connect_incoming  - connect to an incoming call
      disconnect_call   - end a call
      get_calling_IP    - get the calling IP address (accessor)
      get_calling_name  - get the calling name (accessor)
      incoming_call     - is there an incoming call
      initiate_outgoing - initiate an outgoing call
      process_call      - process a continuing call
      set_calling_IP    - set the calling IP address (mutator)
      set_calling_name  - set the calling name (mutator)
      start_call        - start an outgoing call

   The local functions included are:
      ring_fill - fill buffer with a ring tone
      busy_fill - fill buffer with a busy tone
      sine_wave - get the value of the sine function

   The locally global variable definitions included are:
      calling_IP      - the IP number of the other party
      calling_name    - the name of the other party
      last_status     - the lassed status of call (to find changes in status)
      ring_busy_timer - timer for timing ring and busy tones


   Revision History
      6/3/06   Glen George       Initial revision (only dummy versions of the
                                 functions).
      6/6/06   Glen George       Started filling in real functions.
      6/8/06   Glen George       Continued filling in real functions.
      2/28/11  Glen George       Added call to call_halt() in disconnect_call.
      3/10/11  Glen George       Updated code to use TCP functions to
	                         implement actual calling (major changes).
      3/16/11  Glen George       Fixed some bugs in the ring and busy tone
	                         generation and changed the ring tone to 500
				 Hz modulated by 20 Hz.
*/



/* library include files */
  /* none */

/* local include files */
#include  "interfac.h"
#include  "voipdefs.h"
#include  "callutil.h"
#include  "buffers.h"
#include  "tcpconn.h"
#include  "error.h"




/* local function declarations */
static void  ring_fill(short int *p, int size);	/* fill buffer w/ring tone */
static void  busy_fill(short int *p, int size); /* fill buffer w/busy tone */
static int   sine_wave(long int angle);		/* compute sine function */




/* locally global variables */

/* IP address of the "other end" */
static unsigned long int  calling_IP;

/* name of the "other end" */
static char  calling_name[MAX_NAME_LEN];

/* the lagged status of the call (used to check for changes) */
static enum tcp_conn_status  last_status;

/* ring/busy timer */
static unsigned long int  ring_busy_timer;




/* mutators/accessors */


/*
   get_calling_IP

   Description:      This function returns the IP address of the "other end"
   		     of the call.

   Operation:        The value of the shared variable calling_IP is returned.

   Arguments:        None.
   Return Value:     (unsigned long int) - the IP address of the "other end"
                        of the call, either who is calling or who was called.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: calling_IP (accessed) - value to return.

   Author:           Glen George
   Last Modified:    June 3, 2006

*/

unsigned long int  get_calling_IP()
{
    /* variables */
      /* none */



    /* just return the value of calling_IP */
    return  calling_IP;

}




/*
   get_calling_name

   Description:      This function returns the name of the "other end" of the
   		     call.

   Operation:        The value of the shared variable calling_name is
   		     returned.

   Arguments:        None.
   Return Value:     (const char *) - the name of the "other end" of the call,
                        either who is calling or who was called.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: calling_name (accessed) - value to return.

   Author:           Glen George
   Last Modified:    March 8, 2011

*/

const char  *get_calling_name()
{
    /* variables */
      /* none */



    /* just return a pointer to calling_name */
    return  calling_name;

}




/*
   set_calling_IP

   Description:      This function sets the IP address of the "other end" of
   		     the call.

   Operation:        The shared variable calling_IP is set to the passed
                     value.

   Arguments:        ip (unsigned long int) - the new value of the IP address
                                              of the "other end" of the call,
					      either who is calling or who was
					      called.
   Return Value:     None.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: calling_IP (changed) - changed to passed value.

   Author:           Glen George
   Last Modified:    June 3, 2006

*/

void  set_calling_IP(unsigned long int ip)
{
    /* variables */
      /* none */



    /* set the calling IP address */
    calling_IP = ip;


    /* done - return */
    return;

}




/*
   set_calling_name

   Description:      This function sets the name of the "other end" of the
   		     call to a copy of the passed value.

   Operation:        The shared variable calling_name is set to the passed
                     value.  The passed string is copied character by
		     character with care taken to not overwrite the buffer.

   Arguments:        name (const char *) - pointer to the new value for the
                                           name of the "other end" of the
					   call, either who is calling or who
					   was called.
   Return Value:     None.

   Input:            None.
   Output:           None.

   Error Handling:   If the passed string is longer than the buffer for the
                     name, it is truncated.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: calling_name (changed) - changed to passed value.

   Author:           Glen George
   Last Modified:    March 8, 2011

*/

void  set_calling_name(const char *name)
{
    /* variables */
    int  i;		/* general loop index */



    /* copy the passed string up to the end of the string or maximum length */
    for (i = 0; ((i < (MAX_NAME_LEN - 1)) && (name[i] != '\0')); i++)
        calling_name[i] = name[i];

    /* <null> terminate the string */
    calling_name[i] = '\0';


    /* done copying the name, return */
    return;

}




/* status functions */


/*
   incoming_call

   Description:      This function determines whether or not there is an
                     incoming call.  If someone is trying to connect with this
		     phone, TRUE is returned.

   Operation:        The result of calling have_tcp_connection() is returned.

   Arguments:        None.
   Return Value:     (char) -  TRUE is someone is trying to connect with us,
   		     FALSE otherwise.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: None.

   Author:           Glen George
   Last Modified:    March 8, 2011

*/

char  incoming_call()
{
    /* variables */
      /* none */



    /* return whether or not there is a call */
    return  have_tcp_connection();

}




/*
   call_connected

   Description:      This function determines whether or not the "other end"
                     has connected with us.  If the connection has been
		     established TRUE is returned.

   Operation:        The connection status is queried and TRUE is returned if
                     there is a connection.

   Arguments:        None.
   Return Value:     (char) -  TRUE if the connection has been established,
   		     FALSE otherwise.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: None.

   Author:           Glen George
   Last Modified:    March 9, 2011

*/

char  call_connected()
{
    /* variables */
      /* none */



    /* return whether or not have a connection */    
    return  (tcp_connection_status() != CALL_NO_CONNECTION);

}




/* call management functions */


/*
   initiate_outgoing

   Description:      This function initiates an outgoing call to the currently
                     set IP address.

   Operation:        An attempt is made to connect to calling_IP.

   Arguments:        None.
   Return Value:     None.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: calling_IP (accessed) - IP to connect to.

   Author:           Glen George
   Last Modified:    March 9, 2011

*/

void  initiate_outgoing()
{
    /* variables */
      /* none */



    /* start trying to get a connection and return */
    tcp_connection_connect(calling_IP);
    return;

}




/*
   start_call

   Description:      This function starts an outgoing call.

   Operation:        The buffers are initialized, the call state is set to
                     connected, the ring and busy tones are started and the
                     call is started.

   Arguments:        None.
   Return Value:     None.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: last_status (changed)     - set to CALL_NO_CONNECTION.
                     ring_busy_timer (changed) - set to 0.

   Author:           Glen George
   Last Modified:    March 9, 2011

*/

void  start_call()
{
    /* variables */
      /* none */



    /* reset the buffers for a new call */
    reset_rx_buffer();
    reset_tx_buffer();


    /* start the audio portion of the call */
    /*    Note: buffer had better be available now */
    call_start(get_rx_buffer());


    /* the prior call status is that the call is not connected yet */
    last_status = CALL_NO_CONNECTION;


    /* the ring and busy timer starts over for those tones */
    ring_busy_timer = 0;


    /* done setting up the call, return */
    return;

}




/*
   connect_incoming

   Description:      This function connects to an incoming call.

   Operation:        It calls the TCP connection function to answer the call.
                     It then initializes the buffer and audio code to start
		     the call.

   Arguments:        None.
   Return Value:     None.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: last_status (changed) - set to CALL_NO_CONNECTION.

   Author:           Glen George
   Last Modified:    March 9, 2011

*/

void  connect_incoming()
{
    /* variables */
      /* none */



    /* answer the call */
    tcp_connection_answer();


    /* reset the buffers for a new call */
    reset_rx_buffer();
    reset_tx_buffer();


    /* start the audio portion of the call */
    /*    Note: buffer had better be available now */
    call_start(get_rx_buffer());


    /* the prior call status is that the call is not connected yet */
    last_status = CALL_NO_CONNECTION;


    /* done setting up the call, return */
    return;

}




/*
   process_call

   Description:      This function processes a continuing call.  If there are
                     buffers to play and the update function is ready, a
		     buffer is passed.  If there are buffers to fill and the
		     update function is ready, a buffer is passed.  The
		     ethernet interface is also checked for buffers.

   Arguments:        None.
   Return Value:     None.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: None.

   Author:           Glen George
   Last Modified:    March 9, 2011

*/

void  process_call()
{
    /* variables */
    enum tcp_conn_status  cur_status;	/* current connection state */



    /* get the current connection state */
    cur_status = tcp_connection_status();

    /* if the current connection state has changed, need to reset the */
    /*    the receive buffers, they may have old tone data in them */
    if (cur_status != last_status)
        reset_tx_buffer();

    /* always update the last status */
    last_status = cur_status;


    /* now check the status of the connection */
    switch  (cur_status)  {

        case CALL_RINGING:	/* connection is ringing on the other end */

				/* generate a buffer of ringing tone if */
				/*    there is room for it */
				if (rcv_available())  {
				    /* have a buffer, get and fill it */
				    ring_fill(get_rcv_buffer(), AUDIO_BUFLEN);
				}

				/* if there is a buffer to transmit, need */
                                /*    to discard it */
				if (xmit_available())  {
				    /* have a buffer, dump it */
				    get_xmit_buffer();
				}
				break;

        case CALL_BUSY:		/* connection is busy on the other end */

				/* generate a buffer of busy signal if */
				/*    there is room for it */
				if (rcv_available())  {
				    /* have a buffer, get and fill it */
				    busy_fill(get_rcv_buffer(), AUDIO_BUFLEN);
				}

				/* if there is a buffer to transmit, need */
                                /*    to discard it */
				if (xmit_available())  {
				    /* have a buffer, dump it */
				    get_xmit_buffer();
				}
				break;

        case CALL_CONNECTED:	/* are talking on the connection */

			        /* check if there is a buffer to transmit */
				if (xmit_available())  {
				    /* buffer is available - try sending it */
				    if (tcp_connection_tx(get_xmit_next_ptr(), AUDIO_BUFLEN))
				        /* actually sent it, let buffer functions know */
				        get_xmit_buffer();
				}

				/* check if have room for a received buffer */
				if (rcv_available())  {
				    /* have room - try to get a buffer */
				    if (tcp_connection_rx(get_rcv_next_ptr(), AUDIO_BUFLEN))
				        /* got the buffer, so allocate it */
					get_rcv_buffer();
				}
				break;

	default:		/* some other status, must have aborted call */

				/* if there is a buffer to transmit, need */
                                /*    to discard it */
				if (xmit_available())  {
				    /* have a buffer, dump it */
				    get_xmit_buffer();
				}
				break;
    }


    /* try to play and receive any buffers */

    /* check if a receive buffer is available for recording into */
    if (rx_available())  {

        /* have a receive buffer, see if update is ready */
	if (update_rx(get_rx_next_ptr()))
	    /* it wanted the buffer - actually allocate it */
	    get_rx_buffer();
    }

    /* check if a transmit buffer is available for playing */
    if (tx_available())  {

        /* have a transmit buffer, see if update is ready */
	if (update_tx(get_tx_next_ptr()))
	    /* it wanted the buffer - actually allocate it */
	    get_tx_buffer();
    }


    /* all done processing the call for now - return */
    return;

}




/*
   disconnect_call

   Description:      This function ends a call.

   Operation:        The TCP connection is closed and the audio I/O is halted.

   Arguments:        None.
   Return Value:     None.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: None.

   Author:           Glen George
   Last Modified:    March 9, 2011

*/

void  disconnect_call()
{
    /* variables */
      /* none */



    /* close the TCP connection */
    tcp_connection_close();

    /* and halt the call */
    call_halt();


    /* done ending the call - return */
    return;

}




/* ring and busy tone generators */


/*
   ring_fill

   Description:      This function fills the passed buffer of the passed size
                     with a ring tone.  A ring tone is a 500 Hz signal
                     modulated by a 20 Hz signal at -13 dBm.  The tone is on
		     for 2 seconds and off for 4 seconds.

   Operation:        A static shared variable is used to keep track of the
                     position in the waveform pattern when the function is
		     called.  The function computes the the waveform pattern
		     and writes it to the buffer.

   Arguments:        p (short int *) - pointer to buffer to be filled with
	                               data.
                     size (int)      - size of the passed buffer.
   Return Value:     None.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: ring_busy_timer (changed) - updated on each call.

   Author:           Glen George
   Last Modified:    March 16, 2011

*/

static void  ring_fill(short int *p, int size)
{
    /* variables */
    long int  sig_500;		/* value of the 420 Hz tone signal */
    long int  sig_20;		/* value of the 40 Hz modulating signal */

    long int  angle;		/* angle to find the sine for */

    int       i;		/* general loop index */



    /* fill the buffer with data */
    for (i = 0; i < size; i++)  {

        /* check if past the end of the tone + silent portion */
	if (++ring_busy_timer >= (RING_TONE_TIME + RING_SILENT_TIME))
	    /* timer has wrapped, reset it */
	    ring_busy_timer = 0;

	/* get the angle for both waveforms (with a factor of 25) */
	/* calculation is complicated to keep it from overflowing */
	angle = (((500 * ring_busy_timer) / SAMPLE_RATE) * SINE_RESOLUTION) +
		(((500 * ring_busy_timer) % SAMPLE_RATE) * SINE_RESOLUTION) / SAMPLE_RATE;

	/* get the 500 Hz sine wave */
	sig_500 = sine_wave(angle);

	/* now get the 20 Hz modulation wave (25 = 500 Hz / 20 Hz) */
	sig_20 = sine_wave(angle / 25);
	/* the modulation signal should always be positive */
	if (sig_20 < 0)
	    sig_20 = -sig_20;


	/* should the tone be output or should it be silent */
	if (ring_busy_timer < RING_TONE_TIME)
	    /* outputting the tone (make it -13 dBm) */
	    p[i] = (sig_500 * sig_20) / (4096 * 4);
	else
	    /* output no tone - store a 0 */
	    p[i] = 0;
    }


    /* all done filling the buffer - return */
    return;

}




/*
   busy_fill

   Description:      This function fills the passed buffer of the passed size
                     with a busy tone.  A busy tone is a 600 Hz signal
                     modulated by a 120 Hz signal at -13 dBm.  The tone is on
		     for 0.5 seconds and off for 0.5 seconds.

   Operation:        A static shared variable is used to keep track of the
                     position in the waveform pattern when the function is
		     called.  The function computes the the waveform pattern
		     and writes it to the buffer.

   Arguments:        p (short int *) - pointer to buffer to be filled with
	                               data.
                     size (int)      - size of the passed buffer.
   Return Value:     None.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: ring_busy_timer (changed) - updated on each call.

   Author:           Glen George
   Last Modified:    March 16, 2011

*/

static void  busy_fill(short int *p, int size)
{
    /* variables */
    long int  sig_600;		/* value of the 600 Hz tone signal */
    long int  sig_120;		/* value of the 120 Hz modulating signal */

    long int  angle;		/* angle to find the sine for */

    int       i;		/* general loop index */



    /* fill the buffer with data */
    for (i = 0; i < size; i++)  {

        /* check if past the end of the tone + silent portion */
	if (++ring_busy_timer >= (BUSY_TONE_TIME + BUSY_SILENT_TIME))
	    /* timer has wrapped, reset it */
	    ring_busy_timer = 0;

	/* get the angle for both waveforms (with a factor of 5) */
	/* calculation is complicated to keep it from overflowing */
	angle = (((600 * ring_busy_timer) / SAMPLE_RATE) * SINE_RESOLUTION) +
                (((600 * ring_busy_timer) % SAMPLE_RATE) * SINE_RESOLUTION) / SAMPLE_RATE;

	/* get the 600 Hz sine wave */
	sig_600 = sine_wave(angle);

	/* now get the 120 Hz modulation wave (5 = 600 Hz / 120 Hz) */
	sig_120 = sine_wave(angle / 5);
	/* the modulation signal should always be positive */
	if (sig_120 < 0)
	    sig_120 = -sig_120;


	/* should the tone be output or should it be silent */
	if (ring_busy_timer < BUSY_TONE_TIME)
	    /* outputting the tone (make it -13 dBm) */
	    p[i] = (sig_600 * sig_120) / (4096 * 4);
	else
	    /* output no tone - store a 0 */
	    p[i] = 0;
    }


    /* all done filling the buffer - return */
    return;

}




/*
   sine_wave

   Description:      This function returns the value of a sine wave for the
                     passed angle (units of 360/1024 degrees).  The returned
                     value is 13-bits.

   Operation:        A table is used to lookup the sine wave.  The table only
                     covers one quadrant of the sine wave so the argument is
		     first checked to see which quadrant it is in.

   Arguments:        angle (long int) - angle (units of 360/SIN_RESOLUTION
	                                degrees) for which to find the value
	                                of the sine function.
   Return Value:     (int) - value of the sine function for the passed angle
                        with the maximum amplitude being +/- 4096.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: None.

   Author:           Glen George
   Last Modified:    March 16, 2011

*/

static int  sine_wave(long int angle)
{
    /* variables */

    /* samples of one fourth of a cycle of a sine wave */
    static const short int  sin_wave[SINE_RESOLUTION / 4] =  {  
           0,     25,     50,     75,    100,    126,    151,    176,   
         201,    226,    251,    276,    301,    326,    351,    376,   
         401,    426,    451,    476,    501,    526,    551,    576,   
         601,    626,    651,    675,    700,    725,    750,    774,   
         799,    824,    848,    873,    897,    922,    946,    971,   
	 995,   1019,   1044,   1068,   1092,   1116,   1141,   1165,   
	1189,   1213,   1237,   1261,   1285,   1308,   1332,   1356,   
	1380,   1403,   1427,   1450,   1474,   1497,   1521,   1544,   
	1567,   1590,   1613,   1636,   1659,   1682,   1705,   1728,   
	1751,   1773,   1796,   1819,   1841,   1864,   1886,   1908,   
	1930,   1952,   1975,   1996,   2018,   2040,   2062,   2084,   
	2105,   2127,   2148,   2170,   2191,   2212,   2233,   2254,   
	2275,   2296,   2317,   2337,   2358,   2378,   2399,   2419,   
	2439,   2459,   2480,   2499,   2519,   2539,   2559,   2578,   
	2598,   2617,   2636,   2656,   2675,   2694,   2713,   2731,   
	2750,   2769,   2787,   2805,   2824,   2842,   2860,   2878,   
	2896,   2913,   2931,   2948,   2966,   2983,   3000,   3017,   
	3034,   3051,   3068,   3084,   3101,   3117,   3133,   3149,   
	3165,   3181,   3197,   3213,   3228,   3244,   3259,   3274,   
	3289,   3304,   3319,   3333,   3348,   3362,   3377,   3391,   
	3405,   3419,   3433,   3446,   3460,   3473,   3486,   3499,   
	3512,   3525,   3538,   3551,   3563,   3575,   3587,   3600,   
	3611,   3623,   3635,   3646,   3658,   3669,   3680,   3691,   
	3702,   3712,   3723,   3733,   3744,   3754,   3764,   3774,   
	3783,   3793,   3802,   3811,   3821,   3830,   3838,   3847,   
	3856,   3864,   3872,   3880,   3888,   3896,   3904,   3911,   
	3919,   3926,   3933,   3940,   3947,   3953,   3960,   3966,   
	3972,   3978,   3984,   3990,   3995,   4001,   4006,   4011,   
	4016,   4021,   4026,   4030,   4035,   4039,   4043,   4047,   
	4051,   4054,   4058,   4061,   4064,   4067,   4070,   4073,   
	4075,   4078,   4080,   4082,   4084,   4086,   4087,   4089,   
	4090,   4091,   4092,   4093,   4094,   4094,   4095,   4095
    };

    int  sign;		/* sign of the sine wave, based on quadrant */

    int  index;		/* index into the table */



    /* reduce the angle to one cycle */
    angle %= SINE_RESOLUTION;
    /* and make sure it is positive */
    if (angle < 0)
        angle += SINE_RESOLUTION;

    /* find the sign of the sine wave */
    if (angle < (SINE_RESOLUTION / 2))
        /* first two quadrants are positive */
	sign = +1;
    else
        /* third and fourth quadrants are negative */
	sign = -1;


    /* lastly get the table index */
    /* note: there is only one quadrant in the table so the index is a */
    /*       function of which quadrant we are in */
    if (((angle / (SINE_RESOLUTION / 4)) & 0x01) == 0)
        /* in quadrant I or III, go through table in normal order */
	index = angle % (SINE_RESOLUTION / 4);
    else
        /* in quadrant II or IV, go through table in reverse order */
	index = (SINE_RESOLUTION / 4) - angle % (SINE_RESOLUTION / 4) - 1;


    /* now return the sine function (only have one quadrant of function */
    return  (sign * sin_wave[index]);

}
