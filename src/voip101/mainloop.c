/****************************************************************************/
/*                                                                          */
/*                                 MAINLOOP                                 */
/*                             Main Program Loop                            */
/*                          VoIP Telephone Project                          */
/*                                 EE/CS 52                                 */
/*                                                                          */
/****************************************************************************/

/*
   This file contains the main processing loop (background) for the VoIP
   Telephone Project.  The only global function included is:
      main - background processing loop

   The local functions included are:
      key_lookup - get a key and look up its keycode

   The locally global variable definitions included are:
      none


   Revision History
      6/3/06   Glen George       Initial revision.
      6/7/06   Glen George       Added initialization of the rx/tx buffers.
      6/7/06   Glen George       Added displaying incoming call IP address.
      6/7/06   Glen George       Improved processing of memory recall.
      6/9/09   Glen George       Added support for LWIP.
      3/4/11   Glen George       Added static to key_lookup declaration to
	                         avoid compiler errors/warnings.
      3/8/11   Glen George       Changed TCP timer to run at a fixed rate
	                         instead of a fixed interval.
*/



/* library include files */
#ifndef  NO_LWIP		/* don't include files if not using LWIP */
  #include  "lwip/tcp.h"
#endif

/* local include files */
#include  "interfac.h"
#include  "voipdefs.h"
#include  "keyproc.h"
#include  "callutil.h"
#include  "buffers.h"
#include  "tcpconn.h"

#ifndef  NO_LWIP		/* don't include files if not using LWIP */
  #include  "ethernet.h"
#endif




/* local function declarations */
static enum keycode  key_lookup(int);	/* translate key values into keycodes */




/*
   main

   Description:      This procedure is the main program loop for the VoIP
                     Telephone Recorder.

   Operation:        The function loops getting keys from the keypad,
                     processing those keys as is appropriate.  It also handles
                     updating the display and setting up the buffers for audio
                     input and output.  The loop is table driven.

   Arguments:        None.
   Return Value:     (int) - return code, always 0 (never returns).

   Input:            Keys from the keypad.
   Output:           Status information to the display.

   Error Handling:   Invalid input is ignored.

   Algorithms:       The function is table-driven.  The processing routines
                     for each input are given in tables which are selected
                     based on the context (state) the program is operating in.
   Data Structures:  None.

   Global Variables: None.

   Author:           Glen George
   Last Modified:    March 8, 2009

*/

int  main()
{
    /* variables */
    enum keycode  key;			    /* an input key */

    int           timeout;		    /* ethernet timing information */

    enum status   cur_status = STAT_IDLE;   /* current program status */
    enum status   prev_status = STAT_IDLE;  /* previous program status */

    /* array of status type translations (from enum status to #defines) */
    /* note: the array must match the enum definition order exactly */
    const static unsigned int  xlat_stat[] =
        {  STATUS_IDLE,      	/* system idle */
	   STATUS_OFFHOOK,	/* phone is off hook */
	   STATUS_RINGING,	/* incoming call */
	   STATUS_CONNECTING,	/* attempting to connect */
	   STATUS_CONNECTED,	/* connected to remote phone */
	   STATUS_SET_IP,	/* setting the IP address */
	   STATUS_SET_SUBNET,	/* setting the subnet address */
	   STATUS_SET_GATEWAY,	/* setting the gateway address */
	   STATUS_MEM_SAVE,	/* saving an address to memory */
	   STATUS_MEM_RECALL,	/* recalling an address from memory */
	   STATUS_RECALLED 	/* just recalled an address from memory */
	};

    /* array of key type translations (from enum keycode to enum keytype) */
    /* note: the array must match the enum definition order exactly */
    const static enum keytype  key_type[] =
        {  KEYTYPE_DIGIT,       /* <0>             */
           KEYTYPE_DIGIT,       /* <1>             */
	   KEYTYPE_DIGIT,       /* <2>             */
	   KEYTYPE_DIGIT,       /* <3>             */
	   KEYTYPE_DIGIT,       /* <4>             */
	   KEYTYPE_DIGIT,       /* <5>             */
	   KEYTYPE_DIGIT,       /* <6>             */
	   KEYTYPE_DIGIT,       /* <7>             */
	   KEYTYPE_DIGIT,       /* <8>             */
	   KEYTYPE_DIGIT,       /* <9>             */
	   KEYTYPE_ESC,         /* <ESC>           */
	   KEYTYPE_BS,          /* <Backspace>     */
	   KEYTYPE_SEND,        /* <Send>          */
	   KEYTYPE_OFFHOOK,     /* Off-Hook        */
	   KEYTYPE_ONHOOK,      /* On-Hook         */
	   KEYTYPE_SETIP,       /* <Set IP>        */
	   KEYTYPE_SETSUBNET,   /* <Set Subnet>    */
	   KEYTYPE_SET_GW,      /* <Set Gateway>   */
	   KEYTYPE_MEMSAVE,     /* <Memory Save>   */
	   KEYTYPE_MEMRECALL,   /* <Memory Recall> */
	   KEYTYPE_UNKNOWN      /* other keys      */
	};

    /* array of key value translations (from enum keycode to int) */
    /* note: the array must match the enum definition order exactly */
    const static int  key_value[] =
        {  0,                  /* <0>             */
           1,                  /* <1>             */
	   2,                  /* <2>             */
	   3,                  /* <3>             */
	   4,                  /* <4>             */
	   5,                  /* <5>             */
	   6,                  /* <6>             */
	   7,                  /* <7>             */
	   8,                  /* <8>             */
	   9,                  /* <9>             */
	   KEYCODE_ESC,        /* <ESC>           */
	   KEYCODE_BS,         /* <Backspace>     */
	   KEYCODE_SEND,       /* <Send>          */
	   KEYCODE_OFFHOOK,    /* Off-Hook key    */
	   KEYCODE_ONHOOK,     /* Off-Hook key    */
	   KEYCODE_SETIP,      /* <Set IP>        */
	   KEYCODE_SETSUBNET,  /* <Set Subnet>    */
	   KEYCODE_SET_GW,     /* <Set Gateway>   */
	   KEYCODE_MEMSAVE,    /* <Memory Save>   */
	   KEYCODE_MEMRECALL,  /* <Memory Recall> */
	   0                   /* other keys      */
	};

    /* whether or not numeric keys have priority */
    static const int  numeric_priority[] =
	{
          /*                    Current System Status                   */
          /* idle      off-hook    ringing      connecting  connected   */
             FALSE,    TRUE,       FALSE,       FALSE,      FALSE,
          /* set IP    set subnet  set gateway  mem save    mem recall  */
             TRUE,     TRUE,       TRUE,        TRUE,       TRUE,
          /* recalled                                                   */
             FALSE
        };
	  
    /* key processing functions (one for each system status type and key) */
    static enum status  (* const process_key[NUM_KEYTYPES][NUM_STATUS])(enum status, int) =
        /*                            Current System Status                                                */
        /* idle           off-hook        ringing        connecting     connected              key         */
        /* set IP         set subnet      set gateway    mem save       mem recall             type        */
        /* recalled                                                                                        */
      { {  no_action,     add_IPDigit,    no_action,     no_action,     no_action,        /* Digit         */
           add_IPDigit,   add_IPDigit,    add_IPDigit,   add_memDigit,  add_memDigit,
           no_action                                                                  },
        {  no_action,     clear_IPAddr,   no_action,     no_action,     no_action,        /* Escape        */
           clear_IPAddr,  clear_IPAddr,   clear_IPAddr,  clear_memLoc,  clear_memLoc,
           clear_IPAddr                                                               },
        {  no_action,     del_IPDigit,    no_action,     no_action,     no_action,        /* Backspace     */
           del_IPDigit,   del_IPDigit,    del_IPDigit,   del_memDigit,  del_memDigit,
           no_action                                                                  },
        {  no_action,     do_call,        do_answer,     end_call,      end_call,         /* Send/Enter    */
           set_IP,        set_subnet,     set_gateway,   save_mem,      recall_mem,
           do_call                                                                    },
        {  start_IPEntry, no_action,      do_answer,     no_action,     no_action,        /* Off-Hook      */
           no_action,     no_action,      no_action,     no_action,     no_action,
           start_IPEntry                                                              },
        {  no_action,     end_call,       no_action,     end_call,      end_call,         /* On-Hook       */
           no_action,     no_action,      no_action,     no_action,     no_action,
           no_action                                                                  },
        {  start_IPEntry, no_action,      start_IPEntry, no_action,     no_action,        /* Set IP        */
           reset_input,   no_action,      no_action,     no_action,     no_action,
           set_IP                                                                     },
        {  start_IPEntry, no_action,      start_IPEntry, no_action,     no_action,        /* Set Subnet    */
           no_action,     reset_input,    no_action,     no_action,     no_action,
           set_subnet                                                                 },
        {  start_IPEntry, no_action,      start_IPEntry, no_action,     no_action,        /* Set Gateway   */
           no_action,     no_action,      reset_input,   no_action,     no_action,
           set_gateway                                                                },
        {  start_memLoc,  start_memLoc,   start_memLoc,  start_memLoc,  start_memLoc,     /* Memory Save   */
           no_action,     no_action,      no_action,     reset_input,   no_action,
           start_memLoc                                                               },
        {  start_memLoc,  start_memLoc,   start_memLoc,  no_action,     no_action,        /* Memory Recall */
           no_action,     no_action,      no_action,     no_action,     reset_input,
           start_memLoc                                                               },
        {  no_action,     no_action,      no_action,     no_action,     no_action,        /* unknown key   */
           no_action,     no_action,      no_action,     no_action,     no_action,
           no_action                                                                  } };



    /* first initialize everything */
    init_buffers();		/* initialize the rx/tx buffers */
    init_memory();		/* initialize saved IPs */
    tcp_connection_init();	/* initialize the TCP connection */


    /* initialize the lwIP code if it is being used */
    #ifndef  NO_LWIP
        init_networking();
    #endif

    /* reset timing - reset elapsed timer and no time yet */
    elapsed_time();
    timeout = 0;


    /* display the initial status */
    display_status(xlat_stat[cur_status]);


    /* infinite loop processing input */
    while(TRUE)  {

        /* handle networking status changes */
	if ((cur_status == STAT_IDLE) && incoming_call())  {
	    /* have an incoming call - change to ringing state */
	    cur_status = STAT_RINGING;
	    /* and display the incoming call IP Address */
	    display_IP(get_calling_IP());
	}
	else if ((cur_status == STAT_RINGING) && !incoming_call())  {
	    /* no more incoming call - stop ringing */
	    cur_status = STAT_IDLE;
	}
	else if ((cur_status == STAT_CONNECTING) && call_connected())  {
	    /* have connected to the remote phone */
	    cur_status = STAT_CONNECTED;
	    /* need to start the call */
	    start_call();
	}
	else if (cur_status == STAT_CONNECTED)  {
	    /* we are connected, continue processing the call */
	    process_call();
	}


	/* now check for keypad input */
	if (key_available())  {

	    /* have keypad input - get the key */
	    key = key_lookup(numeric_priority[cur_status]);

	    /* execute processing routine for that key */
	    cur_status = process_key[key_type[key]][cur_status](cur_status, key_value[key]);
	}


        /* finally, if the status has changed - display the new status */
        if (cur_status != prev_status)  {

            /* status has changed - update the status display */
	    display_status(xlat_stat[cur_status]);
        }

        /* always remember the current status for next loop iteration */
        prev_status = cur_status;


	/* see if we need to generate a TCP timer event */
	timeout += elapsed_time();
	/* only worry about generating timer event if using LwIP */
        #ifndef  NO_LWIP
   	    if (timeout >= TCP_TIMEOUT)  {

                /* have a timeout - tell the TCP code */
                tcp_tmr();

	        /* have taken care of a timeout interval now */
	        timeout -= TCP_TIMEOUT;
            }
        #endif


	/* check if need to process a packet */
	if (ether_rx_available())  {

	    /* there is ethernet data available - try to process it */
	    /*    only process it if using lwIP code */
            #ifndef  NO_LWIP
                ethernetif_input();
            #endif
        }
    }


    /* done with main (never should get here), return 0 */
    return  0;

}




/*
   key_lookup

   Description:      This function gets a key from the keypad and translates
                     the raw keycode to an enumerated keycode for the main
                     loop.  The keycode precedence is a function of the passed
		     argument.  If the argument is true, numeric keys have
		     precedence.  This allows the numeric keys to have more
		     than one meaning.

   Operation:        The function calls getkey and then converts the returned
                     raw keycode to an enumerated keycode for the main loop by
                     using one of two translation tables.  Which table to use
		     is determined by the passed argument.

   Arguments:        numeric (int) - true to indicate numeric keys have
                                     precedence.
   Return Value:     (enum keycode) - type of the key input on keypad.

   Input:            Keys from the keypad.
   Output:           None.

   Error Handling:   Invalid keys are returned as KEYCODE_ILLEGAL.

   Algorithms:       The function uses arrays to lookup the key types.
   Data Structures:  Arrays of key types versus key codes.

   Global Variables: None.

   Author:           Glen George
   Last Modified:    June 3, 2006

*/

static  enum keycode  key_lookup(int numeric)
{
    /* variables */

    const static enum keycode  nnkeycodes[] = /* array of keycodes for non-numeric precedence */
        {		                      /* order must match keys array exactly */
	   KEYCODE_ESC,        /* <ESC>           */
	   KEYCODE_BS,         /* <Backspace>     */
	   KEYCODE_SEND,       /* <Send>          */
	   KEYCODE_OFFHOOK,    /* Off-Hook        */
	   KEYCODE_ONHOOK,     /* On-Hook         */
	   KEYCODE_SETIP,      /* <Set IP>        */
	   KEYCODE_SETSUBNET,  /* <Set Subnet>    */
	   KEYCODE_SET_GW,     /* <Set Gateway>   */
	   KEYCODE_MEMSAVE,    /* <Memory Save>   */
	   KEYCODE_MEMRECALL,  /* <Memory Recall> */
           KEYCODE_0,          /* <0>             */
	   KEYCODE_1,          /* <1>             */
	   KEYCODE_2,          /* <2>             */
	   KEYCODE_3,          /* <3>             */
	   KEYCODE_4,          /* <4>             */
	   KEYCODE_5,          /* <5>             */
	   KEYCODE_6,          /* <6>             */
	   KEYCODE_7,          /* <7>             */
	   KEYCODE_8,          /* <8>             */
	   KEYCODE_9,          /* <9>             */
	   KEYCODE_ILLEGAL     /* entry needed for illegal codes */
        }; 

    const static enum keycode  nkeycodes[] = /* array of keycodes for numeric precedence */
        {		                     /* order must match nkeys array exactly */
           KEYCODE_0,          /* <0>             */
	   KEYCODE_1,          /* <1>             */
	   KEYCODE_2,          /* <2>             */
	   KEYCODE_3,          /* <3>             */
	   KEYCODE_4,          /* <4>             */
	   KEYCODE_5,          /* <5>             */
	   KEYCODE_6,          /* <6>             */
	   KEYCODE_7,          /* <7>             */
	   KEYCODE_8,          /* <8>             */
	   KEYCODE_9,          /* <9>             */
	   KEYCODE_ESC,        /* <ESC>           */
	   KEYCODE_BS,         /* <Backspace>     */
	   KEYCODE_SEND,       /* <Send>          */
	   KEYCODE_OFFHOOK,    /* Off-Hook        */
	   KEYCODE_ONHOOK,     /* On-Hook         */
	   KEYCODE_SETIP,      /* <Set IP>        */
	   KEYCODE_SETSUBNET,  /* <Set Subnet>    */
	   KEYCODE_SET_GW,     /* <Set Gateway>   */
	   KEYCODE_MEMSAVE,    /* <Memory Save>   */
	   KEYCODE_MEMRECALL,  /* <Memory Recall> */
	   KEYCODE_ILLEGAL     /* entry needed for illegal codes */
        }; 

    const enum keycode  *keycodes;	/* pointer to appropriate keycode array */

    const static int  nnkeys[] = /* array of key values for non-numeric precedence */
        {			 /* order must match keycodes array exactly */
           KEY_ESC,	       /* <ESC>           */
           KEY_BACKSPACE,      /* <Backspace>     */
           KEY_SEND,	       /* <Send>          */
           KEY_OFFHOOK,	       /* Off-Hook        */
           KEY_ONHOOK,	       /* On-Hook         */
           KEY_SET_IP,	       /* <Set IP>        */
           KEY_SET_SUBNET,     /* <Set Subnet>    */
           KEY_SET_GATEWAY,    /* <Set Gateway>   */
           KEY_MEM_SAVE,       /* <Memory Save>   */
           KEY_MEM_RECALL,     /* <Memory Recall> */
           KEY_0,	       /* <0>             */
           KEY_1,	       /* <1>             */
           KEY_2,	       /* <2>             */
           KEY_3,	       /* <3>             */
           KEY_4,	       /* <4>             */
           KEY_5,	       /* <5>             */
           KEY_6,	       /* <6>             */
           KEY_7,	       /* <7>             */
           KEY_8,	       /* <8>             */
           KEY_9	       /* <9>             */
        }; 

    const static int  nkeys[] =  /* array of key values for numeric precedence */
        {			 /* order must match keycodes array exactly */
           KEY_0,	       /* <0>             */
           KEY_1,	       /* <1>             */
           KEY_2,	       /* <2>             */
           KEY_3,	       /* <3>             */
           KEY_4,	       /* <4>             */
           KEY_5,	       /* <5>             */
           KEY_6,	       /* <6>             */
           KEY_7,	       /* <7>             */
           KEY_8,	       /* <8>             */
           KEY_9,	       /* <9>             */
           KEY_ESC,	       /* <ESC>           */
           KEY_BACKSPACE,      /* <Backspace>     */
           KEY_SEND,	       /* <Send>          */
           KEY_OFFHOOK,	       /* Off-Hook        */
           KEY_ONHOOK,	       /* On-Hook         */
           KEY_SET_IP,	       /* <Set IP>        */
           KEY_SET_SUBNET,     /* <Set Subnet>    */
           KEY_SET_GATEWAY,    /* <Set Gateway>   */
           KEY_MEM_SAVE,       /* <Memory Save>   */
           KEY_MEM_RECALL      /* <Memory Recall> */
        }; 

    const int  *keys;	/* pointer to appropriate key array */


    int  key; 		/* an input key */

    int  i;           	/* general loop index */



    /* figure out which array to use */
    if (numeric)  {
        /* should be using the numeric precedence arrays */
	keycodes = nkeycodes;
	keys = nkeys;
    }
    else  {
        /* should be using the non-numeric precedence arrays */
	keycodes = nnkeycodes;
	keys = nnkeys;
    }


    /* get a key */
    key = getkey();


    /* lookup key in the appropriate keys array */
    /* note that nnkeys[] and nkeys[] should be the same size so can */
    /*    use either in the comparison */
    for (i = 0; ((i < (sizeof(nkeys)/sizeof(int))) && (key != keys[i])); i++);


    /* return the appropriate key type */
    return  keycodes[i];

}
