//*************************************************************************************
/** \file shares.h
 *    This file contains extern declarations for queues and other inter-task data
 *    communication objects used in a ME405/507/FreeRTOS project. 
 *
 *  Revisions:
 *    \li 09-30-2012 JRR Original file was a one-file demonstration with two tasks
 *    \li 10-05-2012 JRR Split into multiple files, one for each task plus a main one
 *    \li 10-29-2012 JRR Reorganized with global queue and shared data references
 *
 *  License:
 *		This file is copyright 2012 by JR Ridgely and released under the Lesser GNU 
 *		Public License, version 2. It intended for educational use only, but its use
 *		is not limited thereto. */
/*		THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 *		AND	ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * 		IMPLIED 	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * 		ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
 * 		LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUEN-
 * 		TIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS 
 * 		OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
 * 		CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
 * 		OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * 		OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */
//*************************************************************************************

// This define prevents this .h file from being included multiple times in a .cpp file
#ifndef _SHARES_H_
#define _SHARES_H_

#include "sensor_struct.h"
#include "I2C.h"


//-------------------------------------------------------------------------------------
// Externs:  In this section, we declare variables and functions that are used in all
// (or at least two) of the files in the data acquisition project. Each of these items
// will also be declared exactly once, without the keyword 'extern', in one .cpp file
// as well as being declared extern here. 


/**
 * \var print_ser_queue
 * \brief This queue allows tasks to send characters to the user interface task for display.
 */
extern frt_text_queue print_ser_queue;			// This queue allows tasks to send characters to the user interface task for display.

extern frt_queue<uint16_t> time_queue;
extern frt_queue<int16_t> ang_queue;
extern frt_queue<int16_t> A_queue;
extern frt_queue<int16_t> B_queue;
extern frt_queue<int16_t> C_queue;

extern shared_data<bool>* pmotor1_open;
extern shared_data<bool>* pmotor2_open;
extern shared_data<bool>* pmotor1_close;
extern shared_data<bool>* pmotor2_close;
extern shared_data<bool>* pmotor1_off;
extern shared_data<bool>* pmotor2_off;
extern frt_queue<uint8_t>* psensor_read_buffer;
extern shared_data<uint8_t>* psensor_index;
extern frt_queue<uint8_t>* pus_read_buffer;
extern frt_queue<uint8_t>* pimu_read_buffer;
extern shared_data<bool>* pesc_override;
extern shared_data<bool>* ptransfer_complete;
extern shared_data<uint8_t>* pus_highbyte;
extern shared_data<uint8_t>* pus_lowbyte;
extern shared_data<bool>* pimu_init_flag;
extern I2C* pI2C;
extern frt_queue<uint16_t>* pus_read_queue;
extern frt_queue<uint16_t>* pimu_read_queue;
extern SensorWriteData_t* pwrite_struct;
extern SensorWriteData_t* pultrasound_write_data;
extern SensorWriteData_t* pimu_write_data;


#endif // _SHARES_H_
