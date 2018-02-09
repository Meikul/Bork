#ifndef MOVE_H_
#define MOVE_H_

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
int rectify(int value, int lowerLimit, int upperLimit);
bool isDrivingForward();
int getAccel();

// Global variables
extern Encoder rightEnc;
extern Encoder leftEnc;

#endif
