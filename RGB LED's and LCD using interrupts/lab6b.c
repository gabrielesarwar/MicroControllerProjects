#include <stdint.h>
//#include <msp.h>
#include "C:\Users\maver\Documents\School\Year 3\SYST3310\Labs\inc\BSP.h"
#include "C:\Users\maver\Documents\School\Year 3\SYST3310\Labs\inc\msp432p401r.h"
#include "C:\Users\maver\Documents\School\Year 3\SYST3310\Labs\inc\CortexM.h"

//void delay(void){
	//uint32_t i, j, k;
	//for(i = 1000; i > 0; i--){
	
//		for(j = 1000; j > 0; j--){
//		}
//	}
//}
void button_Init(void){
	__ASM("CPSID I");
	
	P2SEL1 &= ~(BIT4 | BIT6);
	P2SEL0 &= ~(BIT4 | BIT6);
	P2DIR |= BIT4 | BIT6;
  P2OUT &= ~(BIT4 | BIT6); //clear the RGB
	//configure Button S1 as GPIO
	P5SEL0 &= ~BIT1;
	P5SEL1 &= ~BIT1;
	P5DIR &= ~BIT1; //set direction to input 
	P5REN |= BIT1;  //disable pull resistors
	P5OUT |= BIT1;  //pull up 
		
	
	
	//configure Button S2 as  GPIO 0010 0000
	P3SEL0 &= ~BIT5;
	P3SEL1 &= ~BIT5;
	P3DIR &= ~BIT5; //set direction to input 
	P3REN |= BIT5;  //disable pull resistors
	P3OUT |= BIT5;  //pull up 

	
	
	NVIC_IPR9 = (NVIC_IPR9 & 0x00FF00FF) | 0x60000000;
	NVIC_ISER1 = 0x000000A0;
	EnableInterrupts();
	__ASM("CPSIE I");
}

void PORT3__IRQHandler(void){ //turn red on 2.6
	
		P3IFG &= BIT5;
		P2OUT &= ~BIT4;
	  P2OUT ^= BIT6;
}

void PORT5__IRQHandler(void){ //turn green on 2.4
	
		P5IFG &= BIT1;
		P2OUT &= ~BIT6;
	  P2OUT = P2OUT ^ BIT4;
}
int main(void) {
	
	WDTCTL = WDTPW | WDTHOLD;
	button_Init();
	while(1){
		if((P5IN & BIT1) == 0x00){
			P2OUT &= ~BIT6;
			P2OUT ^= BIT4;
			for(int i = 0; i < 75000; i++);
		}
		if((P3IN & BIT5) == 0x00){
			P2OUT &= ~BIT4;
			P2OUT ^= BIT6;
			for(int i = 0; i < 75000; i++);
		}
	}
											
	//return 0;
}
