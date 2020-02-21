/****************************************************
 *	APLICACION #1	:	Uso de leds 7 Segmento		*
 *	Paso1:	Determinar tiempo con TMR2			 	*
 *	Paso2:	Multiplexar segmentos D0-D3				*
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
#define DIS4pin	PORTCbits.RC2
#define DIS3pin	PORTCbits.RC3
#define DIS2pin	PORTCbits.RC4
#define DIS1pin	PORTCbits.RC5

#include <pic16f887.h>
#include <pic16f887.h>
#include "peripheral.h"
//VAR GLOBAL
char dismap[10] = {0x2F,0x06,0x5B,0x4F,0x66,0x6D,0x7C,0x07,0x7F,0x67};
char dis1val, dis2val, dis3val, dis4val;
unsigned int cnt = 0, cntms = 0;
enum led {LEDOFF, LEDON} led1st;
enum dis {DIS1, DIS2, DIS3, DIS4} dis1st;
void TaskLED1();
void TaskDIS1();
void ConvertDIS(unsigned int val);

// Addon Extra para control de tiempo con ISR
/*void interrupt isr()
{
	if(PIR1bits.TMR2IF)
	{
		PIR1bits.TMR2IF = 0;
		TaskDIS1();
	}
}*/

void main()
{
	OSCSetup();
	ANSEL = 0;		
	ANSELH = 0;
	TRISEbits.TRISE2 = 0;
	LED1pin = 0;
	TRISD = 0x00; //Todos los pines como salida seg a,b,c,d,e,f,g,.
	PORTD = 0x00; //Apaga todos los segmentos
	TRISC = 0x00; //Todos los pines como salida disp 1,2,3,4
	PORTC = 0x00; //Desactiva todos los display
	led1st = LEDOFF;
	dis1st = DIS1;
	ConvertDIS(0);
	
	//Addon Extra para control de tiempo con ISR
	/*TMR2Setup(T2PRE4, T2POS10); //5000uS = 1uS x 4 x 10 x 125
	PR2 = 125;
	TMR2Start();
	INTCONbits.PEIE = 1;
	PIE1bits.TMR2IE = 1;
	INTCONbits.GIE = 1;*/
	
	while(true)
	{
		TaskLED1(); //Destello LED
		TaskDIS1(); //Actualiza Display
		__delay_ms(1);
		cntms ++;
		if(cntms > 250)
		{
			cntms = 0;
			cnt ++;
			ConvertDIS(cnt);
		}
	}
}

void TaskLED1()
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
void TaskDIS1()
{
	switch(dis1st)
	{
		case DIS1:
			DIS4pin = 0;
			PORTD = dismap[dis1val];
			DIS1pin = 1;
			break;
		case DIS2:
			DIS1pin = 0;
			PORTD = dismap[dis2val];
			DIS2pin = 1;
			break;
		case DIS3:
			DIS2pin = 0;
			PORTD = dismap[dis3val];
			DIS3pin = 1;
			break;
		case DIS4:
			DIS3pin = 0;
			PORTD = dismap[dis4val];
			DIS4pin = 1;
			break;
	}
}
void ConvertDIS(unsigned int val)
{
	dis1val = 0;
	dis2val = 0;
	dis3val = 0;
	dis4val = 0;
	if(val > 1000) 
	{
		dis1val = val / 1000;
		val = val % 1000;
	}
	if(val > 100)
	{
		dis2val = val / 100;
		val = val % 100;
	}
	if(val > 10)
	{
		dis3val = val / 10;
		dis4val = val % 10;
	}
	else dis4val = val;
}