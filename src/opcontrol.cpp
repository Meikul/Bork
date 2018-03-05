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
	bool watchingEnc = false;
	bool hasPunched = false;
	// TaskHandle lcdMenu = taskCreate(lcdControl, TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT);
	while (1) {
		// Drive
		int rStick = joystickGetAnalog(1, 2);
		int lStick = joystickGetAnalog(1, 3);
		if(abs(rStick) < 10) rStick = 0;
		if(abs(lStick) < 10) lStick = 0;
		driveSet(lStick, rStick);

		// Grab
		if(isNewPress(btn8u)){
			frontGrabSet(!frontGrabGet());
			gateSet(false);
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
				if(isOverBase(frontLight)){
					if(isDrivingForward() && !watchingEnc) {
						encoderReset(leftEnc);
						watchingEnc = true;
					}
					else if(isDrivingForward() && watchingEnc){
						if(joystickGetAnalog(1, 3) > 20 || encoderGet(leftEnc) > 50){
							frontGrabSet(true);
							gateSet(false);
							watchingEnc = false;
							watchingFront = false;
						}
					}
					else{
						frontGrabSet(true);
						gateSet(false);
						watchingFront = false;
						watchingEnc = false;
					}
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
				if(isOverBase(backLight)){
					if(isDrivingForward() && !watchingEnc) {
						encoderReset(leftEnc);
						watchingEnc = true;
					}
					else if(isDrivingForward() && watchingEnc){
						if(joystickGetAnalog(1, 3) > 20 || encoderGet(leftEnc) > 80){
							backGrabSet(true);
							watchingEnc = false;
							watchingBack = false;
						}
					}
					else{
						backGrabSet(true);
						watchingBack = false;
						watchingEnc = false;
					}
				}
			}
		}

		if(isNewPress(btn8l)) digitalWrite(claw, !digitalRead(claw));

		if(!isPressed(btn6u)) watchingFront = false;
		if(!isPressed(btn6d)) watchingBack = false;

		if(isNewPress(btn7u)) gateSet(!gateGet());

		if(isPressed(btn5u)){
				punchSet(true);
		}
		if(isNewRelease(btn5u)){
			punchSet(false);
			frontGrabSet(false);
			watchingFront = false;
		}
		else if(!isPressed(btn5u)){
			if(isPressed(btn5d)){
				// if(digitalRead(launchSwitch)){
				// 	punchSet(true);
				// 	hasPunched = true;
				// }
				if(isOverLine()){
					encoderReset(leftEnc);
					while(encoderGet(leftEnc) < launchDelay && isPressed(btn5d)){
						delay(10);
					}
					if(isPressed(btn5d)){
						punchSet(true);
						hasPunched = true;
					}
				}
			}
			else if(!hasPunched){
				hasPunched = false;
				punchSet(false);
			}
			if(isNewRelease(btn5d) && hasPunched){
				punchSet(false);
				frontGrabSet(false);
				watchingFront = false;
			}
		}

		if(isPressed(btn7l)){
			autonomous();
		}
		else if(isPressed(btn7r)){
			cornerLaunch();
		}
		else if(isPressed(btn7d)){
			firstCorner();
		}
		else if(isPressed(btn8r)){
			firstTraverse();
		}
		else if(isPressed(btn8l)){
			secondCorner();
		}
		lcdControl(NULL);

		delay(20);
	}
	// taskDelete(lcdMenu);
}
