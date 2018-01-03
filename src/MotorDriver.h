/*
 * MotorDriver.h
 *
 * Created: 11/29/2017 3:09:50 PM
 *  Author: Kevin
 */ 


#ifndef MOTORDRIVER_H_
#define MOTORDRIVER_H_

#include <avr/io.h>                         // Port I/O for SFR's
#include <avr/interrupt.h>

class MotorDriver
{
	protected:
	
	PORT_t* pINBport;
	PORT_t* pENBport;
	PORT_t* pPWMport;
	PORT_t* pENAport;
	PORT_t* pINAport;
	TC0_t* ptimer;
	register16_t* pchannel; //TCD0.CCC
	uint8_t INB;
	uint8_t ENB;
	uint8_t PWM;
	uint8_t ENA;
	uint8_t INA;
	uint8_t md_state;
	uint16_t pwm_val;
	
	public:
	
	MotorDriver(PORT_t* pINBport, PORT_t* pENBport, PORT_t* pPWMport, PORT_t*
	pENAport, PORT_t* pINAport, TC0_t* ptimer, register16_t* pchannel, uint8_t
	INB, uint8_t ENB, uint8_t PWM, uint8_t ENA, uint8_t INA, uint16_t pwm_val);
	
	void motorOpen(void);
	
	void motorClose(void);
	
	void motorOff(void);
	
	void runMotorDriver(void);
};




#endif /* MOTORDRIVER_H_ */