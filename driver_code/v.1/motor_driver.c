/*
X axis controller id is 2
Y axis controller id is 3
Z axis controller id is 4
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>
#include "usiTwiSlave.h"

//Attiny speed 16MHZ
#define F_CPU 16000000UL

// Defines for the led and the L298 pins
#define LED PD5
//enable pins
#define EN1 PB3
#define EN2 PB4
//input pins

// by mistake I wired the X axis controller differently than Y and Z ... was too lazy to fix it
///FOR ID 2 the PINOUT IS DIFFERENT ....BOO
#define	A1  PB2
#define B1  PB1
#define A2  PB0
#define B2  PD6

/* 
// For id 3 & 4 the pinout like this ... boo again
#define	A1  PB0
#define B1  PB1
#define A2  PB2
#define B2  PD6
*/

// helper macros 
#define output_low(port,pin) port &= ~(1<<pin)
#define output_high(port,pin) port |= (1<<pin)
#define set_input(portdir,pin) portdir &= ~(1<<pin)
#define set_output(portdir,pin) portdir |= (1<<pin)

// step number is used to hold the current step
int step_number = 0;
// here we define the number of steps per revolution
const int number_of_steps = 96;



// function definitions here

void motor_test (uint8_t type){
// handy to test the motors
	output_high (PORTB, EN1);
	output_high (PORTB, EN2);
	int i=0;
	if (type==1){
	//halstep
		for (i=0; i<number_of_steps; i++){
			step(1,1);
			_delay_ms(100);
		}
		_delay_ms(1000);
		for (i=0; i<number_of_steps; i++){
			step(0,1);
			_delay_ms(100);
		}
		_delay_ms(1000);
	}
	else{
	//fullstepping
		for (i=0; i<number_of_steps; i++){
			step(1,0);
			_delay_ms(100);
		}
		_delay_ms(1000);
		//step 48 full steps forward to test
		for (i=0; i<number_of_steps; i++){
			step(0,0);
			_delay_ms(100);
		}
		_delay_ms(1000);
	}
}

void step_full(uint8_t next_step) {
//takes next_step in the full step sequence
 	switch (next_step) {
		case 0: // 1010
  		  output_high (PORTB, A1);
 		  output_low (PORTB, B1);
		  output_high (PORTB, A2);
		  output_low (PORTD, B2);
		break;
		case 1: // 0110
 		  output_low (PORTB, A1);
  		  output_high (PORTB, B1);
		  output_high (PORTB, A2);
		  output_low (PORTD, B2);
		break;
		case 2: // 0101
 		  output_low (PORTB, A1);
  		  output_high (PORTB, B1);
		  output_low (PORTB, A2);
		  output_high (PORTD, B2);
		break;
		case 3: // 1001
 		  output_high (PORTB, A1);
  		  output_low (PORTB, B1);
		  output_low (PORTB, A2);
		  output_high (PORTD, B2);
		break;
	} 
}


void step_half(uint8_t next_step) {
//takes next_step in the half step sequence
	switch (next_step){
		case 0: //1010
  		  output_high (PORTB, A1);
 		  output_low (PORTB, B1);
		  output_high (PORTB, A2);
		  output_low (PORTD, B2);
		break;
		case 1: //1000
  		  output_high (PORTB, A1);
 		  output_low (PORTB, B1);
		  output_low (PORTB, A2);
		  output_low (PORTD, B2);
		break;
		case 2: //1001
  		  output_high (PORTB, A1);
 		  output_low (PORTB, B1);
		  output_low (PORTB, A2);
		  output_high (PORTD, B2);
		break;
		case 3: //0001
  		  output_low (PORTB, A1);
 		  output_low (PORTB, B1);
		  output_low (PORTB, A2);
		  output_high (PORTD, B2);
		break;
		case 4: //0101
  		  output_low (PORTB, A1);
 		  output_high (PORTB, B1);
		  output_low (PORTB, A2);
		  output_high (PORTD, B2);
		break;
		case 5: //0100
  		  output_low (PORTB, A1);
 		  output_high (PORTB, B1);
		  output_low (PORTB, A2);
		  output_low (PORTD, B2);
		break;
		case 6: //0110
  		  output_low (PORTB, A1);
 		  output_high (PORTB, B1);
		  output_high (PORTB, A2);
		  output_low (PORTD, B2);
		break;
		case 7: //0010
  		  output_low (PORTB, A1);
 		  output_low (PORTB, B1);
		  output_high (PORTB, A2);
		  output_low (PORTD, B2);
		break;
	}
}

void step(uint8_t direction, uint8_t type)
{  
  // decrement the number of steps, moving one step each time:
	   if (direction == 1)
		{
        	step_number++;
	        if (step_number == number_of_steps) 
			{
        		  step_number = 0;
	        	}	
      		} 
      	   else { 
        	if (step_number == 0) {
          		step_number = number_of_steps;
        	}
        	step_number--;
      		}
      // step the motor to step number 0, 1, 2, or 3:
      // Note if half stepping is used then the step number has to be 0 to 7
	if ( type==0 ) {
      		step_full(step_number % 4);}
	else {
		step_half (step_number % 8); 
		}
	
}

// main loop

int main(void) {
	// initialize outputs
	set_output(DDRD, LED);
	set_output(DDRB, EN1);  
	set_output(DDRB, EN2);  
	set_output(DDRB, A1);  
	set_output(DDRB, B1);  
	set_output(DDRB, A2);
	set_output(DDRD, B2);

	//blink once to see init
	output_high(PORTD, LED);
	_delay_ms(250);
	output_low(PORTD, LED);
	
	//initialize twi
	usiTwiSlaveInit(2);

	//init interrupts
	sei();
	
	// temporary storage for i2c received data	
	unsigned char temp = 0;

while (1) {
	if(usiTwiDataInReceiveBuffer()) {
		 temp = usiTwiReceiveByte();
		 output_high(PORTD, LED);
	}
	switch (temp){
		case 1:
			//forwards half stepping
			step(1,1);
			temp = 0;	
		break;
		case 2:
			//backwards half stepping
			step(0,1);
			temp = 0;	
		break;
		case 3:
			//enable driver
			output_high (PORTB, EN1);
			output_high (PORTB, EN2);
			temp = 0;	
		break;
		case 4:
			//disable driver
			output_low (PORTB, EN1);
			output_low (PORTB, EN2);
			temp = 0;	
		break;
		case 5:
			motor_test(0);
		break;
		case 6:
			motor_test(1);
	}
	output_low(PORTD, LED);
}
}

