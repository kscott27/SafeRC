/*
 * LimitSwitch.cpp
 *
 * Created: 11/29/2017 5:52:24 PM
 *  Author: Kevin
 */ 

#include "LimSwitch.h"
	
LimSwitch::LimSwitch(PORT_t* pport, uint8_t pin, register8_t* pinctrl)
{	
	//Configure Port C bits for input capture
	pport->DIRCLR |= pin;


	//Set input sense configuration for PC3
	*pinctrl |= PORT_OPC_PULLUP_gc | PORT_ISC_FALLING_gc;
	
	//Set Lim Switch pins for specific port interrupt
	pport->INTCTRL |= PORT_INT0LVL1_bm | PORT_INT0LVL0_bm;
	pport->INT0MASK |= pin;

	//Enable interrupts for all priority levels
	PMIC.CTRL |= PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm;

	/*//Link event Channel 2 to PC2
	EVSYS.CH2MUX = EVSYS_CHMUX_PORTA_PIN3_gc;
	//Link event Channel 3 to PC3
	EVSYS.CH3MUX = EVSYS_CHMUX_PORTA_PIN4_gc;*/

};
