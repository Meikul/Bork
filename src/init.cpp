/** @file init.c
 * @brief File for initialization code
 *
 * This file should contain the user initialize() function and any functions related to it.
 *
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/
 *
 * PROS contains FreeRTOS (http://www.freertos.org) whose source code may be
 * obtained from http://sourceforge.net/projects/freertos/files/ or on request.
 */

#include "main.h"

// Define globals
Encoder rightEnc;
Encoder leftEnc;
Gyro gyro;


/*
 * Runs pre-initialization code. This function will be started in kernel mode one time while the
 * VEX Cortex is starting up. As the scheduler is still paused, most API functions will fail.
 *
 * The purpose of this function is solely to set the default pin modes (pinMode()) and port
 * states (digitalWrite()) of limit switches, push buttons, and solenoids. It can also safely
 * configure a UART port (usartOpen()) but cannot set up an LCD (lcdInit()).
 */
void initializeIO() {
  // watchdogInit();
  for(int i=1; i<=6; i++){
    pinMode(i, INPUT);
  }
  for(int i=7; i<=12; i++){
    pinMode(i, OUTPUT);
    digitalWrite(i, 0);
  }
}

void initialize() {
  lcdInit(uart1);
  lcdSetBacklight(uart1, 1);
  lcdSetText(uart1, 1, "      USURP");
  lcdSetText(uart1, 2, "    Bork 1.0");
  rightEnc = encoderInit(dre1, dre2, true);
  leftEnc = encoderInit(dle1, dle2, false);
  gyro = gyroInit(gyroPort, 0);
}
