#ifndef FUNC_H_
#define FUNC_H_

#include <API.h>

/**
 * Sets a motor with ramping
 */
void mset();
/**
 * Sets drive speed with linearization
 * @param left  speed of left
 * @param right speed of right
 */
void driveSet(int left, int right);
void frontGrabSet(bool isGrabbing);
void backGrabSet(bool isGrabbing);
void tiltSet(bool isTilting);
void punchSet(bool isPunching);
bool tiltGet();
bool frontGrabGet();
bool backGrabGet();
void punch();
void driveStraight(int speed);
int rectify(int value, int lowerLimit, int upperLimit);
void driveDist(double feet);
bool isDrivingForward();

// Global variables
extern Encoder rightEnc;
extern Encoder leftEnc;
extern int sysState;
extern int autonState;
extern Semaphore isDoneDriving;
extern Semaphore isDoneLifting;
extern Mutex settingDrive;

#endif
