#ifndef PARAMETERS_H
#define PARAMETERS_H

#define TEST_MACHINE

// Set 1s where you have endstops; 0s where you don't
// not in use
#define ENDSTOPS_MIN_ENABLED 0
#define ENDSTOPS_MAX_ENABLED 0

//our command string length
#define COMMAND_SIZE 128

#define INCHES_TO_MM 25.4

// define the parameters of our machine.
#define X_STEPS_PER_MM   76
#define X_STEPS_PER_INCH (X_STEPS_PER_MM*INCHES_TO_MM)
#define X_MOTOR_STEPS    96
#define INVERT_X_DIR 1

#define Y_STEPS_PER_MM   76
#define Y_STEPS_PER_INCH (Y_STEPS_PER_MM*INCHES_TO_MM)
#define Y_MOTOR_STEPS   96
#define INVERT_Y_DIR 0

#define Z_STEPS_PER_MM   76
#define Z_STEPS_PER_INCH (Z_STEPS_PER_MM*INCHES_TO_MM)
#define Z_MOTOR_STEPS    96
#define INVERT_Z_DIR 1

//our maximum feedrates
#define FAST_XY_FEEDRATE 1000.0
#define FAST_Z_FEEDRATE  800.0

// Units in curve section
#define CURVE_SECTION_MM 0.5
#define CURVE_SECTION_INCHES (CURVE_SECTION_MM*INCHES_TO_MM)

// Set to one if enable pins are inverting
// For RepRap stepper boards version 1.x the enable pins are *not* inverting.
// For RepRap stepper boards version 2.x and above the enable pins are inverting.
#define INVERT_ENABLE_PINS 0

// Set to one if sensor outputs inverting (ie: 1 means open, 0 means closed)
// RepRap opto endstops are *not* inverting.
#define ENDSTOPS_INVERTING 0

#endif
