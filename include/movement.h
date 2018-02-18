#ifndef MOVE_H_
#define MOVE_H_

#include <API.h>

/**
 * Sets a motor with ramping
 */
void mset(int port, int speed);
void mset(int port, int speed, int slew);
/**
 * Sets drive speed with linearization
 * @param left  speed of left
 * @param right speed of right
 */
void driveSet(int left, int right);
void driveSetRamp(int left, int right, int slew);
void driveSetImm(int left, int right);
void driveSetNoLin(int left, int right);
void frontGrabSet(bool isGrabbing);
void backGrabSet(bool isGrabbing);
void gateSet(bool isOpen);
void punchSet(bool isPunching);
bool gateGet();
bool frontGrabGet();
bool backGrabGet();
int rectify(int value, int lowerLimit, int upperLimit);
int rectify(int value);
bool isDrivingForward();
int getAccel();
bool isUnderBase(int port);

// Global variables
extern Encoder rightEnc;
extern Encoder leftEnc;

#endif
