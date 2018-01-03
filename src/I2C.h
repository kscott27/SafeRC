/*
 * I2C.h
 *
 * Created: 12/1/2017 12:06:55 AM
 *  Author: Kevin
 */ 


#ifndef I2C_H_
#define I2C_H_

#include <avr/io.h>                         // Port I/O for SFR's
#include <avr/interrupt.h>
#include "sensor_struct.h"
#include "frt_shared_data.h"                // Header for thread-safe shared data
#include "frt_queue.h"


class I2C
{
	protected:
	
	PORT_t* pport;
	TWI_t*  ptwiport;
	uint8_t sda;
	uint8_t scl;
	uint8_t remaining_bytes;
	uint8_t data;
	
	public:
	
	shared_data<uint8_t>* pus_highbyte;
	shared_data<uint8_t>* pus_lowbyte;
	shared_data<bool>* ptransfer_complete;
	SensorWriteData_t* pwrite_struct;
	
	I2C(PORT_t* pport, TWI_t*  ptwiport, uint8_t sda, uint8_t scl, SensorWriteData_t* pwrite_struct, shared_data<bool>* ptransfer_complete, shared_data<uint8_t>* pus_highbyte, shared_data<uint8_t>* pus_lowbyte);
	
	void I2CInitWrite(void);
	
	void I2CInitRead(void);
	
	void I2CWriteDataTrans(void);
	
	void I2CReadDataTrans(void);
	
	void I2CAckStop(void);
	
	void I2CNackStop(void);
};



#endif /* I2C_H_ */

