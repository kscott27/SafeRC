/*
 * Struct.h
 *
 * Created: 12/3/2017 5:21:36 PM
 *  Author: Kevin
 */ 


#ifndef SENSOR_STRUCT_H_
#define SENSOR_STRUCT_H_

#include <avr/io.h>                         // Port I/O for SFR's
#include <avr/interrupt.h>

// Create struct for data to be written by each sensor
typedef struct SensorWriteData {
	uint8_t slave_address;
	uint8_t range_cmd;
	uint8_t units_reg;
	uint8_t euler_bm;
	uint8_t mode_reg;
	uint8_t nine_dof_bm;
	uint8_t package_length;
	uint8_t ctrl_reg;
	uint8_t activate_value;

} SensorWriteData_t;




#endif /* SENSOR_STRUCT_H_*/