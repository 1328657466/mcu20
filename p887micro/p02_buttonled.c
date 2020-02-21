/****************************************************
 *	PRACTICA #2	:	Control Boton		LED2		*
 *	Paso1:	Definir estado Boton y pullup		 	*
 *	Paso2:	Aplicar al control del LED2				*
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
#define _XTAL_FREQ 4000000		//Tcy = 1uS
#define LED1pin PORTEbits.RE2
#define LED2pin PORTEbits.RE0
#define BUT1pin	PORTAbits.RA4
#include <pic16f887.h>
#include <pic16f887.h>
#include "peripheral.h"
//VAR GLOBAL
enum led {LEDOFF, LEDON} led1st;
enum but {RELEASE, PRESSED} but1st; 
char but1OK = false;
void TaskLED1(void);
void TaskBUT1(void);
void main()
{
	OSCSetup();
	ANSEL = 0;		
	ANSELH = 0;
	TRISEbits.TRISE2 = 0;
	TRISEbits.TRISE0 = 0;
	LED1pin = 0;
	LED2pin = 0;
	led1st = LEDOFF;
	but1st = RELEASE;
	while(true)
	{
		TaskLED1();
		TaskBUT1();
		if(but1OK)
		{
			but1OK = 0;
			LED2pin = !LED2pin;
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
void TaskBUT1(void)
{
	static unsigned int cnt = 0;
	switch(but1st)
	{
		case RELEASE:
		{
			if(BUT1pin == 0)
			{
				cnt = cnt + 1;
				if(cnt > 200)
				{
					but1st = PRESSED;
					cnt = 0;
				}
			}
			else cnt = 0;
		} break;
		case PRESSED:
		{	
			if(BUT1pin)
			{
				cnt = cnt + 1;
				if(cnt > 200)
				{
					but1st = RELEASE;
					cnt = 0;
					but1OK = 1;
				}
			}
			else cnt = 0;
		} break;
	}
}
