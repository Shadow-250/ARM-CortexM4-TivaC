// TuningFork.c Lab 12
// Runs on LM4F120/TM4C123
// Use SysTick interrupts to create a squarewave at 440Hz.  
// There is a positive logic switch connected to PA3, PB3, or PE3.
// There is an output on PA2, PB2, or PE2. The output is 
//   connected to headphones through a 1k resistor.
// The volume-limiting resistor can be any value from 680 to 2000 ohms
// The tone is initially off, when the switch goes from
// not touched to touched, the tone toggles on/off.
//                   |---------|               |---------|     
// Switch   ---------|         |---------------|         |------
//
//                    |-| |-| |-| |-| |-| |-| |-|
// Tone     ----------| |-| |-| |-| |-| |-| |-| |---------------
//
// Daniel Valvano, Jonathan Valvano
// January 15, 2016

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2015

 Copyright 2016 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */


#include "TExaS.h"
#include "..//tm4c123gh6pm.h"

// System Control Legacy base address
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))

// PortA (APB) base address: 0x40004000
#define GPIO_PORTA_DATA_R       (*((volatile unsigned long *)0x400043FC))
#define GPIO_PORTA_DEN_R        (*((volatile unsigned long *)0x4000451C))
#define GPIO_PORTA_DIR_R        (*((volatile unsigned long *)0x40004400))
#define GPIO_PORTA_AMSEL_R      (*((volatile unsigned long *)0x40004528))
#define GPIO_PORTA_AFSEL_R      (*((volatile unsigned long *)0x40004420))
#define GPIO_PORTA_PCTL_R       (*((volatile unsigned long *)0x4000452C))
#define GPIO_PORTA_PDR_R        (*((volatile unsigned long *)0x40004514))

// SysTick Registers
#define NVIC_SYS_PRI3_R         (*((volatile unsigned long *)0xE000ED20))
#define NVIC_ST_CTRL_R          (*((volatile unsigned long *)0xE000E010))
#define NVIC_ST_RELOAD_R        (*((volatile unsigned long *)0xE000E014))
#define NVIC_ST_CURRENT_R       (*((volatile unsigned long *)0xE000E018))

// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void WaitForInterrupt(void);  // low power mode

// input from PA3, output from PA2, SysTick interrupts
void Sound_Init(void){ volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x00000001;     // 1) activate clock for Port F
  delay = SYSCTL_RCGC2_R;
	//GPIO Out
	GPIO_PORTA_DEN_R |= 0x0C;           //     Make PA2, PA3 Digital Pins
  GPIO_PORTA_DIR_R |= 0x04;           //     Make PA2 OUTPUT Pins
	GPIO_PORTA_DIR_R &= ~0x08;        // Make PA3 Input pin
	GPIO_PORTA_PDR_R |= 0x08;          //     Set Pull Down resistor for PA3
	
	// GPIO Alternate function control PA2
	GPIO_PORTA_AMSEL_R &= 0;              //     Disable Analog Mode
  GPIO_PORTA_AFSEL_R &= ~0x04;          //     Disable Alternate Function on PA2
  GPIO_PORTA_PCTL_R  &= ~0x00000F00;    //     Keep PA2 as GPIO

  // GPIO Alternate function control PA3
  GPIO_PORTA_AFSEL_R &= ~0x08;          //     Disable Alternate Function on PA3
  GPIO_PORTA_PCTL_R  &= ~0x0000F000;    //     Keep PA3 as GPIO
	
	
}

void initSysTick(unsigned long param){

    NVIC_ST_CTRL_R           =      0;              //     Disable systick on setup
    NVIC_ST_RELOAD_R         =      param - 1;
    NVIC_ST_CURRENT_R        =      0;
    NVIC_SYS_PRI3_R         |=      0x50000000;     //     Interrupt Priority 2
    NVIC_ST_CTRL_R          |=      0x07;           //     Enable interrupts with systick
}

// called at 880 Hz
void SysTick_Handler(void){
	GPIO_PORTF_DATA_R ^= 0x04;  // toggle PF2
}

int main(void){// activate grader and set system clock to 80 MHz
  TExaS_Init(SW_PIN_PA3, HEADPHONE_PIN_PA2,ScopeOn); 
  Sound_Init();         
  EnableInterrupts();   // enable after all initialization are done
  while(1){
    // main program is free to perform other tasks
    // do not use WaitForInterrupt() here, it may cause the TExaS to crash
  }
}
