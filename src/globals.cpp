#include "main.h"

/*
 * Runs pre-initialization code. This function will be started in kernel mode one time while the
 * VEX Cortex is starting up. As the scheduler is still paused, most API functions will fail.
 *
 * The purpose of this function is solely to set the default pin modes (pinMode()) and port
 * states (digitalWrite()) of limit switches, push buttons, and solenoids. It can also safely
 * configure a UART port (usartOpen()) but cannot set up an LCD (lcdInit()).
 */

 Encoder rightEnc;
 Encoder leftEnc;
 int sysState = 1;
 int autonState = 0;
 int sensorIndex = 0;
 int motors[10] = {0,0,0,0,0,0,0,0,0,0};

 TaskHandle drivingTask;
 Mutex settingDrive = mutexCreate();
 Semaphore isDoneDriving = semaphoreCreate();
 Semaphore isDoneLifting = semaphoreCreate();

 void autonSelect();
 void sensorView();
 void lcdSet(const char *line1, const char *line2);
 void lcdSet(int line, const char *string);
 int linSpeed(int speed);
 void drivePid();

int linSpeed(int speed){
  bool negative = false;
  if (speed < 0) {
    negative = true;
    speed *= -1;
  }
  const unsigned int linMotors[128] =
	{
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		0, 21, 21, 21, 22, 22, 22, 23, 24, 24,
	 25, 25, 25, 25, 26, 27, 27, 28, 28, 28,
	 28, 29, 30, 30, 30, 31, 31, 32, 32, 32,
	 33, 33, 34, 34, 35, 35, 35, 36, 36, 37,
	 37, 37, 37, 38, 38, 39, 39, 39, 40, 40,
	 41, 41, 42, 42, 43, 44, 44, 45, 45, 46,
	 46, 47, 47, 48, 48, 49, 50, 50, 51, 52,
	 52, 53, 54, 55, 56, 57, 57, 58, 59, 60,
	 61, 62, 63, 64, 65, 66, 67, 67, 68, 70,
	 71, 72, 72, 73, 74, 76, 77, 78, 79, 79,
	 80, 81, 83, 84, 84, 86, 86, 87, 87, 88,
	 88, 89, 89, 90, 90,127,127,127
 };
 speed = linMotors[speed];
 if (negative) speed *= -1;
 return speed;
}

void frontGrabSet(bool isGrabbing){
  digitalWrite(frontGrab, isGrabbing);
}

bool frontGrabGet(){
  return digitalRead(frontGrab);
}

void backGrabSet(bool isGrabbing){
  digitalWrite(backGrab, isGrabbing);
}

bool backGrabGet(){
  return digitalRead(backGrab);
}

void tiltSet(bool isTilting){
  digitalWrite(tilter, isTilting);
}

bool tiltGet(){
  return digitalRead(tilter);
}

void punchSet(bool isPunching){
  digitalWrite(shooter, isPunching);
}

void punch(){
  if(frontGrabGet()){
    punchSet(true);
    delay(500);
    punchSet(false);
  }
}

void driveStraight(int speed){
  // double kp = 0.1;
  // double ki = 0.0;
  // double kd = 0.0;
  // static int lastError = 0;
  // int error = encoderGet(leftEnc) - encoderGet(rightEnc);
  //
  // int adjustedSpeed = 0;
  //
  // driveSet(speed, adjustedSpeed);
  //
  // lastError = error;
}

void mset(int port, int targetSpeed){
  targetSpeed = rectify(targetSpeed, -127, 127);
  static const unsigned int slew = 30;
  int curSpeed = motors[port-1];
  int output = rectify(targetSpeed, curSpeed-slew, curSpeed+slew);
  motorSet(port, output);
  motors[port-1] = output;
}

void driveSet(int left, int right){
  left = linSpeed(left);
  right = linSpeed(right);
  mset(dr1, right);
  mset(dr2, -right);
  mset(dr3, right);
  mset(dr4, -right);
  mset(dr5, right);
  mset(dl1, -left);
  mset(dl2, -left);
  mset(dl3, -left);
  mset(dl4, left);
  mset(dl5, -left);
}

int feetToTicksLeft(double feet){
  const int ticksPerFoot = 290;
  return (feet * ticksPerFoot);
}

int degToTicksLeft(int degrees){
  const double footPerDeg = 0.02617994;
  double driveDistance = degrees * footPerDeg / 1.41;
  return feetToTicksLeft(driveDistance);
}

double ticksToFeetLeft(int ticks){
  // const double circumference = 12.566372;
  return (ticks / 290.0);
}

int feetToTicksRight(double feet){
  const int ticksPerFoot = 280;
  return (feet * ticksPerFoot);
}

int degToTicksRight(int degrees){
  const double footPerDeg = 0.02617994;
  double driveDistance = degrees * footPerDeg / 1.41;
  return feetToTicksRight(driveDistance);
}

double ticksToFeetRight(int ticks){
  return (ticks / 280.0);
}

long driveTargetRight = 0;
long driveTargetLeft = 0;

void driveTurnDeg(int degrees){
  driveTargetRight = degToTicksRight(degrees);
  driveTargetLeft = degToTicksLeft(degrees);
  drivingTask = taskRunLoop(drivePid, 20);
}

void driveDist(double feet){
  driveTargetRight = feetToTicksRight(feet);
  driveTargetLeft = feetToTicksLeft(feet);
  drivingTask = taskRunLoop(drivePid, 20);
}

void drivePid(){
	const double kp = 0.3;
	const double ki = 0.0;
	const double kd = 2.0;

	static double integL;
	static double integR;
	int encL = encoderGet(rightEnc);
	int encR = encoderGet(leftEnc);
	int errorL = driveTargetLeft - encL;
	int errorR = driveTargetRight - encR;

	static int prevErrorL;
	static int prevErrorR;
	int deltaErrorL = (errorL - prevErrorL);
	int deltaErrorR = (errorR - prevErrorR);

	if(abs(errorL) < 200){
		integL += errorL;
    rectify(integL, -127, 127);
	}
	else{
		integL = 0;
	}

	if(abs(errorR) < 400){
		integR += errorR;
    rectify(integR, -127, 127);
	}
	else{
		integR = 0;
	}

	int pwrL = (errorL * kp) + (integL * ki) + (deltaErrorL * kd);
	int pwrR = (errorR * kp) + (integR * ki) + (deltaErrorR * kd);
	driveSet(pwrL, pwrR);
	prevErrorL = errorL;
	prevErrorR = errorR;
  if(abs(errorR) < 5 && abs(errorL) < 5){
    semaphoreGive(isDoneDriving);
    driveSet(0, 0);
    taskDelete(drivingTask);
  }
}

void lcdControl(){
  switch (sysState) {
    case 0: // Auton select
      autonSelect();
      break;
    case 1: // Sensor view
      sensorView();
      break;
  }
  if(isNewPress(lcdRight)) sensorIndex++;
  else if(isNewPress(lcdLeft)) sensorIndex--;
}

void autonSelect(){
  switch (autonState) {
    case 0: // Whole auton
      lcdSet(2, "Whole Auton");
      break;
  }
}

void sensorView(){
  lcdPrint(uart1, 1, "Sensors %d", sensorIndex);
  sensorIndex = rectify(sensorIndex, 0, 3);
  switch (sensorIndex) {
    case 0:
      lcdPrint(uart1, 2, "Front %d", analogRead(frontLight));
      break;
    case 1:
      lcdPrint(uart1, 2, "L %d R %d", encoderGet(leftEnc), encoderGet(rightEnc));
      if(isPressed(lcdMid)){
        encoderReset(leftEnc);
        encoderReset(rightEnc);
      }
      break;
    case 2:
      lcdPrint(uart1, 2, "T %d Ft %f", encoderGet(leftEnc), ticksToFeetLeft(encoderGet(leftEnc)));
      if(isPressed(lcdMid)){
        encoderReset(leftEnc);
        encoderReset(rightEnc);
      }
      break;
    case 3:
      lcdPrint(uart1, 2, "T %d Ft %f", encoderGet(rightEnc), ticksToFeetLeft(encoderGet(rightEnc)));
      if(isPressed(lcdMid)){
        encoderReset(leftEnc);
        encoderReset(rightEnc);
      }
      break;
  }
}

void lcdSet(const char *line1, const char *line2){
  lcdSetText(uart1, 1, line1);
  lcdSetText(uart1, 2, line2);
}

void lcdSet(int line, const char *string){
  lcdSetText(uart1, line, string);
}

int rectify(int val, int lowerLim, int upperLim){
  if(val < lowerLim) val = lowerLim;
  else if(val > upperLim) val = upperLim;
  return val;
}
