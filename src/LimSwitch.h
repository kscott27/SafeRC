/*
 * LimSwitch.h
 *
 * Created: 11/29/2017 5:49:38 PM
 *  Author: Kevin
 */ 


#ifndef LIMSWITCH_H_
#define LIMSWITCH_H_

#include <stdio.h>
#include <avr/io.h>                         // Port I/O for SFR's
#include <avr/interrupt.h>

class LimSwitch
{
	protected: 
	
	PORT_t* pport;
	uint8_t pin;
	register8_t* pinctrl;
	
	public:
	
	LimSwitch(PORT_t* pport, uint8_t pin, register8_t* pinctrl);
};



#endif /* LIMSWITCH_H_ */