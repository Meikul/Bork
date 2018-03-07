#include "main.h"

/*
 * Runs pre-initialization code. This function will be started in kernel mode one time while the
 * VEX Cortex is starting up. As the scheduler is still paused, most API functions will fail.
 *
 * The purpose of this function is solely to set the default pin modes (pinMode()) and port
 * states (digitalWrite()) of limit switches, push buttons, and solenoids. It can also safely
 * configure a UART port (usartOpen()) but cannot set up an LCD (lcdInit()).
 */

 int motors[10] = {0,0,0,0,0,0,0,0,0,0};

 int linSpeed(int speed);

int linSpeed(int speed){
  bool negative = false;
  if (speed < 0) {
    negative = true;
    speed *= -1;
  }
  const unsigned int linMotors[128] =
	{
    0,  0,  0,  0,  0,  0,  0,  0, 12, 15,
	 18, 19, 21, 21, 22, 22, 22, 23, 24, 24,
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

void gateSet(bool isOpen){
  digitalWrite(baseGate, isOpen);
}

bool gateGet(){
  return digitalRead(baseGate);
}

void punchSet(bool isPunching){
  digitalWrite(shooter, isPunching);
}

void driveStop(){
  driveSetImm(0, 0);
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
  mset(port, targetSpeed, DEFAULT_SLEW);
}

void mset(int port, int targetSpeed, int slew){
  slew = abs(slew);
  targetSpeed = rectify(targetSpeed);
  int curSpeed = motors[port-1];
  int output = rectify(targetSpeed, curSpeed-slew, curSpeed+slew);
  motorSet(port, output);
  motors[port-1] = output;
}

void driveSetRamp(int left, int right, int slew){
  left = linSpeed(left);
  right = linSpeed(right);
  mset(dr2, -right, slew);
  mset(dr3, right, slew);
  mset(dr4, -right, slew);
  mset(dr5, right, slew);
  mset(dl1, -left, slew);
  mset(dl2, -left, slew);
  mset(dl3, -left, slew);
  mset(dl5, -left, slew);
}

void driveSetBoth(int speed){
  driveSet(speed, speed);
}

void driveSet(int left, int right){
  left = linSpeed(left);
  right = linSpeed(right);
  mset(dr2, -right);
  mset(dr3, right);
  mset(dr4, -right);
  mset(dr5, right);
  mset(dl1, -left);
  mset(dl2, -left);
  mset(dl3, -left);
  mset(dl5, -left);
}

void driveSetLeft(int speed){
  speed = linSpeed(speed);
  mset(dl1, -speed);
  mset(dl2, -speed);
  mset(dl3, -speed);
  mset(dl5, -speed);
}

void driveSetRight(int speed){
  speed = linSpeed(speed);
  mset(dr2, -speed);
  mset(dr3, speed);
  mset(dr4, -speed);
  mset(dr5, speed);
}

unsigned int getProx(){
  unsigned int val = ultrasonicGet(prox);
  return (val * 0.393701);

}

void driveSetImm(int left, int right){
  left = linSpeed(left);
  right = linSpeed(right);
  mset(dr2, -right, 255);
  mset(dr3, right, 255);
  mset(dr4, -right, 255);
  mset(dr5, right, 255);
  mset(dl1, -left, 255);
  mset(dl2, -left, 255);
  mset(dl3, -left, 255);
  mset(dl5, -left, 255);
}

void driveSetNoLin(int left, int right){
  mset(dr2, -right);
  mset(dr3, right);
  mset(dr4, -right);
  mset(dr5, right);
  mset(dl1, -left);
  mset(dl2, -left);
  mset(dl3, -left);
  mset(dl5, -left);
}

bool isOverLine(){
  return (isOverLine(lineLeft) || isOverLine(lineRight));
}

bool isOverLine(int port){
  unsigned int val = analogRead(port);
  return (val < 1100);
}

bool isOverBase(int port){
  unsigned int val = analogRead(port);
  return (val < 2990);
}

bool isDrivingForward(){
  int stick = joystickGetAnalog(1, 3);
  return (stick > 0);
}

int rectify(int val, int lowerLim, int upperLim){
  if(val < lowerLim) val = lowerLim;
  else if(val > upperLim) val = upperLim;
  return val;
}

int rectify(int val){
  return rectify(val, -127, 127);
}

void resetGyro(){
  gyroReset(gyro);
}

int getGyro(){
  return gyroGet(gyro);
}

int getAccel(){
  const int accelBias = 1785;
  return analogRead(accel) - accelBias;
}

void topLiftSet(int power){
  mset(topLift, -power, 30);
}

void topLiftSetRamp(int power, int slew){
  mset(topLift, -power, slew);
}

void botLiftSetRamp(int power, int slew){
  mset(botLift, -power, slew);
}

void botLiftSet(int power){
  mset(botLift, -power, 30);
}

int getTl(){
	return analogRead(tlPot);
}

int getBl(){
  return analogRead(blPot);
}
