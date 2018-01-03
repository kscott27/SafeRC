/*
 * IncFile1.h
 *
 * Created: 11/29/2017 12:58:11 PM
 *  Author: Kevin
 */ 


#ifndef ESCDriver_H_
#define ESCDriver_H_

#include <stdio.h>
#include <avr/io.h>                         // Port I/O for SFR's
#include <avr/interrupt.h>

#include "frt_shared_data.h"


//#include "emstream.h"


class ESCDriver
{
	protected:

	uint32_t clock_tick[2];
	uint8_t wave_edge;
	uint16_t pulse_width;
	uint16_t esc_history[256];
	uint8_t esc_index;
	uint16_t avgPWM;
		
	public:
	
	shared_data<bool>* pesc_override;
	
	ESCDriver (shared_data<bool>* pesc_override);

	void stopESC(void);
	
	void disableESC(void);
	
	void enableESC(void);
	
	void operateESC(void);
	
	void reverseESC(void);
	
};



#endif /* ESCDriver_H_ */