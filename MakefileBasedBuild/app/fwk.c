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
#define ADK_INTERNAL
#include "fwk.h"
#include "dbg.h"






typedef struct PeriodicNode{
    struct PeriodicNode* next;
    PeriodicFunc pF;
    void* pD;
    uint32_t count;
    uint32_t reload;
}PeriodicNode;



volatile static uint64_t msec = 0;
volatile static PeriodicNode* periodics = 0;

static uint8_t volume = 255;

#define JOINER(x,y,z)	x ## y ## z
#define CALLER(x,y,z)	JOINER(x,y,z)
#define TIMER_NAME	CALLER(TC, TIMER_FOR_ADK,  _IrqHandler)
#define IRQ_NAME	CALLER(TC, TIMER_FOR_ADK,  _IRQn)

void TIMER_NAME(){

    uint32_t unused = TC0->TC_CHANNEL[TIMER_FOR_ADK].TC_SR;
    PeriodicNode* n = periodics;

    msec++;

    while(n){

        if(!--n->count){

            n->pF(n->pD);
            n->count = n->reload;
        }
        n = n->next;
    }
}

uint64_t fwkGetUptime(void){

    uint64_t t;

    do{
        t = msec;
    }while(t != msec);

    return t;
}

uint64_t fwkGetTicks(void){

    uint64_t hi;
    uint32_t lo;

    do{
        lo = TC0->TC_CHANNEL[TIMER_FOR_ADK].TC_CV;
        hi = msec;
    }while(lo > TC0->TC_CHANNEL[TIMER_FOR_ADK].TC_CV);

    return (hi * TICKS_PER_MS) + lo;
}

void fwkDelay(uint64_t ticks){

    uint64_t start = fwkGetTicks();

    while(fwkGetTicks() - start < ticks);
}

void periodicAdd(PeriodicFunc f, void* data, uint32_t periodMs){

    PeriodicNode* n = malloc(sizeof(PeriodicNode));
    uint32_t failed = 0;

    n->reload = n->count = periodMs;
    n->pF = f;
    n->pD = data;

    do{
        asm(
             "ldrex r0, [%1]	 \n\t"	//atomic linked list addition..booyah!
             "str   r0, [%2]     \n\t"
             "strex %0, %3, [%1] \n\t"
             :"=r"(failed)
             :"r"(&periodics), "r"(&n->next), "r"(n)
             :"r0"
        );
    }while(failed);
}

void periodicDel(PeriodicFunc f){

    uint32_t failed;

    PeriodicNode* p = 0;
    PeriodicNode* n = periodics;

    while(n && n != f){

        p = n;
        n = n->next;
    }
    if(!n) return; //nothing to delete

    if(p) p->next = n->next;
    else periodics = n->next;
    free(n);
}

static void __attribute__((naked)) cpuGetUniqIdFunc(uint32_t interfaceAddr, uint32_t cmd1, uint32_t cmd2, uint32_t* buf){

    asm(
        "    cpsid i			\n\t"   // There is some pretty scary stuff that can happen
        "    mov r12, r2		\n\t"   //  if you reset the device while this code is in
        "    str r1, [r0, #4]		\n\t"   //  the middle of doing its thing. Sometimes the
        "lbl_wait_1:			\n\t"   //  flash controller does not get reset to "code"
        "    ldr r1, [r0, #8]		\n\t"   //  mode and stays in "read unique ID" mode, even
        "    lsrs r1, #1		\n\t"   //  when the chip itself is externally reset. This
        "    bcs lbl_wait_1		\n\t"   //  will cause the unique ID to be treated like the
        "    mov r2, #0x00080000	\n\t"   //  vector table. Needless to say: the chip unique
        "    ldr r1, [r2, #0]		\n\t"   //  ID is not a suitable vector table. After reset,
        "    str r1, [r3, #0]		\n\t"   //  this will likely cause the chip to take a hard
        "    ldr r1, [r2, #4]		\n\t"   //  fault immediately. Worse yet, the hard fault
        "    str r1, [r3, #4]		\n\t"   //  handler vector will also be invalid, causing
        "    ldr r1, [r2, #8]		\n\t"   //  the chip to take yet another hard fault. This
        "    str r1, [r3, #8]		\n\t"   //  will continue forever. The debug bridge, as it
        "    ldr r1, [r2, #12]		\n\t"   //  currently is, cannot handle this chip state.
        "    str r1, [r3, #12]		\n\t"   //  It will be unable to flash or debug the chip,
        "    str r12, [r0, #4]		\n\t"   //  and you - the user - will be sad. The practical
        "lbl_wait_2:			\n\t"   //  upshot of all this: use this code rarely, and
        "    ldr r1, [r0, #8]		\n\t"   //  if you interrupt it, power-cycle the system if
        "    lsrs r1, #1		\n\t"   //  you suspect that you somehow got into this
        "    bcc lbl_wait_2		\n\t"   //  weird state. It is possible that future revs
        "    cpsie i			\n\t"   //  of SAM3X will fix this issue by properly
        "    bx  lr			\n\t"   //  resetting the flash controller at reset time.
    );
}

void cpuGetUniqId(uint32_t* dst){	//produce the 128-bit unique ID

    const unsigned numInstr = 23; //it better match the above function...
    uint16_t buffer[numInstr];
    uint16_t* src = (uint16_t*)(((uint32_t)&cpuGetUniqIdFunc) &~ 1);
    unsigned i;

    for(i = 0; i < numInstr; i++) buffer[i] = *src++;

    ((void (*)(uint32_t,uint32_t,uint32_t,uint32_t*))(((uint32_t)buffer) + 1))(0x400E0A00, 0x5A00000E, 0x5A00000F, dst);
}

void fwkInit(void){

    //disable WDT
    WDT_Disable( WDT ) ;
   
    //clock & periodic setup (MCLK/2, interrupt every ms)
    PMC_EnablePeripheral(ID_TC0);
    TC0->TC_CHANNEL[TIMER_FOR_ADK].TC_CMR = TC_CMR_TCCLKS_TIMER_CLOCK1 | TC_CMR_WAVSEL_UP_RC | TC_CMR_WAVE;
    TC0->TC_CHANNEL[TIMER_FOR_ADK].TC_CV = 0;
    TC0->TC_CHANNEL[TIMER_FOR_ADK].TC_RC = TICKS_PER_MS;
    TC0->TC_CHANNEL[TIMER_FOR_ADK].TC_IER = TC_IER_CPCS;
    TC0->TC_CHANNEL[TIMER_FOR_ADK].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
    NVIC_EnableIRQ(IRQ_NAME);

    //IO on
    PMC_EnablePeripheral(ID_PIOC);
    PMC_EnablePeripheral(ID_PIOB);
    PMC_EnablePeripheral(ID_PIOA);

    //dma setup
    PMC_EnablePeripheral(ID_DMAC);
    DMAC->DMAC_EBCIDR = 0x003F3F3F;	//disable all interrupts
    DMAC->DMAC_CHDR = 0x0000003F;	//disable all channels
    DMAC->DMAC_EN = 1;
}

#define PER	0
#define PDR	1
#define OER	4
#define ODR	5
#define SODR	12
#define CODR	13
#define PDSR	15
#define PUDR	24
#define PUER	25
#define ABSR	28

static inline volatile uint32_t* gpioGetPort(uint8_t pin){

    uint32_t addr = pin >> 5;
    addr <<= 9;
    addr += 0x400E0E00;

    return (volatile uint32_t*)addr;
}

static inline uint32_t gpioGetBit(uint8_t pin){

    return 1UL << (pin & 31);
}

void gpioSetFun(uint8_t pin, uint8_t func){

    volatile uint32_t* port = gpioGetPort(pin);
    uint32_t bit = gpioGetBit(pin);

    if(func == GPIO_FUNC_GPIO){

        port[PER] = bit;
    }
    else{

        port[PDR] = bit;
        if(func == GPIO_FUNC_A) port[ABSR] &=~ bit;
        else port[ABSR] |= bit;
    }
}

void gpioSetDir(uint8_t pin, char in){

    volatile uint32_t* port = gpioGetPort(pin);
    uint32_t bit = gpioGetBit(pin);

    if(in) port[ODR] = bit;
    else port[OER] = bit;
}

void gpioSetVal(uint8_t pin, char on){

    volatile uint32_t* port = gpioGetPort(pin);
    uint32_t bit = gpioGetBit(pin);

    if(on) port[SODR] = bit;
    else port[CODR] = bit;
}

char gpioGetVal(uint8_t pin){

    volatile uint32_t* port = gpioGetPort(pin);
    uint32_t bit = gpioGetBit(pin);

    return (port[PDSR] & bit) != 0;
}

void gpioSetPullup(uint8_t pin, char on){

    volatile uint32_t* port = gpioGetPort(pin);
    uint32_t bit = gpioGetBit(pin);

    if(on) port[PUER] = bit;
    else port[PUDR] = bit;
}

uint8_t getVolume(void){

    return volume;
}

void setVolume(uint8_t vol){

    volume = vol;
}



