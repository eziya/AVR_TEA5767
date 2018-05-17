﻿/*
 * TEA5767.h
 *
 * Created: 2018-05-15 오후 3:49:49
 *  Author: kiki
 */ 


#ifndef TEA5767_H_
#define TEA5767_H_

#include <avr/io.h>
#include <stdbool.h>

#define TEA5767_DEVICE_ADDR		0x60
#define TEA5767_SCAN_UP			1		
#define TEA5767_SCAN_DOWN		0
#define TEA5767_MIN_FREQ		76.0
#define TEA5767_MAX_FREQ		108.0

typedef struct
{
	double	freqMHZ;
	bool	muteFlag;
	bool	readyFlag;
	bool	bandLimitFlag;
	bool	stereoFlag;
	uint8_t signalLevel;	
} _radioStatus;

extern _radioStatus radioStatus;

bool TEA5767_Init(double freqMHz);
bool TEA5767_SetFreq(double freqMHz);
bool TEA5767_ScanFreq(uint8_t updown);
bool TEA5767_Mute(bool on);
bool TEA5767_GetStatus();

#endif /* TEA5767_H_ */