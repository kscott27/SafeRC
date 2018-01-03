/*
 * task_sensor.cpp
 *
 * Created: 12/2/2017 2:59:15 PM
 *  Author: Kevin
 */ 

#include "task_sensor.h"
#include "I2C.h"

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

task_sensor::task_sensor (const char* a_name, unsigned portBASE_TYPE a_priority, size_t 
                          a_stack_size, emstream* p_ser_dev, I2C* pI2C)
	: frt_task (a_name, a_priority, a_stack_size, p_ser_dev)
{
	pI2C = pI2C;
}


//-------------------------------------------------------------------------------------
/** This task interacts with the user for force him/her to do what he/she is told. It
 *  is just following the modern government model of "This is the land of the free...
 *  free to do exactly what you're told." 
 */

void task_sensor::run (void)
{
	//char char_in;                           // Character read from serial device
	//time_stamp a_time;                      // Holds the time so it can be displayed
	portTickType previousTicks;
	
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
			case (0):
			//*p_serial << PMS ("case 0 entered") << endl;
			pI2C->I2CInitWrite();
			transition_to(1);
				
			break; 

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			// In state 1, the ESC is only receiving commands from the microcontroller
			case (1):
			//*p_serial << PMS ("case 1 entered") << endl;

			if (ptransfer_complete->get())
			{
				previousTicks = xTaskGetTickCount();
				delay_from_to(previousTicks, 500);
				transition_to(2);
			}

			
			break; // End of state 1
			
			case (2):
			//*p_serial << PMS ("case 2 entered") << endl;
			pI2C->I2CInitRead();
			transition_to(3);
			
			break;
			
			case (3):
			//*p_serial << PMS ("case 3 entered") << endl;
			if (ptransfer_complete->get())
			{
				//*p_serial << PMS ("case 3 entered") << endl;
				transition_to(0);
			}

			break;
			
			case (4):
			
			us_bufferhigh[buffer_index] = pus_highbyte->get();
			
			avg_highbyte = 0;
			for(uint8_t i = 0; i < 6; i++)
			{
				avg_highbyte = (avg_highbyte + us_bufferhigh[buffer_index - i]) / (i + 1);
			}
			
			if(avg_highbyte < 3)
			{
				pesc_override->put(true);
				previousTicks = xTaskGetTickCount();
				delay_from_to(previousTicks, 2000);
			}
			else
			{
				pesc_override->put(false);
			}
			buffer_index++;
			
			transition_to(0);
			
			break;

		} // End switch state

		runs++;                             // Increment counter for debugging

		// No matter the state, wait for approximately a millisecond before we 
		// run the loop again. This gives lower priority tasks a chance to run
		vTaskDelay (configMS_TO_TICKS (1));
	}
}

