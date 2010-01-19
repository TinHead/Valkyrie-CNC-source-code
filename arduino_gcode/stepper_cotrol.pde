#include "parameters.h"
#include <Wire.h>

//init our variables
long max_delta;
long x_counter;
long y_counter;
long z_counter;
bool x_can_step;
bool y_can_step;
bool z_can_step;
int milli_delay;
int x_stepper = 2;
int y_stepper = 3;
int z_stepper = 4;

void init_steppers()
{
        //init twi
        Wire.begin();
	//init our points.
	current_units.x = 0.0;
	current_units.y = 0.0;
	current_units.z = 0.0;
	target_units.x = 0.0;
	target_units.y = 0.0;
	target_units.z = 0.0;
        //kill the drivers
	disable_stepper(x_stepper);
	disable_stepper(y_stepper);
	disable_stepper(z_stepper);
	calculate_deltas();
}

void send_twi_command(int id, int cmd){
    Wire.beginTransmission(id); // transmit to device with "id"
    Wire.send(cmd);        // sends one byte  
    Wire.endTransmission();    // stop transmitting
}

void dda_move(long micro_delay)
{
	//figure out our deltas
	max_delta = max(delta_steps.x, delta_steps.y);
	max_delta = max(delta_steps.z, max_delta);

	//init stuff.
	long x_counter = -max_delta/2;
	long y_counter = -max_delta/2;
	long z_counter = -max_delta/2;
	
	//our step flags
	bool x_can_step = 0;
	bool y_can_step = 0;
	bool z_can_step = 0;

	//how long do we delay for?
	if (micro_delay >= 16383)
		milli_delay = micro_delay / 1000;
	else
		milli_delay = 0;
        if (can_step(current_steps.x, target_steps.x, x_direction, 2)){
                enable_stepper(x_stepper);
        }
        if (can_step(current_steps.y, target_steps.y, y_direction, 3)){
                enable_stepper(y_stepper);
        }
        if (can_step(current_steps.z, target_steps.z, z_direction, 4)){
                enable_stepper(z_stepper);
        }
	//do our DDA line!
	do
	{
		x_can_step = can_step(current_steps.x, target_steps.x, x_direction, 2);
		y_can_step = can_step(current_steps.y, target_steps.y, y_direction, 3);
		z_can_step = can_step(current_steps.z, target_steps.z, z_direction, 4);

		if (x_can_step)
		{
			x_counter += delta_steps.x;
			
			if (x_counter > 0)
			{
  				do_step(x_stepper, x_direction);
				x_counter -= max_delta;
				if (x_direction){
					current_steps.x++;}
				else {
					current_steps.x--;}
			}
		}

		if (y_can_step)
		{
			y_counter += delta_steps.y;
			
			if (y_counter > 0)
			{
				do_step(y_stepper, y_direction);
				y_counter -= max_delta;
				if (y_direction) {
                                  current_steps.y++;}
				else {
				  current_steps.y--;}
			}
		}
		
		if (z_can_step)
		{
			z_counter += delta_steps.z;
			
			if (z_counter > 0)
			{
				do_step(z_stepper, z_direction);                     
				z_counter -= max_delta;
				if (z_direction){
					current_steps.z++;}
				else {
					current_steps.z--;}
			}
		}
		
		//wait for next step.
		if (milli_delay > 0)
			delay(milli_delay);			
		else
			delayMicrosecondsInterruptible(micro_delay);
	}
	while (x_can_step || y_can_step || z_can_step);
	disable_stepper(x_stepper);
        disable_stepper(y_stepper);
        disable_stepper(z_stepper);
	//set our points to be the same
	current_units.x = target_units.x;
	current_units.y = target_units.y;
	current_units.z = target_units.z;
	calculate_deltas();
}


/*int read_twi_val(int id)
{
    int c=3;
//    Serial.println("SENDING 7 ..");
    Wire.beginTransmission(id); // transmit to device with "id"
    Wire.send(7);        // sends one byte  
    Wire.endTransmission();
    // stop transmitting
    
//    Serial.println("Reading ..");
    //Wire.beginTransmission(id);
    Wire.requestFrom(id, 1);    // request 1 byte from slave device "id"
    while(Wire.available())    // slave may send less than requested
    { 
    c = Wire.receive(); // receive a byte as character
    }
    //Wire.endTransmission();
    Serial.println(c);
    return c; 
}
*/
bool can_step(long current, long target, byte dir, int id)
{
  	//stop us if we're on target
	if (target == current)
		return false;
  
// not implemented yet maybe later
//#if ENDSTOPS_MIN_ENABLED == 1
	//stop us if we're home and still going 
//	else if ((read_twi_val(id)==0) && !dir)
//		return false;
//#endif
/*
#if ENDSTOPS_MAX_ENABLED == 1
	//stop us if we're at max and still going
 	else if (read_switch(max_pin) && dir)
 		return false;
#endif
*/
	//default to being able to step
	return true;
}

void do_step(int id, bool dir)
{
//  sends step command trough i2c  
/*
id 2 = X axis
id 3 = Y axis
id 4 = Z axis
*/
  if (id==2) {
#if INVERT_X_DIR == 1
  if (dir) {
          send_twi_command(id, 2);  
  }
  else {
          send_twi_command(id, 1);  
  }
#else
  if (dir){
          send_twi_command(id, 1);  
  }
  else {
          send_twi_command(id, 2);  
  }
#endif
  }
  if (id==3) {
#if INVERT_Y_DIR == 1
  if (dir) {
          send_twi_command(id, 2);  
  }
  else {
          send_twi_command(id, 1);  
  }
#else
  if (dir){
          send_twi_command(id, 1);  
  }
  else {
          send_twi_command(id, 2);  
  }
#endif
  }
  if (id==4) {
#if INVERT_Z_DIR == 1
  if (dir) {
          send_twi_command(id, 2);  
  }
  else {
          send_twi_command(id, 1);  
  }
#else
  if (dir){
          send_twi_command(id, 1);  
  }
  else {
          send_twi_command(id, 2);  
  }
#endif
  }
// delay for 5 miliseconds so the motors have time to actually step
delayMicrosecondsInterruptible(5);  
}

/*
bool read_switch(byte pin)
{
	//dual read as crude debounce
	#if ENDSTOPS_INVERTING == 1
		return !digitalRead(pin) && !digitalRead(pin);
	#else
		return digitalRead(pin) && digitalRead(pin);
	#endif
}
*/

long to_steps(float steps_per_unit, float units)
{
	return steps_per_unit * units;
}

void set_target(float x, float y, float z)
{
	target_units.x = x;
	target_units.y = y;
	target_units.z = z;

	calculate_deltas();
}

void set_position(float x, float y, float z)
{
	current_units.x = x;
	current_units.y = y;
	current_units.z = z;
	
	calculate_deltas();
}

void calculate_deltas()
{
	//figure our deltas.
	delta_units.x = abs(target_units.x - current_units.x);
	delta_units.y = abs(target_units.y - current_units.y);
	delta_units.z = abs(target_units.z - current_units.z);
				
	//set our steps current, target, and delta
	current_steps.x = to_steps(x_units, current_units.x);
	current_steps.y = to_steps(y_units, current_units.y);
	current_steps.z = to_steps(z_units, current_units.z);

	target_steps.x = to_steps(x_units, target_units.x);
	target_steps.y = to_steps(y_units, target_units.y);
	target_steps.z = to_steps(z_units, target_units.z);

	delta_steps.x = abs(target_steps.x - current_steps.x);
	delta_steps.y = abs(target_steps.y - current_steps.y);
	delta_steps.z = abs(target_steps.z - current_steps.z);

	//what is our direction	
        x_direction = (target_units.x >= current_units.x);
	y_direction = (target_units.y >= current_units.y);
	z_direction = (target_units.z >= current_units.z);
}


long calculate_feedrate_delay(float feedrate)
{
	//how long is our line length?
	float distance = sqrt(delta_units.x*delta_units.x + 
                              delta_units.y*delta_units.y + 
                              delta_units.z*delta_units.z);
	long master_steps = 0;
	
	//find the dominant axis.
	if (delta_steps.x > delta_steps.y)
	{
		if (delta_steps.z > delta_steps.x)
			master_steps = delta_steps.z;
		else
			master_steps = delta_steps.x;
	}
	else
	{
		if (delta_steps.z > delta_steps.y)
			master_steps = delta_steps.z;
		else
			master_steps = delta_steps.y;
	}

	//calculate delay between steps in microseconds.  this is sort of tricky, but not too bad.
	//the formula has been condensed to save space.  here it is in english:
        // (feedrate is in mm/minute)
	// distance / feedrate * 60000000.0 = move duration in microseconds
	// move duration / master_steps = time between steps for master axis.
        // plus the delay for one step
	return ((distance * 60000000.0) / feedrate) / master_steps;	
}

long getMaxSpeed()
{
	if (delta_steps.z > 0)
		return calculate_feedrate_delay(FAST_Z_FEEDRATE);
	else
		return calculate_feedrate_delay(FAST_XY_FEEDRATE);
}



void enable_stepper(int id)
{
  //send the command "3" - enable driver
  send_twi_command(id, 3); 
}

void disable_stepper(int id)
{
  //send the command "4" - kill driver
  send_twi_command(id, 4);
}

void delayMicrosecondsInterruptible(unsigned int us)
{

#if F_CPU >= 16000000L
    // for the 16 MHz clock on most Arduino boards

	// for a one-microsecond delay, simply return.  the overhead
	// of the function call yields a delay of approximately 1 1/8 us.
	if (--us == 0)
		return;

	// the following loop takes a quarter of a microsecond (4 cycles)
	// per iteration, so execute it four times for each microsecond of
	// delay requested.
	us <<= 2;

	// account for the time taken in the preceeding commands.
	us -= 2;
#else
    // for the 8 MHz internal clock on the ATmega168

    // for a one- or two-microsecond delay, simply return.  the overhead of
    // the function calls takes more than two microseconds.  can't just
    // subtract two, since us is unsigned; we'd overflow.
	if (--us == 0)
		return;
	if (--us == 0)
		return;

	// the following loop takes half of a microsecond (4 cycles)
	// per iteration, so execute it twice for each microsecond of
	// delay requested.
	us <<= 1;
    
    // partially compensate for the time taken by the preceeding commands.
    // we can't subtract any more than this or we'd overflow w/ small delays.
    us--;
#endif

	// busy wait
	__asm__ __volatile__ (
		"1: sbiw %0,1" "\n\t" // 2 cycles
		"brne 1b" : "=w" (us) : "0" (us) // 2 cycles
	);
}
/*
#ifdef TEST_MACHINE

// this section is just for testing the motors 
void X_motor_test()
{
    Serial.println("Moving X forward by 100 mm at half maximum speed.");
    set_target(100, 0, 0);
    dda_move(calculate_feedrate_delay(FAST_XY_FEEDRATE/2));
    
    Serial.println("Pause for 2 seconds.");
    delay(2000);
    
    Serial.println("Moving X back to the start.");
    set_target(0, 0, 0);
    dda_move(calculate_feedrate_delay(FAST_XY_FEEDRATE/2));
    
    Serial.println("Pause for 2 seconds."); 
    delay(2000);   
}

void Y_motor_test()
{

    Serial.println("Moving Y forward by 100 mm at half maximum speed.");
    set_target(0, 100, 0);
    dda_move(calculate_feedrate_delay(FAST_XY_FEEDRATE/2));
    
    Serial.println("Pause for 2 seconds.");
    delay(2000);
    
    Serial.println("Moving Y back to the start.");
    set_target(0, 0, 0);
    dda_move(calculate_feedrate_delay(FAST_XY_FEEDRATE/2));
    
    Serial.println("Pause for 2 seconds."); 
    delay(2000);     
}

void Z_motor_test()
{
    Serial.println("Moving Z down by 5 mm at half maximum speed.");
    set_target(0, 0, 5);
    dda_move(calculate_feedrate_delay(FAST_Z_FEEDRATE/2));
    
    Serial.println("Pause for 2 seconds.");
    delay(2000);
    
    Serial.println("Moving Z back to the start.");
    set_target(0, 0, 0);
    dda_move(calculate_feedrate_delay(FAST_Z_FEEDRATE/2));
    
    Serial.println("Pause for 2 seconds."); 
    delay(2000);     
}

#endif */
