/*
 * task_motordriver.cpp
 *
 * Created: 12/2/2017 1:29:58 PM
 *  Author: Kevin
 */ 


#include "task_motordriver.h"                      // Header for this file


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

task_motordriver::task_motordriver (const char* a_name, 
					  unsigned portBASE_TYPE a_priority, 
					  size_t a_stack_size,
					  emstream* p_ser_dev, MotorDriver* pmotordriver, shared_data<bool>* pmotor_open, shared_data<bool>* pmotor_close, shared_data<bool>* pmotor_off
					 )
	: frt_task (a_name, a_priority, a_stack_size, p_ser_dev)
{
	// Nothing is done in the body of this constructor. All the work is done in the
	// call to the frt_task constructor on the line just above this one
}


//-------------------------------------------------------------------------------------
/** This task interacts with the user for force him/her to do what he/she is told. It
 *  is just following the modern government model of "This is the land of the free...
 *  free to do exactly what you're told." 
 */

void task_motordriver::run (void)
{
	//char char_in;                           // Character read from serial device
	//time_stamp a_time;                      // Holds the time so it can be displayed
	

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
			
			if(pmotor_open->get())
			{
				pmotordriver->motorOpen();
				transition_to (1);
			}
				
			break; // End of state 0

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			// In state 1, the ESC is only receiving commands from the microcontroller
			case (1):
			
			if(pmotor_close->get())
			{
				pmotordriver->motorClose();
				transition_to(2);
			}
			
			break; // End of state 1
			
			case (2):
			
			if(pmotor_off->get())
			{
				pmotordriver->motorOff();
				transition_to(0);
			}
				
			break; // End of state 2

		} // End switch state

		runs++;                             // Increment counter for debugging

		// No matter the state, wait for approximately a millisecond before we 
		// run the loop again. This gives lower priority tasks a chance to run
		vTaskDelay (configMS_TO_TICKS (1));
	}
}


//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
/** This method prints a simple help message.
 */

void task_motordriver::print_help_message (void)
{
	*p_serial << ATERM_BKG_CYAN << ATERM_TXT_BLACK << clrscr;
	*p_serial << PROGRAM_VERSION << PMS (" help") << endl;
	*p_serial << PMS ("  Ctl-C: Reset the AVR") << endl;
	*p_serial << PMS ("  Ctl-A: Enter command mode") << endl;
	*p_serial << PMS ("  In command mode only:") << endl;
	*p_serial << PMS ("    n:   Show the time right now") << endl;
	*p_serial << PMS ("    v:   Version and setup information") << endl;
	*p_serial << PMS ("    s:   Stack dump for tasks") << endl;
	*p_serial << PMS ("    e:   Exit command mode") << endl;
	*p_serial << PMS ("    h:   HALP!") << endl;
}


//-------------------------------------------------------------------------------------
/** This method displays information about the status of the system, including the
 *  following: 
 *    \li The name and version of the program
 *    \li The name, status, priority, and free stack space of each task
 *    \li Processor cycles used by each task
 *    \li Amount of heap space free and setting of RTOS tick timer
 */

void task_motordriver::show_status (void)
{
	time_stamp the_time;					// Holds current time for printing

	// Show program vesion, time, and free heap space
	*p_serial << endl << PROGRAM_VERSION << PMS (__DATE__) << endl 
			  << PMS ("Time: ") << the_time.set_to_now ()
			  << PMS (", Heap free: ") << heap_left() << PMS ("/") 
			  << configTOTAL_HEAP_SIZE;

	// Show how the timer/counter is set up to cause RTOS timer ticks
	*p_serial << PMS (", TCC0CCA=") << TCC0.CCA << endl << endl;

	// Have the tasks print their status
	print_task_list (p_serial);
}