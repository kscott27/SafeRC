/*
 * task_sensor.h
 *
 * Created: 12/1/2017 1:49:05 PM
 *  Author: John
 */ 


#ifndef TASK_SENSOR_H_
#define TASK_SENSOR_H_

#include <stdlib.h>                         // Prototype declarations for I/O functions
#include <avr/io.h>                         // Port I/O for SFR's
#include <avr/wdt.h>                        // Watchdog timer header
#include <avr/delay.h>

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

#include "I2C.h"

/// This macro defines a string that identifies the name and version of this program.
#define PROGRAM_VERSION		PMS ("ME405 base radio program V0.4 ")







//-------------------------------------------------------------------------------------
/** This task interacts with the user for force him/her to do what he/she is told. What
 *  a rude task this is. Then again, computers tend to be that way; if they're polite
 *  with you, they're probably spying on you. 
 */

class task_sensor : public frt_task
{
private:
	// No private variables or methods for this class

protected:

    //SensorWriteData_t* pwrite_struct;
	I2C* pI2C;
	uint8_t us_bufferhigh[256];
	uint8_t buffer_index;
	uint8_t avg_highbyte;

	// This method displays a simple help message telling the user what to do. It's
	// protected so that only methods of this class or possibly descendants can use it
	void print_help_message (void);

	// This method displays information about the status of the system
	void show_status (void);


public:
	// This constructor creates a user interface task object
	task_sensor (const char*, unsigned portBASE_TYPE, size_t, emstream*, I2C* pI2C);

	/** This method is called by the RTOS once to run the task loop for ever and ever.
	 */
	void run (void);
};




#endif /* TASK_SENSOR_H_ */