/****************************************************************************/
/*                                                                          */
/*                                  IPPROC                                  */
/*                    IP Address Key Processing Functions                   */
/*                          VoIP Telephone Project                          */
/*                                 EE/CS 52                                 */
/*                                                                          */
/****************************************************************************/

/*
   This file contains the key processing functions for IP addresses for the
   VoIP Telephone Project.  These functions are called by the main loop of
   the system.  The functions included are:
      add_IPDigit   - add a decimal digit to the input IP address
      clear_IPAddr  - clear the input IP address
      del_IPDigit   - delete the last input IP address decimal digit
      set_gateway   - set the gateway address
      set_IP        - set the IP address
      set_subnet    - set the subnet mask
      start_IPEntry - start entering an IP address

   The local functions included are:
      clr_IP_address - clears the IP address

   The locally global (shared) variable definitions included are:
      IP_address   - the current value of the IP address
      IP_digit_cnt - number of decimal digits input to the IP address


   Revision History
      6/3/06   Glen George       Initial revision.
      6/6/06   Glen George       Changed start_IPEntry to output the current
	                         ethernet address (all 0's).
      6/7/06   Glen George       Changed clear_IPAddr to handle clearing a
	                         recalled address correctly.
      6/7/06   Glen George       Updated del_IPDigit to work better with a
	                         recalled address (i.e. don't clear it out).
      6/8/06   Glen George       Added code to actually set the IP address,
                                 gateway, and subnet mask.
      5/26/08  Glen George       Only do actual IP calls if NO_LWIP is not
                                 defined.
      6/13/08  Glen George       Fixed a number of compiler errors.
      3/10/11  Glen George       Updated code so any time the ethernet
                                 interface is changed (changing the gateway
				 for example) the connection is restarted.
*/



/* library include files */
#ifndef  NO_LWIP		/* don't include files if not using LWIP */
  #include  "lwip/netif.h"
  #include  "lwip/ip_addr.h"
#endif

/* local include files */
#include  "interfac.h"
#include  "voipdefs.h"
#include  "keyproc.h"
#include  "error.h"
#include  "callutil.h"
#include  "tcpconn.h"




/* local function declarations */
static void  clr_IP_address(void);




/* locally global (shared) variables */
static unsigned long int  IP_address;	/* the current IP address */
static int                IP_digit_cnt;	/* the number of IP address digits input */




/*
   start_IPEntry

   Description:      This function handles the start of entering an IP
                     address.  It first calls a function to clear the IP
                     address, then sets the new system status based on the
                     passed key value and returns that status.

   Arguments:        cur_status (enum status) - the current system status.
                     key_value (int)          - value of the input key.
   Return Value:     (enum status) - the new status.

   Input:            None.
   Output:           None.

   Error Handling:   If there is an unexpected passed key value, the error
                     handler is called with the error UNKNOWN_KEYCODE_INIT.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: None.

   Author:           Glen George
   Last Modified:    June 6, 2006

*/

enum status  start_IPEntry(enum status cur_status, int key_value)
{
    /* variables */
    enum status  new_status;		/* new status to return */



    /* clear the IP address */
    clr_IP_address();


    /* figure out the new system status */
    switch (key_value)  {

        case  KEYCODE_OFFHOOK:	/* Off-Hook key was seen */
				/* new status is off-hook */
				new_status = STAT_OFFHOOK;
				break;

        case  KEYCODE_SETIP:	/* <Set IP> key was seen */
				/* new status is setting IP address */
				new_status = STAT_SETIP;
				break;

        case  KEYCODE_SETSUBNET:/* <Set Subnet> key was seen */
				/* new status is setting subnet mask */
				new_status = STAT_SETSUBNET;
				break;

        case  KEYCODE_SET_GW:	/* <Set Gateway> key was seen */
				/* new status is setting gateway address */
				new_status = STAT_SET_GW;
				break;

        default:		/* some other key was seen */
				/* generate an error and leave status unchanged */
				process_error(UNKNOWN_KEYCODE_INIT);
				new_status = cur_status;
				break;
    }


    /* if got a valid key (changed status) then output the current IP address */
    if (new_status != cur_status)
        display_IP(IP_address);
        

    /* and return the new status */
    return  new_status;

}




/*
   clear_IPAddr

   Description:      This function handles the clear key when an IP address is
                     being input.  It clears the IP address, displays the now
                     cleared IP address, and returns with the system status it
                     was passed, unless it was the recalled state.  If the
		     current status was the recall state then status reverts
		     to the idle state.

   Arguments:        cur_status (enum status) - the current system status.
                     key_value (int)          - value of the key that was
                                                input (ignored).
   Return Value:     (enum status) - the new status, same as current status or
                     the idle state if it was in the recall state.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: None.

   Author:           Glen George
   Last Modified:    June 7, 2006

*/

enum status  clear_IPAddr(enum status cur_status, int key_value)
{
    /* variables */
      /* none */



    /* clear the IP address and digit count */
    clr_IP_address();

    /* update the calling IP address */
    set_calling_IP(IP_address);

    /* display the new IP address */
    display_IP(IP_address);


    /* was there a recalled IP address on the screen */
    if (cur_status == STAT_RECALLED)
        /* if so, need to switch to idle state since don't have that address anymore */
	cur_status = STAT_IDLE;


    /* and return the possibly new status */
    return  cur_status;

}




/*
   add_IPDigit

   Description:      This function handles a digit key when an IP address is
                     being input.  It adds the passed key value to the current
		     value of the IP address.  If there is an overflow (more
		     than 32-bits or a byte value greater than 255) the error
                     handler is called and the key is ignored.  The function
		     returns with the system status it was passed.

   Arguments:        cur_status (enum status) - the current system status.
                     key_value (int)          - value of the input key.
   Return Value:     (enum status) - the new status (same as current status).

   Input:            None.
   Output:           None.

   Error Handling:   If the input causes the IP address to have more than 32
                     bits or if a byte in the address would be greater than
		     256, the error handler is called with the error
		     IP_ADDRESS_OVERFLOW and the key is ignored.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: IP_address (changed)   - updated to new address based on
                                              the passed key value.
                     IP_digit_cnt (changed) - updated to reflect which decimal
		                              digit of the IP address will be
                                              input next.

   Author:           Glen George
   Last Modified:    June 3, 2006

*/

enum status  add_IPDigit(enum status cur_status, int key_value)
{
    /* variables */
    unsigned long int  current_byte;	/* current byte of the IP address */



    /* check if there is room for the digit */
    if (IP_digit_cnt < NUM_IP_DIGITS)  {

        /* have room for the digit - add it in to the current byte */
	/* get the current byte's value so far */
	current_byte = IP_address & (0xFF000000L >> (8 * (IP_digit_cnt / 3)));
	/* shift it down to an 8-bit value */
	current_byte >>= (8 * (3 - (IP_digit_cnt / 3)));

	/* add in the new digit (if possible) */
	if (((10 * current_byte) + key_value) < 256)  {

	    /* it fits - add it in */
	    current_byte = (10 * current_byte) + key_value;

	    /* compute the new IP address value */
	    /* mask off old bits for the current byte */
	    IP_address &= ~(0xFF000000L >> (8 * (IP_digit_cnt / 3)));
	    /* bring in the new value for the byte */
	    IP_address |= (current_byte << (8 * (3 - (IP_digit_cnt / 3))));

	    /* update the digit count */
	    IP_digit_cnt++;
	}
	else  {

	    /* doesn't fit - call the error handler */
	    process_error(IP_ADDRESS_OVERFLOW);
        }
    }
    else  {

        /* too many digits in the IP address - call the error handler */
	process_error(IP_ADDRESS_OVERFLOW);
    }


    /* update the calling IP address */
    set_calling_IP(IP_address);

    /* display the new IP address */
    display_IP(IP_address);


    /* all done adding the digit, return the passed status */
    return  cur_status;

}




/*
   del_IPDigit

   Description:      This function handles a backspace key when an IP address
                     is being input.  It removes the last input decimal digit
		     from the current value of the IP address.  If there are
		     no digits in the IP address, the error handler is called
		     and the key is ignored.  The function returns with the
		     system status it was passed.

   Arguments:        cur_status (enum status) - the current system status.
                     key_value (int)          - value of the key that was
                                                input (ignored).
   Return Value:     (enum status) - the new status (same as current status).

   Input:            None.
   Output:           None.

   Error Handling:   If there are no digits in the IP address to delete, the
		     key is ignored and the error handler is called with the
		     error IP_ADDRESS_UNDERFLOW.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: IP_address (changed)   - updated to new address by
                                              deleting the lowest decimal
					      digit.
                     IP_digit_cnt (changed) - updated to reflect which decimal
		                              digit of the IP address will be
                                              input next.

   Author:           Glen George
   Last Modified:    June 7, 2006

*/

enum status  del_IPDigit(enum status cur_status, int key_value)
{
    /* variables */
    unsigned long int  current_byte;	/* last input byte of the IP address */



    /* are there any digits to delete? */
    if (IP_digit_cnt > 0)  {

        /* looks like there should be a digit to delete */
        /* get the value of the last input byte */
        current_byte = IP_address & (0xFF000000L >> (8 * ((IP_digit_cnt - 1) / 3)));
	/* shift it down to an 8-bit value */
	current_byte >>= (8 * (3 - ((IP_digit_cnt - 1) / 3)));

	/* remove the last digit - just divide by 10 */
	current_byte /= 10;

	/* compute the new IP address value */
	/* mask off old bits for the current byte */
	IP_address &= ~(0xFF000000L >> (8 * ((IP_digit_cnt - 1) / 3)));
	/* bring in the new value for the byte */
	IP_address |= (current_byte << (8 * (3 - ((IP_digit_cnt - 1) / 3))));

	/* update the digit count - there's one less digit now */
	IP_digit_cnt--;

        /* update the calling IP address */
        set_calling_IP(IP_address);

        /* display the new IP address */
        display_IP(IP_address);
    }
    else  {

        /* no digits to delete - call the error handler */
	process_error(IP_ADDRESS_UNDERFLOW);
    }


    /* all done deleting the digit, return the passed status */
    return  cur_status;

}




/*
   set_IP

   Description:      This function handles setting the IP address to the
                     address entered thus far.  The status is always returned
		     as idle.

   Operation:        The TCP/IP stack functions are setup with the current
   		     value of the IP address and then the connection is
		     restarted.

   Arguments:        cur_status (enum status) - the current system status
                                                (ignored).
                     key_value (int)          - value of the key that was
		     			        input (ignored).
   Return Value:     (enum status) - the new status (always STAT_IDLE).

   Input:            None.
   Output:           None.

   Error Handling:   If there is an error getting the named interface (given
                     by ETHER_INTF), the error handler is called with the
		     error UNKNOWN_ETHER_NAME.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: None.

   Author:           Glen George
   Last Modified:    March 10, 2011

*/

enum status  set_IP(enum status cur_status, int key_value)
{
    /* variables */
#ifndef  NO_LWIP		/* empty function if there is no LWIP code */
    struct netif   *net_intf;		/* network interface to access */
    struct ip_addr   ip;			/* the new IP address */



    /* create the IP address in network order */
    ip.addr = htonl(get_calling_IP());


    /* now get the network interface for our device */
    net_intf = netif_find(ETHER_INTF);

    /* check if got an interface */
    if (net_intf != NULL)
        /* got the interface, set the IP address */
	netif_set_ipaddr(net_intf, &ip);
    else
        /* no interface, report the error */
	process_error(UNKNOWN_ETHER_NAME);
#endif


    /* need to restart the connection */
    tcp_connection_restart();


    /* done so return with idle as the status */
    return  STAT_IDLE;

}




/*
   set_gateway

   Description:      This function handles setting the gateway address to the
                     address entered thus far.  The status is always returned
		     as idle.

   Operation:        The function calls the TCP/IP stack functions to set the
                     gateway to the just entered IP address.  Then the TCP
		     connection is restarted.

   Arguments:        cur_status (enum status) - the current system status
                                                (ignored).
                     key_value (int)          - value of the key that was
		     			        input (ignored).
   Return Value:     (enum status) - the new status (always STAT_IDLE).

   Input:            None.
   Output:           None.

   Error Handling:   If there is an error getting the named interface (given
                     by ETHER_INTF), the error handler is called with the
		     error UNKNOWN_ETHER_NAME.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: None.

   Author:           Glen George
   Last Modified:    March 10, 2011

*/

enum status  set_gateway(enum status cur_status, int key_value)
{
    /* variables */
#ifndef  NO_LWIP		/* empty function if there is no LWIP code */
    struct netif    *net_intf;		/* network interface to access */
    struct ip_addr   gw;		/* the new gateway address */



    /* create the gateway address in network order */
    gw.addr = htonl(get_calling_IP());


    /* now get the network interface for our device */
    net_intf = netif_find(ETHER_INTF);

    /* check if got an interface */
    if (net_intf != NULL)
        /* got the interface, set the gateway address */
	netif_set_gw(net_intf, &gw);
    else
        /* no interface, report the error */
	process_error(UNKNOWN_ETHER_NAME);
#endif


    /* need to restart the connection */
    tcp_connection_restart();


    /* done so return with idle as the status */
    return  STAT_IDLE;

}




/*
   set_subnet

   Description:      This function handles setting the subnet mask to the
                     address entered thus far.  The status is always returned
		     as idle.

   Operation:        The function calls the TCP/IP stack functions to set the
                     subnet mask to the just entered IP address.  Then the TCP
		     connection is restarted.

   Arguments:        cur_status (enum status) - the current system status
                                                (ignored).
                     key_value (int)          - value of the key that was
		     			        input (ignored).
   Return Value:     (enum status) - the new status (always STAT_IDLE).

   Input:            None.
   Output:           None.

   Error Handling:   If there is an error getting the named interface (given
                     by ETHER_INTF), the error handler is called with the
		     error UNKNOWN_ETHER_NAME.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: None.

   Author:           Glen George
   Last Modified:    March 10, 2011

*/

enum status  set_subnet(enum status cur_status, int key_value)
{
    /* variables */
#ifndef  NO_LWIP		/* empty function if there is no LWIP code */
    struct netif    *net_intf;		/* network interface to access */
    struct ip_addr   mask;		/* the new subnet mask */



    /* create the subnet mask in network order */
    mask.addr = htonl(get_calling_IP());


    /* now get the network interface for our device */
    net_intf = netif_find(ETHER_INTF);

    /* check if got an interface */
    if (net_intf != NULL)
        /* got the interface, set the subnet mask */
	netif_set_netmask(net_intf, &mask);
    else
        /* no interface, report the error */
	process_error(UNKNOWN_ETHER_NAME);
#endif


    /* need to restart the connection */
    tcp_connection_restart();


    /* done so return with idle as the status */
    return  STAT_IDLE;

}




/* local helper functions */


/*
   clr_IP_address

   Description:      This function clears the IP address and digit count,

   Arguments:        None.
   Return Value:     None.

   Input:            None.
   Output:           None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Shared Variables: IP_address (changed)   - reset to 0.
                     IP_digit_cnt (changed) - reset to 0.

   Author:           Glen George
   Last Modified:    June 3, 2006

*/

static void  clr_IP_address()
{
    /* variables */
      /* none */



    /* clear the IP address and digit count */
    IP_address = 0;
    IP_digit_cnt = 0;


    /* and return */
    return;

}




