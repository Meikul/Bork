/** @file opcontrol.c
 * @brief File for operator control code
 *
 * This file should contain the user operatorControl() function and any functions related to it.
 *
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/
 *
 * PROS contains FreeRTOS (http://www.freertos.org) whose source code may be
 * obtained from http://sourceforge.net/projects/freertos/files/ or on request.
 */

#include "main.h"

/*
 * Runs the user operator control code. This function will be started in its own task with the
 * default priority and stack size whenever the robot is enabled via the Field Management System
 * or the VEX Competition Switch in the operator control mode. If the robot is disabled or
 * communications is lost, the operator control task will be stopped by the kernel. Re-enabling
 * the robot will restart the task, not resume it from where it left off.
 *
 * If no VEX Competition Switch or Field Management system is plugged in, the VEX Cortex will
 * run the operator control task. Be warned that this will also occur if the VEX Cortex is
 * tethered directly to a computer via the USB A to A cable without any VEX Joystick attached.
 *
 * Code running in this task can take almost any action, as the VEX Joystick is available and
 * the scheduler is operational. However, proper use of delay() or taskDelayUntil() is highly
 * recommended to give other tasks (including system tasks such as updating LCDs) time to run.
 *
 * This task should never exit; it should end with some kind of infinite loop, even if empty.
 */
void operatorControl() {
	bool watchingFront = false;
	bool watchingBack = false;
	while (1) {
		// Drive
		int rStick = joystickGetAnalog(1, 2);
		int lStick = joystickGetAnalog(1, 3);
		driveSet(lStick, rStick);

		// Grab
		if(isNewPress(btn8u)){
			frontGrabSet(!frontGrabGet());
			watchingFront = false;
		}
		if(isNewPress(btn8d)){
			backGrabSet(!backGrabGet());
			watchingBack = false;
		}

		if(isNewPress(btn6u) || watchingFront){
			if(frontGrabGet()){
				frontGrabSet(false);
				watchingFront = false;
			}
			else{
				watchingFront = true;
				if(!digitalRead(frontSwitch)){
					frontGrabSet(true);
					watchingFront = false;
				}
			}
		}

		if(isNewPress(btn6d) || watchingBack){
			if(backGrabGet()){
				backGrabSet(false);
				watchingBack = false;
			}
			else{
				watchingBack = true;
				if(!digitalRead(backSwitch)){
					backGrabSet(true);
					watchingBack = false;
				}
			}
		}

		if(!isPressed(btn6u)) watchingFront = false;
		if(!isPressed(btn6d)) watchingBack = false;

		// Tilt
		tiltSet(isPressed(btn5u));
		
		punchSet(isPressed(btn5d));

		// Punch
		// if(isPressed(btn8d)){
		// 	punch();
		// }

		lcdPrint(uart1, 1, "6u %d 5d %d 8u %d", isPressed(btn6u), isPressed(btn5d), isPressed(btn8u));
		// lcdPrint(uart1, 2, "5u %d 5d %d", isPressed(btn5u), isPressed(btn5d));
		// lcdControl();

		delay(20);
	}
}
