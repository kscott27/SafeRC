/*
 * ESCDriver.cpp
 *
 * Created: 11/29/2017 12:57:50 PM
 *  Author: Kevin
 */ 


#include "ESCDriver.h"


ESCDriver::ESCDriver (shared_data<bool>* pesc_override)
{	
	pesc_override = pesc_override;
	
	//Configure Port E bits 0-3 (pin 1-4) as outputs
	PORTC.DIRSET = PIN4_bm;
	
	//Configure waveform generation mode to single slope PWM
	TCC1.CTRLB = TC1_WGMODE0_bm | TC1_CCAEN_bm;
	
	//Configure period to be 256 counts for a PWM freq of 1.953 kHz
	TCC1.PER = 8000;
	
	//Channels A-D are off when they are enabled
	TCC1.CCA = 750 - 1;
	
	//Link event Channel 2 to PC2
	EVSYS.CH2MUX = EVSYS_CHMUX_PORTC_PIN2_gc;
	//Link event Channel 3 to PC3
	//EVSYS.CH3MUX = EVSYS_CHMUX_PORTC_PIN3_gc;
	
	//Enable interrupts for all priority levels
	PMIC.CTRL = PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm;
	
	//Configure Port C bits for input capture
	PORTC.DIRCLR = PIN2_bm;
	
	//Set input sense configuration for PC2
	PORTC.PIN2CTRL = PORT_OPC_PULLUP_gc | PORT_ISC_BOTHEDGES_gc;
	//Set input sense configuration for PC3
	//PORTC.PIN3CTRL = PORT_OPC_PULLUP_gc | PORT_ISC_BOTHEDGES_gc;

	//Enable input capture on Timer C0, Channel C & D
	TCC0.CTRLB = TC0_CCCEN_bm;
	
	//Configure CTRLD for input capture and specify which channel the event source is
	TCC0.CTRLD = TC_EVACT_CAPT_gc | TC_EVSEL_CH0_gc;
	
	//Enable interrupts and specify priority in INTCTRLB
	TCC0.INTCTRLB = TC_CCCINTLVL_MED_gc;	
	
	//Configure clock select bits for divide by 64
	TCC1.CTRLA = TC_CLKSEL_DIV64_gc;
	
	//Configure clock select bits for divide by 64
	TCC0.CTRLA = TC_CLKSEL_DIV64_gc;
}

void ESCDriver::stopESC(void)
{
	if(avgPWM > 850)
	{
		TCC1.CCA = 550 - 1;
	}
	else
	{
		TCC1.CCA = 750 - 1;
	}	
}

void ESCDriver::disableESC(void)
{
	TCC0.INTCTRLB = 0;	
}

void ESCDriver::enableESC(void)
{
	TCC0.INTCTRLB = TC_CCCINTLVL_MED_gc; 
}

void ESCDriver::operateESC(void)
{
	if((PORTC.IN & PIN2_bm) && wave_edge == 0)
	{
		clock_tick[0] = TCC0.CCC;
		++wave_edge;
	}
	if(!(PORTC.IN & PIN2_bm) && wave_edge == 1)
	{
		clock_tick[1] = TCC0.CCC;
		++wave_edge;
	}
	if(wave_edge == 2)
	{
		pulse_width = clock_tick[1] - clock_tick[0];
		esc_history[esc_index] = pulse_width;
		
		avgPWM = 0;
		for(uint8_t i = 0; i < 6; i++)
		{
			avgPWM = (avgPWM + esc_history[esc_index - i]) / (i + 1);
		}
		esc_index++;
		
		if(!(pesc_override->ISR_get()))
		{
			TCC1.CCA = pulse_width - 1;
		}
		wave_edge = 0;
	}
}

