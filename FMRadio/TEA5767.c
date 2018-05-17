/*
 * TEA5767.c
 *
 * Created: 2018-05-15 오후 3:49:59
 *  Author: kiki
 */ 

#include "TEA5767.h"
#include "128A_TWI.h"

_radioStatus radioStatus;
uint8_t	radioTxBuffer[5];
uint8_t	radioRxBuffer[5];

static bool TEA5767_WriteBuffer(uint8_t *buffer);
static bool TEA5767_ReadBuffer(uint8_t *buffer);

bool TEA5767_Init(double freqMHz)
{
	TWI_Init();
	
	if(freqMHz < TEA5767_MIN_FREQ)	freqMHz = TEA5767_MIN_FREQ;
	if(freqMHz > TEA5767_MAX_FREQ)	freqMHz = TEA5767_MAX_FREQ;
	
	/* Calculate PLL value */
	uint16_t PLL = 4 * (freqMHz * 1000000 + 225000) / 32768;
	
	radioTxBuffer[0] = PLL >> 8;		/* Freq. High */
	radioTxBuffer[1] = PLL & 0xFF;		/* Freq. Low */
	radioTxBuffer[2] = 0x70;			/* Search Down, Level High, High Injection */
	radioTxBuffer[3] = 0x12;			/* XTAL, SNC */
	radioTxBuffer[4] = 0x40;			/* DTC */
	
	if(!TEA5767_WriteBuffer(radioTxBuffer)) return false;
	
	return true;
}

bool TEA5767_SetFreq(double freqMHz)
{
	if(freqMHz < TEA5767_MIN_FREQ)	freqMHz = TEA5767_MIN_FREQ;
	if(freqMHz > TEA5767_MAX_FREQ)	freqMHz = TEA5767_MAX_FREQ;
	
	/* Calculate PLL value */
	uint16_t PLL = 4 * (freqMHz * 1000000L + 225000) / 32768;
	
	radioTxBuffer[0] = PLL >> 8;
	radioTxBuffer[1] = PLL & 0xFF;
	
	if(!TEA5767_WriteBuffer(radioTxBuffer)) return false;
	
	return true;
}

bool TEA5767_ScanFreq(uint8_t updown)
{
	if(!TEA5767_GetStatus()) return false;
		
	double currentFreq = radioStatus.freqMHZ;	
	
	if(updown == TEA5767_SCAN_UP)
	{
		currentFreq += 0.1;
		uint16_t PLL = 4 * (currentFreq * 1000000L + 225000) / 32768;
		radioTxBuffer[0] = (PLL >> 8) | 0x40;
		radioTxBuffer[1] = PLL & 0xFF;		
		radioTxBuffer[2] |= 0x80;
	}
	else
	{
		currentFreq -= 0.1;
		uint16_t PLL = 4 * (currentFreq * 1000000L + 225000) / 32768;
		radioTxBuffer[0] = (PLL >> 8) | 0x40;
		radioTxBuffer[1] = PLL & 0xFF;
		radioTxBuffer[2] &= ~0x80;
	}
	
	if(!TEA5767_WriteBuffer(radioTxBuffer)) return false;
	
	return true;
}

bool TEA5767_Mute(bool on)
{	
	if(on)
	{
		radioTxBuffer[0] |= 0x80;		
	}
	else
	{
		radioTxBuffer[0] &= ~0x80;
	}
	
	if(!TEA5767_WriteBuffer(radioTxBuffer)) return false;
	
	return true;
}

bool TEA5767_GetStatus()
{
	if(!TEA5767_ReadBuffer(radioRxBuffer)) return false;
	
	uint16_t PLL = ((radioRxBuffer[0] & 0x3F) << 8) | (radioRxBuffer[1]);
	
	radioStatus.readyFlag = (radioRxBuffer[0] & 0x80 ? true : false);
	radioStatus.bandLimitFlag = (radioRxBuffer[0] & 0x40 ? true : false);
	radioStatus.freqMHZ = (double)(PLL * 32768 / 4 - 225000) / (double)1000000;
	radioStatus.stereoFlag = (radioRxBuffer[2] & 0x80 ? true : false);
	radioStatus.signalLevel = (radioRxBuffer[3] >> 4) & 0x0F;
	
	return true;
}

static bool TEA5767_WriteBuffer(uint8_t *buffer)
{
	return TWI_TxBuffer(TEA5767_DEVICE_ADDR, buffer, 5);
}

static bool TEA5767_ReadBuffer(uint8_t *buffer)
{
	return TWI_RxBuffer(TEA5767_DEVICE_ADDR, buffer, 5);
}
