#ifndef PARAMS_H
#define PARAMS_H

// Communication
#define SERIAL_BAUD 9600  

//PINS
#define PIN_LCD_WRITE 3
#define DIRECTION_PIN 4
#define SERVOMOTOR_PIN 6
#define INITIAL_THETA 110  // Initial angle of the servomotor

// Min and max values for motors
#define THETA_MIN 60
#define THETA_MAX 150
#define SPEED_MAX 100

// If DEBUG is set to true, the arduino will send back all the received messages
#define DEBUG false

#endif