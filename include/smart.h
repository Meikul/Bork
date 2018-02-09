#ifndef SMART_H_
#define SMART_H_

#include <API.h>

extern Semaphore isDoneDriving;

void driveStraight(int speed);

void driveDist(double feet);

#endif
