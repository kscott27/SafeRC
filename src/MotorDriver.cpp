/*
 * CPPFile1.cpp
 *
 * Created: 11/29/2017 3:12:40 PM
 *  Author: Kevin
 */ 

#include "MotorDriver.h"

MotorDriver::MotorDriver(PORT_t* pINBport, PORT_t* pENBport, PORT_t* pPWMport, PORT_t* 
pENAport, PORT_t* pINAport, TC0_t* ptimer, register16_t* pchannel, uint8_t
INB, uint8_t ENB, uint8_t PWM, uint8_t ENA, uint8_t INA, uint16_t pwm_val)
{
	//Configure pins for specific motor driver function lines
	pINBport->DIRSET |= INB;
	pENBport->DIRSET |= ENB;
	pPWMport->DIRSET |= PWM;
	pENAport->DIRSET |= ENA;
	pINAport->DIRSET |= INA;
	
	//Ensure motor is off upon initialization
	motorOff();
	
	//Configure timer clock for PWM
	ptimer->CTRLA = TC_CLKSEL_DIV1_gc;
	
	//Configure waveform generation mode for PWM
	ptimer->CTRLB = TC0_WGMODE0_bm | TC0_WGMODE1_bm | TC0_CCBEN_bm;
	
	//Configure period to be 1600 counts for a frequency of 20kHz
	ptimer->PER = 1600; //set for appropriate duty cycle denominator
	
}
	
void MotorDriver::motorOpen(void)
{
	pINAport->OUTSET = INA;
	pINBport->OUTCLR = INB;
	*pchannel = pwm_val;
}
	
void MotorDriver::motorClose(void)
{
	pINAport->OUTCLR = INA;
	pINBport->OUTSET = INB;	
	*pchannel = pwm_val;
}
	
void MotorDriver::motorOff(void)
{
	//Initially have motor in brake mode
	pINAport->OUTCLR = INA;
	pINBport->OUTCLR = INB;
	//Duty cycle numerator initially set to 0
	*pchannel = 0;	
}
	


