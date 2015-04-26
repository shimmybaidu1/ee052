/****************************************************************************/
/*                                                                          */
/*                                INTERFAC.H                                */
/*                           Interface Definitions                          */
/*                               Include File                               */
/*                          VoIP Telephone Project                          */
/*                                 EE/CS 52                                 */
/*                                                                          */
/****************************************************************************/

/*
   This file contains the constants for interfacing between the C code and
   the assembly code/hardware.  This is a sample interface file to allow test
   compilation and linking of the code.


   Revision History:
      6/3/06   Glen George       Initial revision.
      6/7/06   Glen George       Added ETHER_INTF definition.
      3/8/11   Glen George       Added MAC_ADDR_H and MAC_ADDR_L definitions.
*/



#ifndef  I__INTERFAC_H__
    #define  I__INTERFAC_H__


/* library include files */
  /* none */

/* local include files */
  /* none */




#define  DRAM_START      0x40000000
#define  MEMORY_SIZE     32
#define  SAMPLE_RATE     8000

#define  MAC_ADDR_H      0xEA09
#define  MAC_ADDR_L      0x87654321

#define  ETHER_INTF      "et0"

#define  KEY_0           0
#define  KEY_1           1
#define  KEY_2           2
#define  KEY_3           3
#define  KEY_4           4
#define  KEY_5           5
#define  KEY_6           6
#define  KEY_7           7
#define  KEY_8           8
#define  KEY_9           9
#define  KEY_ESC         10
#define  KEY_BACKSPACE   11
#define  KEY_SEND        12
#define  KEY_OFFHOOK     13
#define  KEY_ONHOOK      14
#define  KEY_SET_IP      15
#define  KEY_SET_SUBNET  16
#define  KEY_SET_GATEWAY 17
#define  KEY_MEM_SAVE    18
#define  KEY_MEM_RECALL  19
#define  KEY_ILLEGAL     20

#define  STATUS_IDLE         0
#define  STATUS_OFFHOOK      1
#define  STATUS_RINGING      2
#define  STATUS_CONNECTING   3
#define  STATUS_CONNECTED    4
#define  STATUS_SET_IP       5
#define  STATUS_SET_SUBNET   6
#define  STATUS_SET_GATEWAY  7
#define  STATUS_MEM_SAVE     8
#define  STATUS_MEM_RECALL   9
#define  STATUS_RECALLED     10
#define  STATUS_ILLEGAL      11

#define  AUDIO_BUFLEN    256


#endif
