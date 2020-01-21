/*
 * MyFirstProject.c
 *
 * Created: 2019-11-30 14:51:10
 * Author : t530
 */ 

#ifndef F_CPU					// if F_CPU was not defined in Project -> Properties
#define F_CPU 1000000UL			// define it now as 1 MHz unsigned long
#endif

#include <avr/io.h>				// this is always included in AVR programs
#include <util/delay.h>			// add this to use the delay function

///////////////////////////////////////////////////////////////////////////////////////////////////
int main(void) {
	
	DDRC |= (1 << PC0);			// set Port C pin PC5 for output
	DDRC |= (1 << PC1);			// set Port C pin PC5 for output
	while (1) {					// begin infinite loop
		//PORTC ^= (1 << PC5);	// flip state of LED on PC5
		//PORTC = 1;
		PORTC |= (1 << PC0);
		_delay_ms(2000);
		PORTC &= ~(1 << PC0);
		int i = 10;
		while(i--)
		{
			_delay_ms(2000);
			PORTC ^= (1 << PC1);
		}
		//_delay_ms(10000);
		PORTC |= (1 << PC0);
		while(1);
	}
	return(0);					// should never get here, this is to prevent a compiler warning
}