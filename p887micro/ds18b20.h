/****************************************************************
 *	Libreria de rutinas basicas para los Recursos/Perifericos	*
 *	Lista de MCU: PIC16F882/F883/F884/F886/F887				*
 *	Company: Universidad Evangelica Boliviana				*
 *	Author: Pablo Zarate A. pablinza@me.com					*
 *	Version: Jan 2020 V20.01							*
 *	Summary: Es una libreria de funciones y procedimientos para *
 *	hacer uso del sensor DS18B mediante	protocolo 1-wire		*
 *	Requiere definir DS18pin y DS18Tris con pullup 4.7k			*
 *******************************************************************/	
#ifndef _DS18B20_LIB
#define _DS18B20_LIB
#define wait240US __delay_us(240)
#define wait60US __delay_us(60)
#define wait5US __delay_us(5)
//#define DS18pin	PORTBbits.RB2
//#define DS18tris TRISBbits.TRISB2
/* ROM COMMANDS*/
#define SCHROM		0xF0	//Master read the ROM codes from multiple devices
#define READROM		0x33	//Master read the ROM code from one device only
#define MATCHROM	0x55	//Allows Master to address a specific device
#define SKIPROM		0xCC	//Address all devices on the bus simultaneously without sending out any ROM code 
#define ALARMSCH	0xEC	//Alarm Search
/* FUNCTION COMMANDS*/
#define CONVERT		0x44	//Initiates a temperature conversion. result is stored in temperature register 
#define WRITESC		0x4E	//Master to write 3 bytes (TH, TL, CONFIG) of data to the DS18B20’s scratchpad. 
#define READSC		0xBE	//Reads the entire scratchpad including the CRC byte.
char DS18Reset();
void DS18Write(char data);
char DS18Read();

#include "ds18b20.c"
#endif
