/*
 * I2C.cpp
 *
 * Created: 12/1/2017 12:48:43 AM
 *  Author: Kevin
 */ 


#include "I2C.h"

I2C::I2C(PORT_t* pport, TWI_t*  ptwiport, uint8_t sda, uint8_t scl, SensorWriteData_t* pwrite_struct, shared_data<bool>* ptransfer_complete, shared_data<uint8_t>* pus_highbyte, shared_data<uint8_t>* pus_lowbyte)
{
	pwrite_struct = pwrite_struct;
	ptransfer_complete = ptransfer_complete;
	pus_highbyte = pus_highbyte;
	pus_lowbyte = pus_lowbyte;
	
	pport->DIRSET	= PIN0_bm | PIN1_bm;
	pport->PIN0CTRL	= PORT_OPC_WIREDANDPULL_gc; //SDA pull up output
	pport->PIN1CTRL	= PORT_OPC_WIREDANDPULL_gc; //SCL pull up output
	
	ptwiport->MASTER.BAUD=150; //baud rate is set such that TWI freq=100KHz
	
	ptwiport->CTRL=0x00; //SDA hold time off, normal TWI operation
	
	ptwiport->MASTER.CTRLA = TWI_MASTER_INTLVL_HI_gc|TWI_MASTER_RIEN_bm|TWI_MASTER_WIEN_bm|TWI_MASTER_ENABLE_bm; //enable high priority read and write interrupt, enable MASTER
	
	ptwiport->MASTER.CTRLB = 0x00; //TWI_MASTER_QCEN_bm; //no inactive bus timeout, quick command and smart mode enabled
	
	ptwiport->MASTER.CTRLC = 0x00; //initially send ACK and no CMD selected
	
	ptwiport->MASTER.STATUS |= TWI_MASTER_RIF_bm|TWI_MASTER_WIF_bm|TWI_MASTER_ARBLOST_bm|TWI_MASTER_BUSERR_bm|TWI_MASTER_BUSSTATE_IDLE_gc; //clear all flags initially and select bus state IDLE
}

void I2C::I2CInitWrite(void)
{
	ptransfer_complete->put(false);
	
	remaining_bytes = pwrite_struct->package_length;
	ptwiport->MASTER.ADDR = pwrite_struct->slave_address;
	
}

void I2C::I2CWriteDataTrans(void)
{
	if(!(TWIC.MASTER.STATUS & TWI_MASTER_RXACK_bm))      //if ACK was received
	{		
		if(remaining_bytes)
		{
			TWIC.MASTER.DATA = pwrite_struct->range_cmd;
			TWIC.MASTER.STATUS |= PIN6_bm;
			remaining_bytes--;
		}
		else
		{
			I2CNackStop();
			TWIC.MASTER.STATUS |= PIN6_bm;
			ptransfer_complete->ISR_put(true);
		}
	}
}

void I2C::I2CInitRead(void)
{
	ptransfer_complete->put(false);
	
	remaining_bytes = 2;
	ptwiport->MASTER.ADDR = (pwrite_struct->slave_address) + 1;
}

void I2C::I2CReadDataTrans(void)
{
	data = TWIC.MASTER.DATA;
	
	if(remaining_bytes > 1)
	{
		pus_highbyte->ISR_put(data);
		TWIC.MASTER.CTRLC = 1 << TWI_MASTER_CMD1_bp; //send ACK and Byte Receive
	}
	else
	{
		pus_lowbyte->ISR_put(data);
	    I2CNackStop(); //send NACK and stop condition if all bytes are transferred
		TWIC.MASTER.STATUS |= PIN6_bm;
		ptransfer_complete->ISR_put(true);
	}
	remaining_bytes = remaining_bytes - 1;
}

void I2C::I2CAckStop(void)
{
	ptwiport->MASTER.CTRLC = (1<<TWI_MASTER_CMD1_bp) | (1<<TWI_MASTER_CMD0_bp); //send stop condition with an ACK
}

void I2C::I2CNackStop(void)
{
	ptwiport->MASTER.CTRLC = TWI_MASTER_ACKACT_bm | (1<<TWI_MASTER_CMD1_bp) | (1<<TWI_MASTER_CMD0_bp);
}

