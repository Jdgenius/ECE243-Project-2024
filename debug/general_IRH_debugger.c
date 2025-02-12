//2024.03.30.AE -> Modified Version

//Author: Intel FPGA (DE1-SoC Computer System with Nios® II)
//Modified by Arnold Etta


/*******************************************************************
 *	Combines all the necessary files required to run C interrupts  *
 *	on CPULator. Simply modify ISR, IH & main functions where fit. *
 *******************************************************************/

#ifndef __NIOS2_CTRL_REG_MACROS__
#define __NIOS2_CTRL_REG_MACROS__
	
/*****************************************************************************/
/* Macros for accessing the control registers. 								 */
/*****************************************************************************/
	
	#define NIOS2_READ_STATUS(dest) \
		do { dest = __builtin_rdctl(0); } while (0)
	#define NIOS2_WRITE_STATUS(src) \
		do { __builtin_wrctl(0, src); } while (0)
	#define NIOS2_READ_ESTATUS(dest) \
		do { dest = __builtin_rdctl(1); } while (0)
	#define NIOS2_READ_BSTATUS(dest) \
		do { dest = __builtin_rdctl(2); } while (0)
	#define NIOS2_READ_IENABLE(dest) \
		do { dest = __builtin_rdctl(3); } while (0)
	#define NIOS2_WRITE_IENABLE(src) \
		do { __builtin_wrctl(3, src); } while (0)
	#define NIOS2_READ_IPENDING(dest) \
		do { dest = __builtin_rdctl(4); } while (0)
	#define NIOS2_READ_CPUID(dest) \
		do { dest = __builtin_rdctl(5); } while (0)
#endif


#define TIMER_BASE ((volatile long *) 0xFF202000)
#define LED_BASE ((volatile long *) 0xFF200000)
#define SW_BASE ((volatile long *) 0xFF200040)
#define KEY_BASE ((volatile long *) 0xFF200050)
volatile int pattern, shift_dir, shift_enable;
int timeElapsed = 0;

/* function prototypes */
//void main(void);
void interrupt_handler(void);
void interval_timer_ISR(void);
void pushbutton_ISR(void);

volatile int pattern = 0x0000000F; // pattern for shifting
volatile int shift_dir = 'LEFT'; // direction to shift the pattern
volatile int shift_enable = 'ENABLE'; // enable/disable shifting of the pattern
/*******************************************************************************
* This program demonstrates use of interrupts. It
* first starts the interval timer with 50 msec timeouts, and then enables
* Nios II interrupts from the interval timer and pushbutton KEYs
*
* The interrupt service routine for the interval timer displays a pattern on
* the LED lights, and shifts this pattern either left or right. The shifting
* direction is reversed when KEY[1] is pressed
********************************************************************************/
int main(void) 
{
	/* Declare volatile pointers to I/O registers (volatile means that IO load
	 * and store instructions will be used to access these pointer locations,
	 * instead of regular memory loads and stores)
	 */
	
	volatile int * interval_timer_ptr = (int *)TIMER_BASE; // interal timer base address
	volatile int * KEY_ptr = (int *)KEY_BASE; // pushbutton KEY address
	
	/* set the interval timer period for scrolling the LED lights */
	int counter = 100000000; // 1/(50 MHz) x (50_000_000) = 1000 msec or 1sec
	*(interval_timer_ptr + 0x2) = (counter & 0xFFFF);
	*(interval_timer_ptr + 0x3) = (counter >> 16) & 0xFFFF;
	
	/* start interval timer, enable its interrupts */
	*(interval_timer_ptr + 1) = 0x7; // STOP = 0, START = 1, CONT = 1, ITO = 1
	*(KEY_ptr + 2) = 0x3; // enable interrupts for all pushbuttons
	
	/* set interrupt mask bits for levels 0 (interval timer) and level 1
	* (pushbuttons) */
	NIOS2_WRITE_IENABLE(0x3);
	NIOS2_WRITE_STATUS(1); // enable Nios II interrupts
	
	while (1); // main program simply idles
}

/* The assembly language code below handles CPU reset processing */
void the_reset(void) __attribute__((section(".reset")));
void the_reset(void)
/*******************************************************************************
* Reset code. By giving the code a section attribute with the name ".reset" we
* allow the linker program to locate this code at the proper reset vector
* address. This code just calls the main program.
******************************************************************************/
{
	asm(".set noat"); /* Instruct the assembler NOT to use reg at (r1) as
					   * a temp register for performing optimizations */
	asm(".set nobreak"); /* Suppresses a warning message that says that
						  * some debuggers corrupt regs bt (r25) and ba
						  * (r30)
						  */
	asm("movia r2, main"); // Call the C language main program
	asm("jmp r2");
}
/* The assembly language code below handles CPU exception processing. This
* code should not be modified; instead, the C language code in the function
* interrupt_handler() can be modified as needed for a given application.
*/
void the_exception(void) __attribute__((section(".exceptions")));
void the_exception(void)
/*******************************************************************************
* Exceptions code. By giving the code a section attribute with the name
* ".exceptions" we allow the linker program to locate this code at the proper
* exceptions vector address.
* This code calls the interrupt handler and later returns from the exception.
******************************************************************************/
{
	asm("subi sp, sp, 128");
	asm("stw et, 96(sp)");
	asm("rdctl et, ctl4");
	asm("beq et, r0, SKIP_EA_DEC"); // Interrupt is not external
	asm("subi ea, ea, 4"); /* Must decrement ea by one instruction
							* for external interrupts, so that the
							* interrupted instruction will be run */
	asm("SKIP_EA_DEC:");
	asm("stw r1, 4(sp)"); // Save all registers
	asm("stw r2, 8(sp)");
	asm("stw r3, 12(sp)");
	asm("stw r4, 16(sp)");
	asm("stw r5, 20(sp)");
	asm("stw r6, 24(sp)");
	asm("stw r7, 28(sp)");
	asm("stw r8, 32(sp)");
	asm("stw r9, 36(sp)");
	asm("stw r10, 40(sp)");
	asm("stw r11, 44(sp)");
	asm("stw r12, 48(sp)");
	asm("stw r13, 52(sp)");
	asm("stw r14, 56(sp)");
	asm("stw r15, 60(sp)");
	asm("stw r16, 64(sp)");
	asm("stw r17, 68(sp)");
	asm("stw r18, 72(sp)");
	asm("stw r19, 76(sp)");
	asm("stw r20, 80(sp)");
	asm("stw r21, 84(sp)");
	asm("stw r22, 88(sp)");
	asm("stw r23, 92(sp)");
	asm("stw r25, 100(sp)"); // r25 = bt (skip r24 = et, because it is saved
							 // above)
	asm("stw r26, 104(sp)"); // r26 = gp
	// skip r27 because it is sp, and there is no point in saving this
	asm("stw r28, 112(sp)"); // r28 = fp
	asm("stw r29, 116(sp)"); // r29 = ea
	asm("stw r30, 120(sp)"); // r30 = ba
	asm("stw r31, 124(sp)"); // r31 = ra
	asm("addi fp, sp, 128");
	asm("call interrupt_handler"); // Call the C language interrupt handler
	asm("ldw r1, 4(sp)"); // Restore all registers
	asm("ldw r2, 8(sp)");
	asm("ldw r3, 12(sp)");
	asm("ldw r4, 16(sp)");
	asm("ldw r5, 20(sp)");
	asm("ldw r6, 24(sp)");
	asm("ldw r7, 28(sp)");
	asm("ldw r8, 32(sp)");
	asm("ldw r9, 36(sp)");
	asm("ldw r10, 40(sp)");
	asm("ldw r11, 44(sp)");
	asm("ldw r12, 48(sp)");
	asm("ldw r13, 52(sp)");
	asm("ldw r14, 56(sp)");
	asm("ldw r15, 60(sp)");
	asm("ldw r16, 64(sp)");
	asm("ldw r17, 68(sp)");
	asm("ldw r18, 72(sp)");
	asm("ldw r19, 76(sp)");
	asm("ldw r20, 80(sp)");
	asm("ldw r21, 84(sp)");
	asm("ldw r22, 88(sp)");
	asm("ldw r23, 92(sp)");
	asm("ldw r24, 96(sp)");
	asm("ldw r25, 100(sp)"); // r25 = bt
	asm("ldw r26, 104(sp)"); // r26 = gp
	// skip r27 because it is sp, and we did not save this on the stack
	asm("ldw r28, 112(sp)"); // r28 = fp
	asm("ldw r29, 116(sp)"); // r29 = ea
	asm("ldw r30, 120(sp)"); // r30 = ba
	asm("ldw r31, 124(sp)"); // r31 = ra
	asm("addi sp, sp, 128");
	asm("eret");
}
/******************************************************************************
* Interrupt Service Routine
* Determines what caused the interrupt and calls the appropriate
* subroutine.
*
* ipending - Control register 4 which has the pending external interrupts
******************************************************************************/
void interrupt_handler(void) {
	int ipending;
	NIOS2_READ_IPENDING(ipending);
	
	if (ipending & 0x1) // interval timer is interrupt level 0
	{
		interval_timer_ISR();
	}
	if (ipending & 0x2) // pushbuttons are interrupt level 1
	{
		pushbutton_ISR();
	}
	// else, ignore the interrupt
	return;
}

/*******************************************************************************
* Interval timer interrupt service routine
*
* Shifts a PATTERN being displayed on the LED lights. The shift direction
* is determined by the external variable key_dir.
******************************************************************************/
void interval_timer_ISR() {
	volatile int * interval_timer_ptr = (int *)TIMER_BASE;
	volatile int * LEDG_ptr = (int *)LED_BASE; // LED address
	
	*(interval_timer_ptr) = 0; // clear the interrupt
/*	
	*(LEDG_ptr) = pattern; // display pattern on LED
	
	if (shift_enable == 'DISABLE') // check if shifting is disabled
		return;
	/* rotate the pattern shown on the LEDG lights */
/*	if (shift_dir == 'LEFT') // rotate left
		if (pattern & 0x80000000)
		pattern = (pattern << 1) | 1;
	else
		pattern = pattern << 1;
	else // rotate right
		if (pattern & 0x00000001)
		pattern = (pattern >> 1) | 0x80000000;
	else
		pattern = (pattern >> 1) & 0x7FFFFFFF;
*/	
	printf("%d sec: 1 sec has passed \n", ++timeElapsed);
	
	return;
}

void pushbutton_ISR(void) {
	volatile int * KEY_ptr = (int *)KEY_BASE;
	volatile int * slider_switch_ptr = (int *)SW_BASE;
	
	int press;
	press = *(KEY_ptr + 3); // read the pushbutton interrupt register
	*(KEY_ptr + 3) = press; // Clear the interrupt
	
	if (press & 0x1) // KEY0
		pattern = *slider_switch_ptr;
	if (press & 0x2) // KEY1
		shift_dir = shift_dir ^ 1;
	
	return;
}