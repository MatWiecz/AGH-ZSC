/*
 * MyFirstProject.c
 *
 * Created: 2019-11-30 14:51:10
 * Author : t530
 * 

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
}*/


/*
 * RasterLaserProjector.c
 *
 * Created: 2019-12-12 09:51:29
 * Author : t530
 */ 

#ifndef F_CPU					// if F_CPU was not defined in Project -> Properties
#define F_CPU 8000000UL			// define it now as 8 MHz unsigned long
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void _NOP(){
	__asm__ __volatile__ ("nop");
}

void setYellowLEDState(char newState){
	if(newState==0)
		PORTB &= ~(1<<PB7);
	else
		PORTB |= (1<<PB7);
}

void setGreenLEDState(char newState){
	if(newState==0)
		PORTB &= ~(1<<PB0);
	else
		PORTB |= (1<<PB0);
}

void setBlueLEDState(char newState){
	if(newState==0)
		PORTD &= ~(1<<PD5);
	else
		PORTD |= (1<<PD5);
}

void setRedLEDState(char newState){
	if(newState==0)
		PORTD &= ~(1<<PD4);
	else
		PORTD |= (1<<PD4);
}

char getPowerSupplyStatus(){
	char cData = PINC;
	if(cData & (1<<PC2))
		return 1;
	return 0;
}

char buttonReady;
char get1ButtonState(char buttonId){
	char bData = PINB;
	char dData = PIND;
	switch(buttonId){
		case 1:
			if(dData & (1<<PD1)){
				if(buttonReady & (1<<buttonId)){
					buttonReady &= ~(1<<buttonId);
					return 1;
				}
			}
			else 
				buttonReady |= 1<<buttonId;
			break;
		case 2:
			if(bData & (1<<PB6)){
				if(buttonReady & (1<<buttonId)){
					buttonReady &= ~(1<<buttonId);
					return 1;
				}
			}
			else 
				buttonReady |= 1<<buttonId;
			break;
		case 3:
			if(dData & (1<<PD7)){
				if(buttonReady & (1<<buttonId)){
					buttonReady &= ~(1<<buttonId);
					return 1;
				}
			}
			else 
				buttonReady |= 1<<buttonId;
			break;
		case 4:
			if(dData & (1<<PD6)){
				if(buttonReady & (1<<buttonId)){
					buttonReady &= ~(1<<buttonId);
					return 1;
				}
			}
			else 
				buttonReady |= 1<<buttonId;
			break;
		case 5:
			if(dData & (1<<PD0)){
				if(buttonReady & (1<<buttonId)){
					buttonReady &= ~(1<<buttonId);
					return 1;
				}
			}
			else
				buttonReady |= 1<<buttonId;
			break;
	}
	
	return 0;
}

char systemFlags;
#define SYSTEM_READY 0x01
#define SYSTEM_ON 0x02
#define SYSTEM_INITIATED 0x04
#define SYSTEM_SHUTDOWN_ON 0x40
#define SYSTEM_TO_RESET 0x80

char systemErrorFlags;
#define POWER_SUPPLY_ERROR 0x01

long timeToReset;

void setSystemFlags(char flagsMask){
	systemFlags |= flagsMask;
}

void unsetSystemFlags(char flagsMask){
	systemFlags &= ~flagsMask;
}

char getSystemFlag(char flag){
	if(systemFlags & flag)
		return 1;
	return 0;
}

void setSystemErrorFlags(char flagsMask){
	systemErrorFlags |= flagsMask;
}

void unsetSystemErrorFlags(char flagsMask){
	systemErrorFlags &= ~flagsMask;
}

char getSystemErrorFlag(char flag){
	if(systemErrorFlags & flag)
		return 1;
	return 0;
}

char isSystemWorking(){
	return (char)(getSystemFlag(SYSTEM_INITIATED) & !getSystemFlag(SYSTEM_SHUTDOWN_ON));
}

void setPowerOnState(char on){
	if(on == 0)
		PORTC &= ~(1<<PC5);
	else
		PORTC |= (1<<PC5);
}

void setLaserState(char on){
	if(on == 0)
		PORTC &= ~(1<<PC4);
	else
		PORTC |= (1<<PC4);
}

void processShutDown(){
	if(getSystemFlag(SYSTEM_SHUTDOWN_ON))
		if(timeToReset > 0)
			timeToReset--;
	if(timeToReset == 0)
		setSystemFlags(SYSTEM_TO_RESET);
}

char drawNow = 0;
long int0Count;
ISR(INT0_vect)
{
	setBlueLEDState(1);
	drawNow = (drawNow + 1) % 6;
	{
		int i = 10;
		while(i--){
			if(i - 1 == drawNow)
				setLaserState(1);
			_delay_us(50);
			setLaserState(0);
			_delay_us(50);
		}
	}
	int0Count++;
	setBlueLEDState(0);
}

ISR(INT1_vect)
{
	setGreenLEDState(1);
	_delay_ms(1);
	setGreenLEDState(0);
	_delay_ms(1);
}

void controlPower(){
	setPowerOnState(getSystemFlag(SYSTEM_ON));
}

char laserOn;
char laserUpState;
void controlLaser(){
	if(isSystemWorking()){
		int i = 1000;
		while(i--){
			if(laserOn)
			laserUpState = 1 - laserUpState;
			else
			laserUpState = 1;
			setLaserState(laserUpState);
			_delay_us(10);
		}
	}
	else
		setLaserState(0);
}

void setXAxisMotorSpeed(int speed){
	if(speed >= 0 && speed < 1024)
		OCR1A = speed;
}

int timeToNextAdvancedPowerSupplyCheck;
void checkPowerSupplyStatus(){
	if(getSystemFlag(SYSTEM_ON) && timeToNextAdvancedPowerSupplyCheck == 0){
		setPowerOnState(0);
		_delay_us(10);
	}
	if(!getSystemFlag(SYSTEM_ON) || timeToNextAdvancedPowerSupplyCheck == 0){
		if(getPowerSupplyStatus()){
			setSystemFlags(SYSTEM_READY);
			unsetSystemErrorFlags(POWER_SUPPLY_ERROR);
		}
		else{
			unsetSystemFlags(0xFF);
			setSystemErrorFlags(POWER_SUPPLY_ERROR);
		}
	}
	if(getSystemFlag(SYSTEM_ON)){
		if(timeToNextAdvancedPowerSupplyCheck == 0){
			setPowerOnState(1);
			_delay_us(10);
			timeToNextAdvancedPowerSupplyCheck = 8000;
		}
		timeToNextAdvancedPowerSupplyCheck--;
	}
}

void controlXAxisMotor(){
	if(isSystemWorking()){
		//setXAxisMotorSpeed(512);
	}
	else
		setXAxisMotorSpeed(0);
}

void adjustXAxisMotor(long targetRPS){
	if(targetRPS < 10)
		return;
	long nextSpeedDelta = 320;
	long curSpeed = nextSpeedDelta;
	setXAxisMotorSpeed(curSpeed + 383);
	_delay_ms(2000);
	long curRPS = 0;
	char measureFailure = 0;
	nextSpeedDelta /= 2;
	while(nextSpeedDelta){
		long startInt0Count = int0Count;
		_delay_ms(1000);
		long endInt0Count = int0Count;
		long delta = endInt0Count - startInt0Count;
		if(delta < 10){
			measureFailure++;
			if(measureFailure == 5)
				break;
			continue;
		}
		else{
			measureFailure = 0;
			curRPS = delta;
			if(curRPS > targetRPS){
				curSpeed -= nextSpeedDelta;
			}
			else{
				curSpeed += nextSpeedDelta;
			}
			setXAxisMotorSpeed(curSpeed + 383);
			_delay_ms(2000);
		}
		nextSpeedDelta /= 2;
	}
	setBlueLEDState(1);
	_delay_ms(2000);
	setBlueLEDState(0);
	int0Count = curRPS;
}

long powerSwitchButtonSecondPress;
void handlePowerSwitchButton(){
	if(getSystemFlag(SYSTEM_READY) && get1ButtonState(1))
	{
		if(!getSystemFlag(SYSTEM_ON)){
			setSystemFlags(SYSTEM_ON|SYSTEM_INITIATED);
			return;
		}
		if(getSystemFlag(SYSTEM_INITIATED)){
			if(!powerSwitchButtonSecondPress)
				powerSwitchButtonSecondPress = 5000;
			else
				setSystemFlags(SYSTEM_SHUTDOWN_ON);
		}
	}
	if(powerSwitchButtonSecondPress > 0)
		powerSwitchButtonSecondPress--;
}

void handleLaserOnButton(){
	if(getSystemFlag(SYSTEM_ON) && get1ButtonState(2))
	{
		laserOn = 1 - laserOn;
		setBlueLEDState(laserOn);
	}
}

char int0CountNextBit = 0;
void handleGetInt0CountButton1(){
	if(get1ButtonState(3)){
		int copy = int0Count;
		if(int0CountNextBit == 16)
			int0CountNextBit = 0;
		char temp = int0CountNextBit + 1;
		char val = 0;
		while(temp--){
			val = copy % 2;
			copy = copy / 2;
		}
		setBlueLEDState(1);
		_delay_ms(10);
		setBlueLEDState(0);
		_delay_ms(100);
		setBlueLEDState(val);
		_delay_ms(10);
		setBlueLEDState(0);
		int0CountNextBit++;
	}
	
}

void handleGetInt0CountButton2(){
	if(get1ButtonState(4)){
		int0CountNextBit = 0;
	}
}

void updateErrorLEDState(){
	setRedLEDState(systemErrorFlags);
}

void updatePowerSupplyLEDState(){
	setYellowLEDState(getSystemFlag(SYSTEM_READY));
}

unsigned int systemOnLEDBlinkPoints;
char systemOnLEDstate;

void updateSystemOnLEDState(){
	if(getSystemFlag(SYSTEM_ON)){
		if(getSystemFlag(SYSTEM_INITIATED) & !getSystemFlag(SYSTEM_SHUTDOWN_ON)){
			setGreenLEDState(1);
		}
		else{
			if(systemOnLEDBlinkPoints == 0){
				systemOnLEDBlinkPoints = 8000;
				systemOnLEDstate = 1 - systemOnLEDstate;
				setGreenLEDState(systemOnLEDstate);
			}
			systemOnLEDBlinkPoints--;
		}
	}
	else{
		setGreenLEDState(0);
		systemOnLEDBlinkPoints = 0;
		systemOnLEDstate = 0;
	}
}

void updateLEDsState(){
	updatePowerSupplyLEDState();
	updateSystemOnLEDState();
	updateErrorLEDState();
}

void initSystem(){
	setYellowLEDState(1);
	setGreenLEDState(1);
	setBlueLEDState(1);
	setRedLEDState(1);
	unsetSystemErrorFlags(0xFF);
	unsetSystemFlags(0xFF);
	timeToReset = 65000;
	laserOn = 0;
	laserUpState = 0;
	controlLaser();
	controlPower();
	buttonReady = 0;
	timeToNextAdvancedPowerSupplyCheck = 0;
	systemOnLEDBlinkPoints = 0;
	systemOnLEDstate = 0;
	powerSwitchButtonSecondPress = 0;
	int0Count = 0;
	_delay_ms(2000);
	setYellowLEDState(0);
	setGreenLEDState(0);
	setBlueLEDState(0);
	setRedLEDState(0);
}

int main(void)
{
	// Set up I/O Pins
	DDRB = (1<<PB7)|(1<<PB0)|(1<<PB1)|(1<<PB2);
	DDRC = (1<<PC4)|(1<<PC5);
	DDRD = (1<<PD5)|(1<<PD4);
	
	cli();
	MCUCR = (MCUCR & 0b11110000) | 0b1010;
	GICR = (GICR & 0b00111111) | (1<<INT0)|(1<<INT1);
	//PORTD = (1<<PD2)|(1<<PD3);
	sei();
	
	OCR1A = 0;
	OCR1B = 0;
	
	TCCR1A |= (1 << COM1A1) | (1 << COM1B1);
	// set non-inverting mode
	TCCR1A |= (1 << WGM11) | (1 << WGM10);
	// set 10bit phase corrected PWM Mode
	TCCR1B |= (1 << CS20)|(1 << CS21);
	// set prescaler to 8 and starts PWM
	
	//PORTB = (1<<PB6);
	//PORTC = (1<<PC2);
	//PORTD = (1<<PD1)|(1<<PD7)|(1<<PD6)|(1<<PD0);
	_NOP();
	
    /* Replace with your application code */
    while (1) 
    {
		initSystem();
		while(!getSystemFlag(SYSTEM_TO_RESET)){
			checkPowerSupplyStatus();
			controlPower();
			controlLaser();
			controlXAxisMotor();
			handlePowerSwitchButton();
			//handleLaserOnButton();
			if(get1ButtonState(2)){
				int0Count = 0;
			}
			handleGetInt0CountButton1();
			handleGetInt0CountButton2();
			if(get1ButtonState(5)){
				/*setXAxisMotorSpeed(1023);
				_delay_ms(100);
				for(int i = 400; i < 1023; i++){
					setXAxisMotorSpeed(i);
					_delay_ms(100);
					if(get1ButtonState(5)){
						int0Count = i;
						break;
					}
				}*/
				adjustXAxisMotor(50);
			}
			processShutDown();
			updateLEDsState();
			_delay_us(1);
		}
    }
}

