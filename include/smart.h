#ifndef SMART_H_
#define SMART_H_

#include <API.h>

extern Semaphore isDoneDriving;

void driveStraight(int speed);

void driveDist(double feet);

void driveDist(double feet, char stopOnCode);

void drivePidArgs(double kp, double ki, double kd, int targetLeft, int targetRight);

void driveDistSync(double feet);

void driveWait(double feet, int leftPwr, int rightPwr);

void driveWaitRamp(double feet, int leftPwr, int rightPwr, int slew);

void driveTurnDeg(int degrees);

void smartGrabFront(int speed);

#endif
