#include "main.h"

TaskHandle drivingTask;

void drivePid(void * parameter);

int feetToTicksLeft(double feet){
  const int ticksPerFoot = 290;
  return (feet * ticksPerFoot);
}

int degToTicksLeft(int degrees){
  const double footPerDeg = 0.02617994;
  double driveDistance = degrees * footPerDeg / 1.41;
  return feetToTicksLeft(driveDistance);
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

long driveTargetRight = 0;
long driveTargetLeft = 0;

void driveTurnDeg(int degrees){
  driveTargetRight = degToTicksRight(degrees);
  driveTargetLeft = degToTicksLeft(degrees);
  drivingTask = taskCreate(drivePid, TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT);
}

void drivePid(void * parameter){
  bool done = false;
  while(!done){
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
    if(abs(errorR) < 1 && abs(errorL) < 1){
      done = true;
      driveSet(0, 0);
    }
    delay(20);
  }
  semaphoreGive(isDoneDriving);
}

void driveDist(double feet){
  driveTargetRight = feetToTicksRight(feet);
  driveTargetLeft = feetToTicksLeft(feet);
  drivingTask = taskCreate(drivePid, TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT);
}
