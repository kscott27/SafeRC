//*************************************************************************************
/** \file lab1_main.cpp
 *    This file contains the main() code for a program which runs a port of the FreeRTOS
 *    for AVR devices. This port is specific to the XMEGA family.
 *
 *  Revisions:
 *    \li 09-14-2017 CTR Adapted from JRR code for AVR to be compatible with xmega 
 *
 *  License:
 *    This file is released under the Lesser GNU Public License, version 2. This 
 *    program is intended for educational use only, but it is not limited thereto. 
 */
//*************************************************************************************


#include <stdlib.h>                         // Prototype declarations for I/O functions
#include <avr/io.h>                         // Port I/O for SFR's
#include <avr/wdt.h>                        // Watchdog timer header
#include <avr/interrupt.h>
#include <string.h>                         // Functions for C string handling

#include "FreeRTOS.h"                       // Primary header for FreeRTOS
#include "task.h"                           // Header for FreeRTOS task functions
#include "queue.h"                          // FreeRTOS inter-task communication queues
#include "croutine.h"                       // Header for co-routines and such

#include "rs232int.h"                       // ME405/507 library for serial comm.
#include "time_stamp.h"                     // Class to implement a microsecond timer
#include "frt_task.h"                       // Header of wrapper for FreeRTOS tasks
#include "frt_text_queue.h"                 // Wrapper for FreeRTOS character queues
#include "frt_queue.h"                      // Header of wrapper for FreeRTOS queues
#include "frt_shared_data.h"                // Header for thread-safe shared data
#include "shares.h"                         // Global ('extern') queue declarations
#include "sensor_struct.h"

#include "task_user.h"                      // Header for user interface task
#include "task_ESC.h"
#include "task_sensor.h"
#include "task_motordriver.h"
#include "MotorDriver.h"
#include "ESCDriver.h"
#include "I2C.h"


frt_text_queue print_ser_queue (32, NULL, 10);

// Create Pointers
I2C* pI2C;
ESCDriver* pescdriver;

shared_data<bool>* pesc_override;
shared_data<bool>* ptransfer_complete;

SensorWriteData_t* pultrasound_write_data;
shared_data<uint8_t>* pus_highbyte;
shared_data<uint8_t>* pus_lowbyte;

// Created in main and shared so that each task can use it to point to its
// respective struct object for write data. It will then be used in interrupts to
// call certain data from that object.
SensorWriteData_t* psensor_write_data;
frt_queue<uint8_t>* psensor_read_buffer;
shared_data<uint8_t>* psensor_index;

uint8_t remaining_bytes;
	

/*! \brief CCP write helper function written in assembly.
 *
 *  This function is written in assembly because of the time critical
 *  operation of writing to the registers.
 *
 *  \param address A pointer to the address to write to.
 *  \param value   The value to put in to the register.
 */
void CCPWrite( volatile uint8_t * address, uint8_t value )
{
	#if defined __GNUC__
	uint8_t volatile saved_sreg = SREG;
	cli();
	volatile uint8_t * tmpAddr = address;
	#ifdef RAMPZ
	RAMPZ = 0;
	#endif
	asm volatile(
	"movw r30,  %0"	      "\n\t"
	"ldi  r16,  %2"	      "\n\t"
	"out   %3, r16"	      "\n\t"
	"st     Z,  %1"       "\n\t"
	:
	: "r" (tmpAddr), "r" (value), "M" (0xD8), "i" (&CCP)
	: "r16", "r30", "r31"
	);

	SREG = saved_sreg;
	#endif
}

//=====================================================================================
/** The main function sets up the RTOS.  Some test tasks are created. Then the 
 *  scheduler is started up; the scheduler runs until power is turned off or there's a 
 *  reset.
 *  @return This is a real-time microcontroller program which doesn't return. Ever.
 */

int main (void)
{
	cli();
	// Configure the system clock
	{	
		// Enable the 32MHz internal RC oscillator and the external 32KHz oscillator
		OSC.CTRL |= (1 << OSC_RC32MEN_bp);
		do {} while((OSC.STATUS & (1 << OSC_RC32MRDY_bp)) != (1 << OSC_RC32MRDY_bp));

		// Select the clock
		CCPWrite(&(CLK.CTRL),((CLK.CTRL & ~CLK_SCLKSEL_gm) | (1 << CLK_SCLKSEL0_bp)));
		
		// Disable the 2MHz internal RC oscillator
		OSC.CTRL &= ~(1 << OSC_RC2MEN_bp);
	}
	
	// Disable the watchdog timer unless it's needed later. This is important because
	// sometimes the watchdog timer may have been left on...and it tends to stay on	 
	wdt_disable ();

	//Creating Objects
	
	pesc_override = new shared_data<bool>;
	ptransfer_complete = new shared_data<bool>;
	
	pus_highbyte = new shared_data<uint8_t>;
	pus_lowbyte = new shared_data<uint8_t>;
	
	// Create data struct objects for each sensor	
	SensorWriteData_t ultrasound_write_data;
	
	// Link data struct pointers to each respective object
	pultrasound_write_data = &ultrasound_write_data;
	pultrasound_write_data->slave_address = 224;
	pultrasound_write_data->package_length = 1;
	pultrasound_write_data->range_cmd = 81;
	
	pescdriver = new ESCDriver(pesc_override);	
	pI2C = new I2C (&PORTC, &TWIC, PIN0_bm, PIN1_bm, pultrasound_write_data, ptransfer_complete, pus_highbyte, pus_lowbyte);
	

	// Configure a serial port which can be used by a task to print debugging infor-
	// mation, or to allow user interaction, or for whatever use is appropriate.  The
	// serial port will be used by the user interface task after setup is complete and
	// the task scheduler has been started by the function vTaskStartScheduler()
	rs232 ser_dev(0,&USARTC1); // Create a serial device on USART C1
	ser_dev << clrscr << "FreeRTOS Xmega Testing Program" << endl << endl;
	
	
	// The user interface is at low priority; it could have been run in the idle task
	// but it is desired to exercise the RTOS more thoroughly in this test program
	
	new task_user ("UserInt", task_priority (0), 260, &ser_dev);
	
	new task_sensor ("task_ultrasound", task_priority (6), 260, &ser_dev, pI2C);
	
	new task_ESC ("task_esc", task_priority (6), 260, &ser_dev, pescdriver);

		
	// Enable high level interrupts and global interrupts
	PMIC_CTRL = (1 << PMIC_HILVLEN_bp | 1 << PMIC_MEDLVLEN_bp | 1 << PMIC_LOLVLEN_bp);
	sei();
	
	// Here's where the RTOS scheduler is started up. It should never exit as long as
	// power is on and the microcontroller isn't rebooted
	vTaskStartScheduler ();
}

//***********************************************************************************************************
//************************************INTERRUPTS*************************************************************
//***********************************************************************************************************

ISR(TCC0_CCC_vect)
{
	pescdriver->operateESC();
}

ISR(TWIC_TWIM_vect)
{
	//If TWI arbitration is lost send STOP
	
	if(TWIC.MASTER.STATUS & (1<<TWI_MASTER_ARBLOST_bp))
	{
		pI2C->I2CAckStop(); //send stop condition
	}
	
	//If TWI bus error flag is set or NACK received then send STOP
	
	else if((TWIC.MASTER.STATUS & (1<<TWI_MASTER_BUSERR_bp)) || (TWIC.MASTER.STATUS & (1<<TWI_MASTER_RXACK_bp)))
	{
		pI2C->I2CAckStop(); //send stop condition
	}
	
	//If TWI write interrupt flag is set
	
	else if(TWIC.MASTER.STATUS & TWI_MASTER_WIF_bm)
	{
		pI2C->I2CWriteDataTrans();		
	}
	
	//If RTC read interrupt flag is set
	else if(TWIC.MASTER.STATUS & (1<<TWI_MASTER_RIF_bp))          //check whether read interrupt flag is set or not
	{
		pI2C->I2CReadDataTrans();
	}
}

