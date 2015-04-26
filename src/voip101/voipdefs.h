/****************************************************************************/
/*                                                                          */
/*                                VOIPDEFS.H                                */
/*                           General Definitions                            */
/*                               Include File                               */
/*                          VoIP Telphone Project                           */
/*                                EE/CS  52                                 */
/*                                                                          */
/****************************************************************************/

/*
   This file contains the general definitions for the VoIP Telephone.  This
   includes constant and structure definitions along with the function
   declarations for the assembly language functions.


   Revision History:
      6/3/06   Glen George       Initial revision.
      6/5/06   Glen George       Removed the "far" keyword from the pointers
	                         in the assembly language function
			         declarations.
      6/6/06   Glen George       Removed buffer structure definition.
      6/6/06   Glen George       Fixed missing declaration of update_tx.
      6/9/09   Glen George       Added declarations for networking functions.
      6/12/09  Glen George       Fixed minor compiler error.
      2/28/11  Glen George       Updated prototypes for call_start, update_rx,
	                         and update_tx to match new specification.
      3/9/11   Glen George       Added some networking constants and removed
	                         the audio constants, they are no longer used.
*/



#ifndef  I__VOIPDEFS_H__
    #define  I__VOIPDEFS_H__


/* library include files */
#ifndef  NO_LWIP		/* don't include files if not using LWIP */
  #include  "lwip/pbuf.h"
#endif

/* local include files */
#include  "interfac.h"




/* constants */

/* general constants */
#define  FALSE       0
#define  TRUE        !FALSE
#define  NULL        (void *) 0


/* IP parameters */
#define  NUM_IP_DIGITS   12	/* number of decimal digits in an IP address */
#define  MAGIC_IP        0x00FF55AA	/* IP address that should not occur */
#define  CALL_LISTEN_PORT  0x4747	/* port to listen for connections */


/* miscellaneous constants */
#define  MAX_NAME_LEN  40	/* maximum length of a caller ID name */




/* structures, unions, and typedefs */

/* status types */
enum status  {  STAT_IDLE,		/* system idle */
		STAT_OFFHOOK,		/* phone is off hook */
		STAT_RINGING,		/* incoming call */
		STAT_CONNECTING,	/* attempting to connect */
		STAT_CONNECTED,		/* connected to remote phone */
		STAT_SETIP,		/* setting the IP address */
		STAT_SETSUBNET,		/* setting the subnet address */
		STAT_SET_GW,		/* setting the gateway address */
		STAT_MEMSAVE,		/* saving an address to memory */
		STAT_MEMRECALL,		/* recalling an address from memory */
		STAT_RECALLED,		/* just recalled an address from memory */
		NUM_STATUS		/* number of status types */
	     };

/* key codes */
enum keycode  {  KEYCODE_0,          /* <0>             */
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
	         KEYCODE_ILLEGAL,    /* other keys      */
		 NUM_KEYCODES        /* number of key codes */
              }; 

/* key types */
enum keytype  {  KEYTYPE_DIGIT,      /* <0> <1> <2> <3> <4> */
                                     /* <5> <6> <7> <8> <9> */
                 KEYTYPE_ESC,        /* <ESC>               */
                 KEYTYPE_BS,         /* <Backspace>         */
                 KEYTYPE_SEND,       /* <Send>              */
                 KEYTYPE_OFFHOOK,    /* Off-Hook            */
                 KEYTYPE_ONHOOK,     /* On-Hook             */
                 KEYTYPE_SETIP,      /* <Set IP>            */
                 KEYTYPE_SETSUBNET,  /* <Set Subnet>        */
                 KEYTYPE_SET_GW,     /* <Set Gateway>       */
                 KEYTYPE_MEMSAVE,    /* <Memory Save>       */
	         KEYTYPE_MEMRECALL,  /* <Memory Recall>     */
	         KEYTYPE_UNKNOWN,    /* unknown key type    */
		 NUM_KEYTYPES        /* number of key types */
              }; 

/* declare the ethernet buffer if not using LWIP code */
#ifdef  NO_LWIP
    struct pbuf  { struct pbuf *next; };
#endif



/* function declarations */

/* update needed functions */
unsigned char  update_rx(short int *);  /* record data update */
unsigned char  update_tx(short int *);  /* play data update */

/* keypad functions */
unsigned char  key_available(void);     /* key is available */
int            getkey(void);            /* get a key */

/* display functions  */
void  display_IP(unsigned long int);        /* display the track time */
void  display_memory_addr(unsigned int);    /* display the track number */
void  display_status(unsigned int);	    /* display the system status */

/* audio functions */
void  call_start(short int *);          /* start playing */
void  call_halt(void);             	/* halt play or record */

/* timing function */
int  elapsed_time(void);

/* networking functions */
char          ether_init(void);
char          ether_transmit(struct pbuf *);
char          ether_rx_available(void);
struct pbuf  *ether_receive(void);


#endif
