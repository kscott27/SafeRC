/*
 * task_ESC.cpp
 *
 * Created: 12/1/2017 3:44:33 PM
 *  Author: Kevin
 */ 


#include "task_ESC.h"


/** This constant sets how many RTOS ticks the task delays if the user's not talking.
 *  The duration is calculated to be about 5 ms.
 */
const portTickType ticks_to_delay = ((configTICK_RATE_HZ / 1000) * 5);


//-------------------------------------------------------------------------------------
/** This constructor creates a new data acquisition task. Its main job is to call the
 *  parent class's constructor which does most of the work.
 *  @param a_name A character string which will be the name of this task
 *  @param a_priority The priority at which this task will initially run (default: 0)
 *  @param a_stack_size The size of this task's stack in bytes 
 *                      (default: configMINIMAL_STACK_SIZE)
 *  @param p_ser_dev Pointer to a serial device (port, radio, SD card, etc.) which can
 *                   be used by this task to communicate (default: NULL)
 */

task_ESC::task_ESC (const char* a_name, 
					  unsigned portBASE_TYPE a_priority, 
					  size_t a_stack_size,
					  emstream* p_ser_dev, ESCDriver* pesc_driver
					 )
	: frt_task (a_name, a_priority, a_stack_size, p_ser_dev)
{
	
}


//-------------------------------------------------------------------------------------
/** This task interacts with the user for force him/her to do what he/she is told. It
 *  is just following the modern government model of "This is the land of the free...
 *  free to do exactly what you're told." 
 */

void task_ESC::run (void)
{
	//char char_in;                           // Character read from serial device
	//time_stamp a_time;                      // Holds the time so it can be displayed
	//ESCDriver esc_driver = ESCDriver();
	portTickType previousTicks;
	pesc_override->put(false);	
	delay_ms(3000);
	// Tell the user how to get into command mode (state 1), where the user interface
	// task does interesting things such as diagnostic printouts
	//*p_serial << PMS ("Press Ctrl-A for command mode") << endl;

	// This is an infinite loop; it runs until the power is turned off. There is one 
	// such loop inside the code for each task
	for (;;)
	{
		// Run the finite state machine. The variable 'state' is kept by the parent class
		switch (state)
		{
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			// In state 0, the ESC is accepting commands from the radio transmitter
			case (0):
			*p_serial << PMS ("case 0 entered") << endl;
			
			previousTicks = xTaskGetTickCount();
			delay_from_to(previousTicks, 5000);
			
			if(pesc_override->get()) //If the sensors trigger an ESC override, transition to state 1
			{
				pesc_driver->stopESC();
				
				previousTicks = xTaskGetTickCount();
				delay_from_to(previousTicks, 1000);
				
				transition_to (1);
			}
				
			break; // End of state 0

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			
			// In state 1, the ESC is reversing
			case (1):
			*p_serial << PMS ("case 1 entered") << endl;
			
			if(!pesc_override->get())
			{
				transition_to(0);
			}
			
			break; // End of state 1
			
		}

		runs++;                             // Increment counter for debugging

		// No matter the state, wait for approximately a millisecond before we 
		// run the loop again. This gives lower priority tasks a chance to run
		vTaskDelay (configMS_TO_TICKS (1));
	}
}
