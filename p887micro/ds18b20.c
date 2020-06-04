/****************************************************************
 *	Libreria de rutinas basicas para los Recursos/Perifericos	*
 * 	Lista de MCU: PIC16F882/F883/F884/F886/F887					*
 *	Company: Universidad Evangelica Boliviana 					*
 *	Author: Pablo Zarate A. pablinza@me.com						*
 *	Version: Jan 2020 V20.01									*
 *	Summary: Es una libreria de funciones y procedimientos para	*
 *	hacer uso del sensor DS18B mediante protocolo 1-wire		*
 *	Requiere definir DS18pin y DS18Tris con pullup 4.7k			*
 ****************************************************************/
#include <stdint.h>
#include <stdlib.h>
#include <xc.h>
#include "ds18b20.h"
uint8_t DS18Reset()
{
	uint8_t res;
	DS18pin = 0;	//Set pin in low level
	DS18tris = 0;	//Master Put bus in low level (output)
	wait240US;
	wait240US;	//1-wire require time delay
	DS18tris = 1;	//Master release bus (input tri-state)
	wait60US;// 1-wire wait time before sample
	if(DS18pin == 0) res = 1;	//Check slave presence
	else res = 0;	//No salve detect
	wait240US;//wait time
	return res;
}

void DS18Write(uint8_t data)
{
	uint8_t cnt = 8;
	DS18tris = 1; //Master release bus (input tri-state)
	while(cnt --)
	{
		if(data & 0x01) 
		{  // write 1 on bus
			DS18pin = 0;	//Set pin in low level
			DS18tris = 0;	//Master Put bus in low level (output)
			wait5US;	//wait time max 15us
			DS18tris = 1;	//Master release bus (input tri-state)
			wait60US;//1-wire require time delay
		} 
		else 
		{  //write 0 on bus
			DS18pin = 0;	//Set pin in low level
			DS18tris = 0;	//Master Put bus in low level (output)
			wait60US;//1-wire require time delay
			DS18tris = 1;	//Master release bus (input tri-state)
			wait5US;
		}
		data = data >> 1;
	}
}

uint8_t DS18Read()
{
	uint8_t cnt = 8, data = 0;
	DS18tris = 1; //Master release bus (input tri-state)
	while(cnt --)
	{
		DS18pin = 0;	//Set pin in low level
		DS18tris = 0;	//Master Put bus in low level (output)
		wait5US; //wait between 5-15uS
		DS18tris = 1;	//Master release bus (input tri-state)
		wait5US; //wait time before sample level
		wait5US;
		if(DS18pin)		//sample level
			data |= 0x80; //if level is high(1)
		if(cnt) data >>= 1;
		wait60US; // wait for recovery time
	}
	return data;
}
