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

void liftControl(void * ignore);

void operatorControl() {
	bool watchingFront = false;
	bool watchingBack = false;
	bool watchingEnc = false;
	bool hasPunched = false;
	// TaskHandle liftTask = taskCreate(liftControl);
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
		// if(isNewPress(btn8d)){
		// 	backGrabSet(!backGrabGet());
		// 	watchingBack = false;
		// }

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
			firstBase();
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
		else if(isPressed(btn8d)){
			dejaVu();
			mwuah();
		}

		lcdControl(NULL);

		delay(20);
	}
	// taskDelete(lcdMenu);
}

void liftControl(void * ignore){
	PID topLiftPid = initPid(0.3, 0.0, 0.2, getTl, topLiftSet, getTl());
	// PID botLiftPid = initPid(0.3, 0.0, 0.3, getBl, botLiftSet, getBl());
	setTarget(topLiftPid, 2000);
	while(true){
		if(isNewPress(btn7u)){
			lcdSetText(uart1, 1, "Start");
			startTask(topLiftPid);
		}
		else if(isNewPress(btn7d)){
			lcdSetText(uart1, 1, "Kill");
			killTask(topLiftPid);
			mset(topLift, 0, 127);
		}
		else if(isNewPress(btn7l)){
			lcdSetText(uart1, 1, "Pause");
			pauseTask(topLiftPid);
			mset(topLift, 0, 127);
		}
		else if(isNewPress(btn7r)){
			lcdSetText(uart1, 1, "Resume");
			resumeTask(topLiftPid);
		}
		delay(20);
	}
}

// void liftControl(void * ignore){
	// PID topLiftPid = initPid(0.12, 0.0, 0.16, getTl, topLiftSet, getTl());
	// PID botLiftPid = initPid(0.3, 0.0, 0.3, getBl, botLiftSet, getBl());
	// startTask(topLiftPid);
	// startTask(botLiftPid);
	// int state = 0;
	// // 3296 1850
	// // 364 1163
	// int at = 400;
	// int ab = 1163;
	// int bt = 3296;
	// int bb = 1850;
	// while(true){
	// 	lcdPrint(uart1, 1, "%d", state);
	// 	switch (state) {
	// 		case 0: // Nothing
	// 			pauseTask(topLiftPid);
	// 			pauseTask(botLiftPid);
	// 			topLiftSet(0);
	// 			botLiftSet(0);
	// 			break;
	// 		case 1: // Recording A
	// 			pauseTask(topLiftPid);
	// 			pauseTask(botLiftPid);
	// 			topLiftSet(0);
	// 			botLiftSet(0);
	// 			at = getTl();
	// 			ab = getBl();
	// 			break;
	// 		case 2: // Recording B
	// 			pauseTask(topLiftPid);
	// 			pauseTask(botLiftPid);
	// 			topLiftSet(0);
	// 			botLiftSet(0);
	// 			bt = getTl();
	// 			bb = getBl();
	// 			break;
	// 		case 3: // Seeking A
	// 			resumeTask(topLiftPid);
	// 			resumeTask(botLiftPid);
	// 			setTarget(topLiftPid, at);
	// 			setTarget(botLiftPid, ab);
	// 			break;
	// 		case 4: // Seeking B
	// 			resumeTask(topLiftPid);
	// 			resumeTask(botLiftPid);
	// 			setTarget(topLiftPid, bt);
	// 			setTarget(botLiftPid, bb);
	// 			break;
	// 		default:
	// 			state = 0;
	// 	}
	// 	if(isPressed(btn7l)) state = 1;
	// 	else if(isPressed(btn7r)) state = 3;
	// 	else if(isPressed(btn8l)) state = 2;
	// 	else if(isPressed(btn8r)) state = 4;
	// 	else if(isPressed(btn7d)) state = 0;
	// 	else if(isPressed(btn8d)){
	// 		state = 0;
	// 		killTask(topLiftPid);
	// 		killTask(botLiftPid);
	// 	}
	// 	else if(isPressed(btn8u)){
	// 		state = 0;
	// 		startTask(topLiftPid);
	// 		startTask(botLiftPid);
	// 	}
		// if(isPressed(btn7l)){
		// 	topLiftSet(0);
		// 	botLiftSet(0);
		// 	setTarget(topLiftPid, getTl());
		// 	setTarget(botLiftPid, getBl());
		// }
		// else{
		// 	int tlPwr = calculatePid(topLiftPid, getTl());
		// 	int blPwr = calculatePid(botLiftPid, getBl());
		// 	botLiftSet(blPwr);
		// 	topLiftSet(tlPwr);
		// }
	// 	delay(20);
	// }
// }
