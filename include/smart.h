#ifndef SMART_H_
#define SMART_H_

#include <API.h>

extern Semaphore isDoneDriving;

void driveStraight(int speed);

void driveDist(double feet);

void driveDist(double feet, char stopOnCode);

void driveDist(double feet, int slew, char stopOnCode);

void drivePidArgs(double kp, double ki, double kd, int targetLeft, int targetRight);

void driveDistSync(double feet);

void driveWait(double feet, int power);

void driveWaitRamp(double feet, int power, int slew);

void driveWait(double feet, int leftPwr, int rightPwr);

void driveTime(unsigned long waitTime, int power);

void driveWaitRamp(double feet, int leftPwr, int rightPwr, int slew);

void driveTurnDeg(int degrees);

void smartGrabFront(int speed);

void smartGrabBack(int speed);

void lineUp(double feet);

void stopOnLine(double feet);

void stopOnLine(double feet, int slew);

void stopOnLineBlind(int speed, int slew);

void stopOnLineBlind(int speed);

#endif
