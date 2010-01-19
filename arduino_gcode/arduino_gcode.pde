//G-code interpreter based on the reprap code

#include "parameters.h"

void setup()
{
 	Serial.begin(19200);
	Serial.println("start");
	//other initialization.
	init_process_string();
	init_steppers();
}

void loop()
{
get_and_do_command();
}
