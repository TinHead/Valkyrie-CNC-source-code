#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <math.h>
#include "usiTwiSlave.h"

// Pin assignments:

#define LED PD6
#define EN1 PB0
#define EN2 PB1


// A1, A2 is coil a, B1,B2 coil b corresponding to pins 1,2,3,4 on the motor connector on the board
#define	A1  PB2
#define A2  PD5
#define B1  PB3
#define B2  PB4

// easyer to remember which is what
#define A1PWM OCR0A
#define A2PWM OCR0B
#define B1PWM OCR1A
#define B2PWM OCR1B

// shorcuts 
#define output_low(port,pin) port &= ~(1<<pin)
#define output_high(port,pin) port |= (1<<pin)
#define set_input(portdir,pin) portdir &= ~(1<<pin)
#define set_output(portdir,pin) portdir |= (1<<pin)

int step_number = 0;

// defines the number of steps for the motor
// since we are using halfstepping you have here the number of real steps * 2 
const int number_of_steps = 96;

void init_pwm() {
//8 bit timers
//timer 0 - phase corect pwm mode 
// prescaler 1:1
	// Clear TCCR0A and B
	TCCR0A = 0;
	TCCR0B = 0;
	OCR0A = 0;
	OCR0B = 0;
	TCCR0A = (1<<WGM00)|(1<<COM0A1)|(1<<COM0B1);
	TCCR0B = (1<<CS00);

//timer 1 - 8 bit phase corect pwm mode 
//prescaler 1:1
	OCR1A = 0;
	OCR1B = 0;
	TCCR1A = 0;
	TCCR1B = 0;
	TCCR1A = (1<<WGM10)|(1<<COM1A1)|(1<<COM1B1);
	TCCR1B = (1<<CS10); //prescaler set to 8 frequency => 3kHz

}

void step_micro(uint8_t next_step) {
// This implements the fulltorque half stepping mode
//takes next_step in the half step sequence
	uint8_t quadrant = next_step % 8;
	switch (quadrant){
		case 0: //0110
			A1PWM=0;
			A2PWM=255;
			B1PWM=102;
			B2PWM=0;
			output_low (PORTB, A1);
		  	output_high (PORTD, A2);
			output_high (PORTB, B1);
		  	output_low (PORTB, B2);
		break;
		case 1: //0110
			A1PWM=0;
			A2PWM=102;
			B1PWM=255;
			B2PWM=0;
			output_low (PORTB, A1);
		  	output_high (PORTD, A2);
			output_high (PORTB, B1);
		  	output_low (PORTB, B2);
 		break;
		case 2: //0101
			A1PWM=0;
			A2PWM=255;
			B1PWM=0;
			B2PWM=102;
			output_low (PORTB, A1);
 		  	output_high (PORTD, A2);
 		  	output_low (PORTB, B1);
		  	output_high (PORTB, B2);
 		break;
		case 3: //0101
			A1PWM=0;
			A2PWM=102;
			B1PWM=0;
			B2PWM=255;
			output_low (PORTB, A1);
 		  	output_high (PORTD, A2);
 		  	output_low (PORTB, B1);
		  	output_high (PORTB, B2);
 		break;
		case 4: //1001
			A1PWM=255;
			A2PWM=0;
			B1PWM=0;
			B2PWM=102;
 		  	output_high (PORTB, A1);
  		  	output_low (PORTD, A2);
  		  	output_low (PORTB, B1);
		  	output_high (PORTB, B2);

 		break;
		case 5: //1001
			A1PWM=102;
			A2PWM=0;
			B1PWM=0;
			B2PWM=255;
 		  	output_high (PORTB, A1);
  		  	output_low (PORTD, A2);
  		  	output_low (PORTB, B1);
		  	output_high (PORTB, B2);
 		break;
		case 6: //1010
			A1PWM=255;
			A2PWM=0;
			B1PWM=102;
			B2PWM=0;
	 		output_high (PORTB, A1);
  			output_low (PORTD, A2);
  			output_high (PORTB, B1);
			output_low (PORTB, B2);		
 		break;
		case 7: //1010
			A1PWM=102;
			A2PWM=0;
			B1PWM=255;
			B2PWM=0;
	 		output_high (PORTB, A1);
  			output_low (PORTD, A2);
  			output_high (PORTB, B1);
			output_low (PORTB, B2);		
 		break;
	}
}


void step(uint8_t direction)
{  
  // decrement the number of steps, moving one step each time:
	  // move only if the appropriate delay has passed:
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
	step_micro (step_number); 
}

void motor_test (uint8_t type){
	output_high (PORTB, EN1);
	output_high (PORTB, EN2);
	int i=0;
	if (type==1){
	//halstep
		for (i=0; i<number_of_steps; i++){
			step(1);
			_delay_ms(500);
		}
		_delay_ms(100);
		for (i=0; i<number_of_steps; i++){
			step(0);
			_delay_ms(500);
		}
		_delay_ms(100);
	}
	else{
	//halfstep
		for (i=0; i<number_of_steps; i++){
			step(1);
			_delay_ms(100);
		}
		_delay_ms(1000);
		//step 48 full steps forward to test
		for (i=0; i<number_of_steps; i++){
			step(0);
			_delay_ms(100);
		}
		_delay_ms(1000);
	}
}



int main(void) {

// initialize outputs
set_output(DDRD, LED);
set_output(DDRB, EN1);  
set_output(DDRB, EN2);  
set_output(DDRB, A1);  
set_output(DDRD, A2);  
set_output(DDRB, B1);
set_output(DDRB, B2);

//blink once to see init
output_high(PORTD, LED);
_delay_ms(250);
output_low(PORTD, LED);

//initialize twi
// BIG FAT NOTE: for each driver you need to set the ID below as follows:
// X axis = 2
// Y axis = 3
// Z axis = 4

usiTwiSlaveInit(2);

// temporary receive variable
unsigned char temp = 0;

//init interrupts
sei();

//init pwm on input pins
init_pwm();

while (1) {
	if(usiTwiDataInReceiveBuffer()) {
		 temp = usiTwiReceiveByte();
	}
	switch (temp){
		case 1:
			//forwards
			output_high(PORTD, LED);
			step(1);
			output_low(PORTD, LED);
			temp = 0;	
		break;
		case 2:
			//backwards
			output_high(PORTD, LED);
			step(0);
			output_low(PORTD, LED);
			temp = 0;	
		break;
		case 3:
			//enable driver
			output_high(PORTD, LED);
			output_high (PORTB, EN1);
			output_high (PORTB, EN2);
			output_low(PORTD, LED);
			temp = 0;	
		break;
		case 4:
			//disable driver
			output_high(PORTD, LED);
			output_low (PORTB, EN1);
			output_low (PORTB, EN2);
			output_low (PORTD, LED);
			temp = 0;	
		break;
		case 5:
			motor_test(0);
		break;
		case 6:
			motor_test(1);
	}
}
}

