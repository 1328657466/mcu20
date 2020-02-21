/****************************************************
 *	PRACTICA #9	:	Lectura ADC + POT				*
 ****************************************************/

#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config MCLRE = ON       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown Out Reset Selection bits (BOR enabled)
#pragma config IESO = ON        // Internal External Switchover bit (Internal/External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)
#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <pic16f887.h>
#define _XTAL_FREQ 4000000		//Tcy = 1uS
#define LED1pin PORTEbits.RE2
#define LED2pin PORTEbits.RE0
#define LED3pin	PORTDbits.RD0	//Add program 7
#define POT1pin	PORTAbits.RA0	//Add program 9
#define LDR1pin	PORTAbits.RA1	//Add program 9
#define BUT2pin	PORTBbits.RB0	//Add program 7
#include "peripheral.h"
//VAR GLOBAL
enum led {LEDOFF, LEDON} led1st;
enum but {RELEASE, PRESSED} but2st; 
enum adc {STOP, START, WAIT} adc1st;
char but2OK = false, tmrOK = false, tmrcnt = 0;
unsigned int adcvalue; //Add program 9
void TaskLED1(void);
void TaskBUT2(void);
void TaskADC1(void);
void interrupt isr()
{
	if(INTCONbits.T0IF)
	{
		INTCONbits.T0IF = 0;
		TMR0Setval(100);
		tmrOK = true;
	}
}
void main()
{
	OSCSetup();
	ANSEL = 0;		
	ANSELH = 0;
	ANSELbits.ANS0 = 1;	//Add program 09
	ANSELbits.ANS1 = 1; //Add program 09
	ADCSetup();
	ADCON0bits.GO;
	TRISEbits.TRISE2 = 0;
	TRISEbits.TRISE0 = 0;
	TRISDbits.TRISD0 = 0; //add P07
	EnablePU();	//Add P07
	LED1pin = 0;
	LED2pin = 0;
	LED3pin = 0;
	led1st = LEDOFF;
	but2st = RELEASE;
	adc1st = STOP;	//add program 09
	TMR0Setup(TIMER, T0PRE64);
	//1uS * 1:64 * (256-TMR0) = 10mS
	TMR0Setval(100);
	INTCONbits.T0IE = 1;
	INTCONbits.GIE = 1;
	USARTSetup(9600);
	while(true)
	{
		TaskLED1();
		TaskBUT2();
		TaskADC1();
		if(but2OK)
		{
			but2OK = 0;
			LED2pin = !LED2pin;
		}
		if(tmrOK) //Add P07 mod P09 para 1Seg
		{
			tmrOK = 0;
			tmrcnt ++;
			if(tmrcnt > 99)
			{
				adc1st = START;
				tmrcnt = 0;
			}
			
		}
		__delay_ms(1);
	}
}

void TaskLED1(void)
{
	static unsigned int cnt = 0;
	switch(led1st)
	{
		case LEDOFF:
		{
			
			cnt = cnt + 1;
			if(cnt == 500)
			{
				cnt = 0;
				LED1pin = 1;
				led1st = LEDON;
			}
		}
		break;
		case LEDON:
		{
			cnt = cnt + 1;
			if(cnt == 500)
			{
				cnt = 0;
				LED1pin = 0;
				led1st = LEDOFF;
			}
		}
		break;
	}
}
void TaskBUT2(void)
{
	static unsigned int cnt = 0;
	switch(but2st)
	{
		case RELEASE:
		{
			if(BUT2pin == 0)
			{
				cnt = cnt + 1;
				if(cnt > 200)
				{
					but2st = PRESSED;
					cnt = 0;
				}
			}
			else cnt = 0;
		} break;
		case PRESSED:
		{	
			if(BUT2pin)
			{
				cnt = cnt + 1;
				if(cnt > 200)
				{
					but2st = RELEASE;
					cnt = 0;
					but2OK = 1;
				}
			}
			else cnt = 0;
		} break;
	}
}
void TaskADC1(void)
{
	switch(adc1st)
	{
		case STOP:
		{
			NOP();
		} break;
		case START:
		{
			ADCStart(0);
			LED3pin = 1;
			adc1st = WAIT;
		} break;
		case WAIT:
		{
			if(ADCBusy() == 0)
			{
				adcvalue = ADCRead();
				LED3pin = 0;
				printf("ADC:%u", adcvalue);
				adc1st = STOP;
			}
		} break;
	}
}