/****************************************************
 *	APLICACION #3	:	Menu y LCD Encoder			*
 *	Paso1:	Crear Menu LCD de 1 Nivel				*
 *	Paso2:	Agregar control Encode a Menu			*
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
#define LED1pin PORTEbits.RE2	//Pin del LED1
#define BUT1pin PORTBbits.RB0	//Pulsador del Encoder
#define ENCApin	PORTBbits.RB1	//Senal A(DATO) del Encoder
#define ENCBpin	PORTBbits.RB2	//Senal B(CLK) de Enconder
#define KEYUP	1	
#define KEYDOWN 2
#define LCD_PORT PORTD			//Puerto de datos para LCD
#define LCD_TRIS TRISD			//Registro de configuracion LCD
#define LCD_RS	PORTDbits.RD4	//Pin de control RS del LCD
#define LCD_EN	PORTDbits.RD5	//Pin de control EN del LCD
#define LCD_BACKLIGHT PORTDbits.RD6 //Control de luz de fondo LCD
#include "lcd.h"
#include "peripheral.h"
void func1();
void func2();
void func3();
void salir();
//Estructura del Menu
typedef const struct 
{
    const char *text;   //Menu Title
    char mpoints;       //Options Title numbers
    char btUP, btDOWN, btENTER;
    void (*fp) (void);
} menustruc;
char menusel = 0;
const char menu_000[] = "  MENU A   "; 
const char menu_001[] = "  MENU B   ";
const char menu_002[] = "  SALIR    ";
const char menu_010[] = "  Opcion 1 "; //MENU A
const char menu_011[] = "  Opcion 2 ";
const char menu_012[] = "  Opcion 3 ";
const char menu_013[] = "  Atras    ";
const char menu_020[] = "  Opcion 4 "; //MENU B
const char menu_021[] = "  Opcion 5 "; 
const char menu_022[] = "  Opcion 6 ";
const char menu_023[] = "  Opcion 7 ";
const char menu_024[] = "  Atras    ";
menustruc menu[] = 
{
    {menu_000, 3, 0, 1, 3, 0},  //0 MENU A
	{menu_001, 3, 0, 2, 7, 0},  //1 MENU B
	{menu_002, 3, 1, 2, 2, 0},  //2 Salir
	{menu_010, 4, 3, 4, 4, func1}, //3 
    {menu_011, 4, 3, 5, 5, func2}, //4 
    {menu_012, 4, 4, 6, 6, func3}, //5
    {menu_013, 4, 5, 6, 0, 0},	 //6 
	{menu_020, 5, 7, 8, 8, func1}, //7 
	{menu_021, 5, 7, 9, 9, func1}, //8 
	{menu_022, 5, 8, 10, 10, func1}, //9
	{menu_023, 5, 9, 11, 11, func1}, //10
	{menu_024, 5, 10, 11, 1, 0},	//11
};
//VAR GLOBAL
enum led {LEDOFF, LEDON} led1st; //Enumaracion de estados para el LED
enum but {RELEASE, PRESSED} but1st; //Enumeracion de estados para el BUT
enum men {NORMAL, AJUSTE, RETARDO} men1st; //Enumeracion de estados para el MENU
volatile char keysense = 0;	//Captura el valor del encoder
char but1OK = false;	//Captura el pulso de RB0
char lcdtext[20], hora = 12, min = 0, sec = 0; //Variables para control de hora
unsigned int seccnt = 0; //Contador de segundos
void TaskLED1();
void TaskBUT1(void);
void TaskMENU1();
void ShowMenu();
void interrupt isr()
{
	if(INTCONbits.RBIF && INTCONbits.RBIE)
	{
		if(ENCApin) //if DAT = 1
		{
			if(ENCBpin) keysense = KEYUP;
			else keysense = KEYDOWN;
		}
		else //if(DAT = 0) Discard
		{
			if(ENCBpin) keysense = KEYDOWN;
			else keysense = KEYUP;
		}
		INTCONbits.RBIE = 0;
		INTCONbits.RBIF = 0;
	}
}
void main()
{
	OSCSetup();	//Ajuste del Oscilador
	ANSEL = 0;	//Deshabilita pines analogicos AN0-AN7
	ANSELH = 0;	//Deshabilita pines analogicos AN8-AN13
	TRISEbits.TRISE2 = 0; //Configura como salida pin LED1
	LED1pin = 0;			//Apaga el LED1
	TRISB = 0xFF;	//Configura como entrada los pines del PORTB
	EnablePU();	//Activa las resistencias pull-up del PORTB
	TRISD = 0;	//Configura todo el PORTC como salida
	PORTD = 0;	//Coloca en nivel bajo todos los pines del PORTD
	led1st = LEDOFF;	//Estado iniical del LED1
	but1st = RELEASE;	//Estado inicial del pulsador BUT1
	men1st = NORMAL;	//Estado inicial del Menu LCD
	__delay_ms(1000);
	LCDSetup(LINES2);	//Inicia el modulo LCD a dos lineas
	LCDPuts(" L.C.D. PIC16F "); //Escribe el mensaje en la primera linea
	LCDGotoln(1);//Cambia el cursos a la segunda linea LCD
	IOCBbits.IOCB1 = 1;	//Habilita la interrupcion por cambio en el pin RB1(DATO)
	INTCONbits.RBIE = 1;//Habilita la interrupcion por cambio de estado PORTB
	INTCONbits.GIE = 1; //Habilita la interrupcion global
	while(true)
	{
		TaskLED1(); //Destello LED
		TaskBUT1();
		TaskMENU1();
		if(seccnt++ > 990)
		{
			seccnt  = 0;
			if(sec++> 59)
			{
				sec = 0;
				if(min ++> 59)
				{
					min = 0;
					if(hora++>23) hora = 0;
				}
			}
		}
		__delay_ms(1);
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
				if(cnt > 100)
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
				if(cnt > 100)
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
void TaskMENU1()
{
	static unsigned int cnt = 0;
	switch(men1st)
	{
		case NORMAL:
		{
			cnt ++;
			if(cnt > 499)
			{
				LCDGotoln(1);
				sprintf(lcdtext, "   [%02u:%02u:%02u]", hora, min, sec);
				LCDPuts(lcdtext);
				cnt  = 0;
			}
			if(but1OK == 1)
			{
				but1OK = 0;
				men1st = AJUSTE;
				LCDSet(CLEAR);
				ShowMenu();
				cnt = 0;
			}
		} break;
		case AJUSTE:
		{
			if(keysense || but1OK)
			{
				if(keysense == KEYUP) menusel = menu[menusel].btUP;
                if(keysense == KEYDOWN) menusel = menu[menusel].btDOWN;
                if(but1OK)
                {
                    if(menu[menusel].fp != 0)
						menu[menusel].fp(); //
					else
						menusel = menu[menusel].btENTER; //Hold in menu position
				}
				cnt = 0;
				men1st = RETARDO;
				ShowMenu();
			}
		} break;
		case RETARDO:
		{
			cnt ++;
			if(cnt > 100)
			{
				keysense = PORTB;
				INTCONbits.RBIF = 0;
				INTCONbits.RBIE = 1;
				keysense = 0;
				if(but1OK)
				{
					but1OK = 0;
					if(menusel == 2) //Opcion de salir
					{ 
						LCDSet(CLEAR);
						LCDPuts(" L.C.D. PIC16F ");
						LCDGotoln(1);
						men1st = NORMAL;
						cnt = 0;
						break;
					}
				}
				men1st = AJUSTE;
			}
		} break;
	}
}
void ShowMenu()
{
    char lcnt = 0, from = 0, till = 0, temp = 0;
    while(till <= menusel)
        till += menu[till].mpoints; //Max position for present option
    from = till - menu[menusel].mpoints; //Min for actual pos
    till --; //Till is value - 1
    temp = from;
    //Verifica si la seleccion esta mas alla de la segunda linea para desplazar el lcd
    if(menusel >= (from + 1) && (menusel <= (till-1))) //for lcd is 2 lines only
    {
        from = menusel - 1; //2lines = 1, 4lines = 2
        till = from + 1; //only two lines show last line
        for(from; from <= till; from ++)
        {
            LCDGotoln(lcnt);
            LCDPuts(menu[from].text);
            lcnt++;
        }
        LCDGotoln(1); //Second line
        LCDPutc(0x7E);
    }
    else
    {
        if(menusel < (from + 1))
        {
            till = from  + 1; //for 2Lines=1 4Lines=3
            for (from; from <= till; from ++)
            {
                LCDGotoln(lcnt);
                LCDPuts(menu[from].text);
                lcnt ++;
            }
            LCDGotoln(menusel - temp);
            LCDPutc(0x7E);
        }
        if(menusel == till)
        {
            from = till - 1; //for 2Lines=1 4Lines=3
            for (from; from <= till; from ++)
            {
                LCDGotoln(lcnt);
                LCDPuts(menu[from].text);
                lcnt ++;
            }
            LCDGotoln(1);
            LCDPutc(0x7E);
        }
    }
}
void func1()
{
	LCDSet(CLEAR);
	LCDPuts(" Funcion 1");
	__delay_ms(1000);
}
void func2()
{
	LCDSet(CLEAR);
	LCDPuts(" Funcion 2");
	__delay_ms(1000);
}
void func3()
{
	LCDSet(CLEAR);
	LCDPuts(" Funcion 3");
	__delay_ms(1000);
}