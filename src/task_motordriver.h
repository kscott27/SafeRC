/*
 * task_motordriver.h
 *
 * Created: 12/2/2017 1:13:31 PM
 *  Author: Kevin
 */ 


#ifndef TASK_MOTORDRIVER_H_
#define TASK_MOTORDRIVER_H_

#include <stdlib.h>                         // Prototype declarations for I/O functions
#include <avr/io.h>                         // Port I/O for SFR's
#include <avr/interrupt.h>

#include "FreeRTOS.h"                       // Primary header for FreeRTOS
#include "task.h"                           // Header for FreeRTOS task functions
#include "queue.h"                          // FreeRTOS inter-task communication queues

#include "ansi_terminal.h"

#include "rs232int.h"                       // ME405/507 library for serial comm.
#include "time_stamp.h"                     // Class to implement a microsecond timer
#include "frt_task.h"                       // Header for ME405/507 base task class
#include "frt_queue.h"                      // Header of wrapper for FreeRTOS queues
#include "frt_text_queue.h"                 // Header for a "<<" queue class
#include "frt_shared_data.h"                // Header for thread-safe shared data

#include "shares.h"                         // Global ('extern') queue declarations

#include "MotorDriver.h"

/// This macro defines a string that identifies the name and version of this program. 
#define PROGRAM_VERSION		PMS ("ME405 base radio program V0.4 ")


//-------------------------------------------------------------------------------------
/** This task interacts with the user for force him/her to do what he/she is told. What
 *  a rude task this is. Then again, computers tend to be that way; if they're polite
 *  with you, they're probably spying on you. 
 */

class task_motordriver : public frt_task
{
private:
	// No private variables or methods for this class

protected:


	// This method displays a simple help message telling the user what to do. It's
	// protected so that only methods of this class or possibly descendents can use it
	void print_help_message (void);

	// This method displays information about the status of the system
	void show_status (void);

	MotorDriver* pmotordriver;
	shared_data<bool>* pmotor_open;
	shared_data<bool>* pmotor_close;
	shared_data<bool>* pmotor_off;
	

public:
	// This constructor creates a user interface task object
	task_motordriver (const char*, unsigned portBASE_TYPE, size_t, emstream*,  MotorDriver* pmotordriver, shared_data<bool>* pmotor_open, shared_data<bool>* pmotor_close, shared_data<bool>* pmotor_off);

	/** This method is called by the RTOS once to run the task loop for ever and ever.
	 */
	void run (void);
};



#endif /* TASK_MOTORDRIVER_H_ */