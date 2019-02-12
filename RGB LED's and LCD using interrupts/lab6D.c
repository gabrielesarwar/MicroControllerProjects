#include <stdint.h>
//#include <msp.h>
#include "C:\Users\maver\Documents\School\Year 3\SYST3310\Labs\inc\BSP.h"
#include "C:\Users\maver\Documents\School\Year 3\SYST3310\Labs\inc\msp432p401r.h"
#include "C:\Users\maver\Documents\School\Year 3\SYST3310\Labs\inc\CortexM.h"

volatile uint8_t p3count = 0;
volatile uint8_t p5count=0;

void WaitForInterrupt(void);
void button_Init(void){
	__ASM("CPSID I");
	
	//configure Button S1 as GPIO
	P5SEL0 &= ~BIT1;
	P5SEL1 &= ~BIT1;
	P5DIR &= ~BIT1; //set direction to input 
	P5REN &= BIT1;  //disable pull resistors
	P5OUT |= BIT1;  //pull up 
	P5IES |= BIT1;
	P5IFG &= BIT1;
	P5IE |= BIT1;
	
	
	
	//configure Button S2 as  GPIO 0010 0000
	P3SEL0 &= ~BIT5;
	P3SEL1 &= ~BIT5;
	P3DIR &= ~BIT5; //set direction to input 
	P3REN &= BIT5;  //disable pull resistors
	P3OUT |= BIT5;  //pull up 
	P3IES |= BIT5;
	P3IFG &= BIT5;
	P3IE |= BIT5;
	
	
	NVIC_IPR9 = (NVIC_IPR9 & 0x00FFFFFF) | 0x60006000;
	NVIC_ISER1 = 0x00000080;
	
	//NVIC_IPR9 = (NVIC_IPR9 & 0x00FF00FF) | 0x00006000;
//	NVIC_ISER1 = 0x00000020;
//	EnableInterrupts();
	__ASM("CPSIE I");
}

void PORT3__IRQHandler(void){
		if((P3IFG & BIT5) != 0x00){
		P3IFG &= ~BIT5;
		p3count ++;
		//BSP_LCD_DrawString(0,6,"B1", BSP_LCD_Color565(255,0,0));
		BSP_LCD_SetCursor(0,6);
		BSP_LCD_OutUDec(p3count, BSP_LCD_Color565(255,0,0));
		}
}

void PORT5__IRQHandler(void){
		if((P5IFG & BIT1) != 0x00){
		P5IFG &= ~BIT1;
		p5count ++;
		//BSP_LCD_DrawString(2,6,"B2", BSP_LCD_Color565(0,255,0));
		BSP_LCD_SetCursor(2, 6 );
		BSP_LCD_OutUDec(p5count, BSP_LCD_Color565(0,255,0));
		}
}
int main(void) {
	
	WDTCTL = WDTPW | WDTHOLD;
	button_Init();
	BSP_LCD_Init();
//while(1){
	BSP_LCD_DrawString(0,2,"B1", BSP_LCD_Color565(255,0,0));
	BSP_LCD_SetCursor(0,6);
	BSP_LCD_OutUDec(p3count, BSP_LCD_Color565(255,0,0));
	
  
		
	BSP_LCD_DrawString(2,2,"B2", BSP_LCD_Color565(0,255,0));
	BSP_LCD_SetCursor(2, 6 );
	BSP_LCD_OutUDec(p5count, BSP_LCD_Color565(0,255,0));
	while(1);
	
	return 0;
//}
}
