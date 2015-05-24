@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@                                                                            @
@ system.inc                                                                 @
@                                                                            @
@ Include file for the system hardware definitions for the EE/CS 52 VoIP     @
@ phone project.  This file should contain all of the hardware specific      @
@ definitions (chip select register values, control words, etc).             @
@                                                                            @
@ Revision History:                                                          @
@                                                                            @
@   2012/01/29  Glen George     Initial revision.                            @
@                                                                            @
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@



@ Stack definitions

@ top of the stack, usually the end of the external SRAM
	.equ	TOP_STACK, 0
   
@ interrupt stack size (see chapter 13 of the manual)
        .equ	IRQ_STACK_SIZE, 0

@ supervisor mode stack size (see chapter 13 of the manual)
	.equ	SVC_STACK_SIZE,	0
