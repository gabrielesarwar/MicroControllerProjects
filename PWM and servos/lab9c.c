//*****************************************************************************
//
// MSP432 main.c  - P2.5 port incremental-cycle for servo demo on MKII
// TA0CCR2 is the length of the low pulse
//    Smaller the value, the shorter the low pulse and the longer the high pulse
//    Theory: Need 2 ms high-pulse for 90 degrees, 1 ms high-pulse for -90 degrees, 1.5 ms high-pulse for 0 degrees
//		Practice: Somewhat different.
//    Note: Tried with SMCLK = 3 MHz, but worked reliably when reprogrammed to 8 MHz
//
//*****************************************************************************

//#include "msp.h"
#include "../inc/msp432p401r.h"
#include "../inc/BSP.h"
#include "../inc/CortexM.h"
#include <stdint.h>


#define PWM_PERIOD 40000
#define PWM_PULSE_MAX  20000		  // Experienced Value = Maximum high pulse is 2 ms for +90 degrees
#define PWM_PULSE_MIN	 4500		// Experienced Value = Maximum high pulse is 2 ms for -90 degrees
#define PWM_PULSE_MID  12000

#define DELAY 800
volatile uint32_t high = PWM_PERIOD/10;
volatile _Bool interpt1 = 0;			//to check if switch1 clicked twice
volatile _Bool interpt2 = 0;		//to check if switch2 clicked twice

void Interrupt_Init(void){
	
	__ASM("CPSID I");

	//Switch 1 iNIT
  P5SEL1 &= ~BIT1;
	P5SEL0 &= ~BIT1;
	P5DIR &= ~BIT1;
	P5REN |= BIT1;
	P5OUT |= BIT1;                   // P5.1 is pull-up
  P5IES &= ~BIT1;                  
  P5IFG &= ~BIT1;                  // clear flag4 and flag1 (reduce possibility of extra interrupt)
  P5IE |= BIT1;                    // arm interrupt on P1.4 and P1.1
	
	// Switch 2 INIT
	P3SEL1 &= ~BIT5;
	P3SEL0 &= ~BIT5;
	P3DIR &= ~BIT5;
	P3REN |= BIT5;
	P3OUT |= BIT5;                   // P3.5 is are pull-up
  P3IES &= ~BIT5;                   // P3.5 is falling edge event
  P3IFG &= ~BIT5;                  // clear flag (reduce possibility of extra interrupt)
  P3IE |= BIT5;                    // arm interrupt
	
	//interrupt init
	NVIC_IPR9 = (NVIC_IPR9&0x00FF00FF)|0x60000000;	// priority 3
  NVIC_ISER1 = 0x000000A0;         // enable interrupt 37 in NVIC
	__ASM("CPSIE I");	
}


void PORT5_IRQHandler(void){
	if(P5IFG&0x02){
		P5IFG &= ~0x02;
		
		uint32_t i;

		if(interpt1 == 0){
			interpt1 = 1;
			while (high < PWM_PULSE_MAX)		// CW: to -90
			{
				TA0CCR2 = PWM_PERIOD - high;
				high += 2;
				for(i=DELAY; i>0; i--);	
			}
		}else{
			interpt1 = 0;
			return;
		}
	}
}


void PORT3_IRQHandler(void){
	if(P3IFG&0x20){
		P3IFG &= ~0x20;
		uint32_t i;
		
		if(interpt2 == 0){
			interpt2 = 1;
			while (high > PWM_PULSE_MIN)		// CW: to +90
			{
				TA0CCR2 = PWM_PERIOD - high;
				high -= 2;
				for(i=DELAY; i>0; i--);
			}
		}else {
			interpt2 = 0;
			return;
		}
	}
}

void PORT4_IRQHandler(void){
	if(P4IFG & 0x02){
		P4IFG &= ~0x02;
		uint32_t i;
		
		while (high > PWM_PULSE_MID)		// CW: to +90
		{
				TA0CCR2 = PWM_PERIOD - high;
				high -= 2;
				for(i=DELAY; i>0; i--);
		}
		
		while (high < PWM_PULSE_MID)		// CW: to -90
		{
				TA0CCR2 = PWM_PERIOD - high;
				high += 2;
				for(i=DELAY; i>0; i--);
		}		
	}
}

void BSP_Joystick_Int_Init(void){
	P4IFG &= ~0x0002;
	P4IE |= 0x02;
	
	NVIC_IPR9 = (NVIC_IPR9&0xFFFFFF00) | 0x00000040;
	NVIC_ISER1 = 0x00000040;
}




int main(void)
{	
		WDTCTL = WDTPW | WDTHOLD;
		uint32_t i;
         
		BSP_Joystick_Init();
	  BSP_Joystick_Int_Init();
		Interrupt_Init();
	  P2DIR |= BIT5;			// Configure P12.5 as output
	  P2SEL1 &= ~BIT5;	
	  P2SEL0 |= BIT5;			// Set P2.5 as TA0.2 PWM output pin (Primary module function)
	
		CSKEY = CSKEY_VAL;
	  CSCTL0 = 0;
	  CSCTL0 = DCOEN | DCORSEL_2 | 0x485;     	 // (From Bia, page 667)
	
		TA0CCTL2 = OUTMOD_3 | CCIE;
	  TA0CTL = TASSEL_2 | MC_1 | TACLR;		// SMCLK = 3 MHz, Divide by 1, Up Mode
		TA0CCR0 = PWM_PERIOD;
		TA0CCR2 = PWM_PULSE_MAX;

		while (1){}
}
