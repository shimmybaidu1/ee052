/****************************************************************************/
/*                                                                          */
/*                               CALLUTIL.H                                 */
/*                        Calling Utility Functions                         */
/*                              Include File                                */
/*                         VoIP Telephone Project                           */
/*                                EE/CS 52                                  */
/*                                                                          */
/****************************************************************************/

/*
   This file contains the constants and function prototypes for the calling
   utility functions for the VoIP Telephone Project which are defined in
   callutil.c and memproc.c.


   Revision History
      6/3/06   Glen George       Initial revision.
      3/9/11   Glen George       Made numerous changes to fully support making
	                         actual phone calls.
*/




#ifndef  I__CALLUTIL_H__
    #define  I__CALLUTIL_H__


/* library include files */
  /* none */

/* local include files */
  /* none */




/* constants */

/* resolution of sine calculation (points per 360 degrees) */
#define  SINE_RESOLUTION   1024

/* length of the ring tone in sample rate units (2 seconds) */
#define  RING_TONE_TIME    (2 * SAMPLE_RATE)

/* length of the ring silent period in sample rate units (4 seconds) */
#define  RING_SILENT_TIME  (4 * SAMPLE_RATE)

/* length of the busy tone period in sample rate units (0.5 seconds) */
#define  BUSY_TONE_TIME    (SAMPLE_RATE / 2)

/* length of the busy silent time in sample rate units (0.5 seconds) */
#define  BUSY_SILENT_TIME  (SAMPLE_RATE / 2)




/* structures, unions, and typedefs */
    /* none */




/* function declarations */

/* call status functions */
char  incoming_call(void);	/* there is an incoming call */
char  call_connected(void);	/* other end has connected with us */

/* call management functions */
void  initiate_outgoing(void);	/* initiate an outgoing call */
void  start_call(void);		/* start an outgoing call */
void  connect_incoming(void);	/* connect to an incoming call */
void  process_call(void);	/* process a continuing call */
void  disconnect_call(void);	/* end a call */

/* IP accessor/mutator functions */
unsigned long int   get_calling_IP(void);
const char         *get_calling_name(void);
void                set_calling_IP(unsigned long int ip);
void                set_calling_name(const char *name);

/* memory functions */
void  init_memory(void);	/* initialize the IP address memory system */


#endif
