//version 1.0
// This file is for joystick and objects definitions. All external libraries must be included here as well.
#include <Wire.h>
#include <Joystick.h>
#include <Adafruit_ADS1015.h>
#include <Keyboard.h>



//Joystick_ b8stick(0x20,JOYSTICK_TYPE_GAMEPAD,
//  6, 0,                  // Button Count, Hat Switch Count
//  true, true, false,     // X and Y, but no Z Axis
//  false, false, false,   // No Rx, Ry, or Rz
//  false, false,          // No rudder or throttle
//  false, false, false);  // No accelerator, brake, or steering

Joystick_ simchair(0x20, 0x04, 32, 1, true, true, true, true, true, false, true, true, false, false, false);
Joystick_ simchair_aux1(0x21, 0x04, 32, 1, true, true, true, true, true, true, true, true, true, true, true);
Joystick_ simchair_aux2(0x22, 0x04, 96, 1, false, false, false, true, true, false, false, false, false, false, false);

Adafruit_ADS1115 cyclic;
Adafruit_ADS1115 pedals(0x4A);


