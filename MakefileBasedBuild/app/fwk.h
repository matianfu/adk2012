/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef BOARD_MCK 
# define BOARD_MCK 	84000000ULL
#endif


#ifdef ADK_INTERNAL
#ifndef _ADK_FWK_H_
#define _ADK_FWK_H_

#include <stdint.h>
#include <stdlib.h>
#include "chip.h"
#include <stdio.h>
#include <math.h>



//common


	#define TICKS_PER_MS	((BOARD_MCK) / 2000)
	#define ADK_HEAP_SZ	65536

	void fwkInit(void);
        uint64_t fwkGetUptime(void);
	uint64_t fwkGetTicks(void);
	void fwkDelay(uint64_t ticks);
	
	#define delay_s(s)	fwkDelay(TICKS_PER_MS * 1000ULL * (unsigned long long)(ms))
	#define delay_ms(ms)	fwkDelay(TICKS_PER_MS * (unsigned long long)(ms))
	#define delay_us(us)	fwkDelay((TICKS_PER_MS * (unsigned long long)(ms)) / 1000ULL)

	typedef void (*PeriodicFunc)(void* data);
	void periodicAdd(PeriodicFunc f, void* data, uint32_t periodMs);
	void periodicDel(PeriodicFunc f);

	void cpuGetUniqId(uint32_t* dst);	//produce the 128-bit unique ID

	#define DMA_CHANNEL_LEDS	0	// Want a cleaner way of doing this? Get a better development environment!
	#define TIMER_FOR_ADK		0	// only in timer unit 0, // Want a cleaner way of doing this? Get a better development environment!

	uint8_t getVolume(void);
	void setVolume(uint8_t);


//GPIO stuffs

	#define PORTA(x)	(x)
	#define PORTB(x)	((x) + 32)
	#define PORTC(x)	((x) + 64)
	#define PORTD(x)	((x) + 96)
	#define PORTE(x)	((x) + 128)
	#define PORTF(x)	((x) + 160)

	#define GPIO_FUNC_GPIO	0
	#define GPIO_FUNC_A	1
	#define GPIO_FUNC_B	2

	void gpioSetFun(uint8_t pin, uint8_t func);
	void gpioSetDir(uint8_t pin, char in);
	void gpioSetVal(uint8_t pin, char on);
        char gpioGetVal(uint8_t pin);
	void gpioSetPullup(uint8_t pin, char on);


#endif
#endif


