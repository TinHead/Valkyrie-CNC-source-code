/*
Step/Dir/Enable driver code for V4

V0.1 - halfstepping ignored for now
V0.2 - first try at microstepping 1/4 - working!!
V0.3 - 1/16 - working --- now need to optimize
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <math.h>

// Control INPUTS 

#define EN_PIN PD0
#define DIR_PIN PD1
#define STEP_PIN PD2

#define DBG_LED PD6

/* OUTPUTS for TEA3718*/

// reference voltage pins */
#define	REF1  PB4
#define	REF2  PB3

// phase pins 
#define	PHASE1  PB5
#define PHASE2  PB2

//inhibit pins

#define	IN0_1  PB7
#define	IN1_1  PB6
#define	IN0_2  PB0
#define	IN1_2  PB1

/* Helper macros */

#define read_pin(port, pin) (port & (1<<pin))
#define output_low(port,pin) port &= ~(1<<pin)
#define output_high(port,pin) port |= (1<<pin)
#define set_input(portdir,pin) portdir &= ~(1<<pin)
#define set_output(portdir,pin) portdir |= (1<<pin)

/* global var definitions */
struct step{
        uint8_t pwm1;
        uint8_t pwm2;
        };

struct step pwm_table[16]={
	{255,0},{254,25},{250,50},{244,74},{236,98},{225,120},{212,142},{197,162},{180,180},{162,197},{142,212},{120,225},{98,236},{74,244},{50,250},{25,254}
	};

int32_t step_number = 0;

uint8_t enabled = 0;


/* has to be volatile for interrupt use*/
volatile uint8_t step_cmd = 0;
volatile uint8_t edge = 0;

/* function defs */

/* init functions */

void init_pwm(void){
//timer 1 - 8 bit phase corect pwm mode 
//prescaler 1:8
	OCR1A = 0;
	OCR1B = 0;
	TCCR1A = 0;
	TCCR1B = 0;
	TCCR1A = (1<<WGM10)|(1<<COM1A1)|(1<<COM1B1);
	TCCR1B = (1<<CS11)|(1<<WGM12);
}

void init_ports(void){
// setup input/output ports
	// initialize inputs
	set_input(DDRD, EN_PIN);
	set_input(DDRD, DIR_PIN);
	set_input(DDRD, STEP_PIN);

	// initialize outputs
	set_output(DDRB, REF1);
	set_output(DDRB, REF2);  
	set_output(DDRB, PHASE1);  
	set_output(DDRB, PHASE2);  
	set_output(DDRB, IN0_1);  
	set_output(DDRB, IN0_2);
	set_output(DDRB, IN1_1);
	set_output(DDRB, IN1_2);

	//debug led 
	set_output(DDRD, DBG_LED);
}


void init_interrupts(void){
//setup interrupts for the step pin
	cli();
	MCUCR |= (1<<ISC01);
	GIMSK |= (1<<INT0);
	sei();
}

void init_drivers(void){
//setup initial driver state
	// setup inhibit pins disabled
	output_low(PORTB, IN0_1);  
	output_low(PORTB, IN1_1);
	output_low(PORTB, IN0_2);
	output_low(PORTB, IN1_2);

	// enable reference voltage pins 
	output_high(PORTB, REF1);
	output_high(PORTB, REF2);

	//enble pull-up on the step pin
	output_high(PORTD, EN_PIN);
	output_high(PORTD, DIR_PIN);
	output_high(PORTD, STEP_PIN);
	
	// initial state as step no 0
	OCR1B=pwm_table[0].pwm1;
	OCR1A=pwm_table[0].pwm2;
}


/* control functions*/

void enable(void){
// enable drivers - sets the inhibit pins to max power
	output_low(PORTB, IN0_1);
	output_low(PORTB, IN1_1);
	output_low(PORTB, IN0_2);	
	output_low(PORTB, IN1_2);
	output_high(PORTD, DBG_LED);
}

void disable(void){
// disable drivers - sets the inhibit pins to high = off
	output_high(PORTB, IN0_1);
	output_high(PORTB, IN1_1);
	output_high(PORTB, IN0_2);
	output_high(PORTB, IN1_2);
	output_low(PORTD, DBG_LED);
}

void step_micro(int32_t next_step) {
//step using the values in the array for the step

	uint8_t quad = next_step % 64;
	uint8_t step = quad % 16;
	
	if (quad <16){
			PORTB=0b00100100;
			OCR1B=pwm_table[step].pwm1;
			OCR1A=pwm_table[step].pwm2;
	}
	else if (quad >= 16 && quad <32){
			PORTB=0b00000100;
			OCR1B=pwm_table[step].pwm2;
			OCR1A=pwm_table[step].pwm1;
	}
	else if (quad >= 32 && quad <48){
			PORTB=0b00000000;
			OCR1B=pwm_table[step].pwm1;
			OCR1A=pwm_table[step].pwm2;
    	}
	else if (quad >= 48 && quad <64){
			PORTB=0b00100000;
			OCR1B=pwm_table[step].pwm2;
			OCR1A=pwm_table[step].pwm1;
	}
}

void step(uint8_t direction)
{  
  // decrement the number of steps, moving one step each time:
	   if (direction == 1)
		{
        	step_number++;
      		} 
      	   else { 
         step_number--;
      	}
	step_micro (step_number); 
}


/* Interrupt handlers*/

ISR(INT0_vect){
		if (step_cmd==0){
			step_cmd=1;
		}
}

int main(void) {
	// init stuff
	init_ports();
	init_drivers();
	init_interrupts();
	init_pwm();
	//blink led to see init
	output_high(PORTD, DBG_LED);
	_delay_ms(100);
	output_low(PORTD, DBG_LED);
	// main loop
	while (1) {
		if (!read_pin(PIND, EN_PIN)){
			if (enabled==0) {
				enable();
				enabled = 1;
			}
		}
		else
		{
			disable();
			enabled=0;
		}
		if (step_cmd){
			if ((read_pin(PIND, DIR_PIN))){
				step(1);
			}	
			else{
				step(0);
			}
			step_cmd=0;
		}
	}
}


