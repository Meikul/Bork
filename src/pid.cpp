#include "pid.h"
#include "main.h"

struct pidst{
  double kp;
  double ki;
  double kd;
  int integ;
  unsigned int outerIntegLimit;
  unsigned int innerIntegLimit;
  int lastError;
  int target;
  unsigned long lastTime;
  unsigned int maxSpeed;
  unsigned char arg;
  // IntFunction input;
};

int rectify(int val, int lim){
  if(val > abs(lim)) val = abs(lim);
  else if(val < -abs(lim)) val = -abs(lim);
  return val;
}

PID initPid(double kp, double ki, double kd, int process, int initTarget){
  pidst* pid = new pidst;
  pid->kp = kp;
  pid->ki = ki;
  pid->kd = kd;
  pid->integ = 0;
  pid->outerIntegLimit = 0;
  pid->innerIntegLimit = 0;
  pid->lastError = 100000;
  pid->lastTime = millis();
  pid->maxSpeed = 127;
  pid->target = initTarget;
  calculatePid(pid, process);
  return pid;
}

void setGains(PID pidptr, double kp, double ki, double kd){
  pidst *pid = (pidst*) pidptr;
  pid->kp = kp;
  pid->ki = ki;
  pid->kd = kd;
}

void setMaxSpeed(PID pidptr, int val){
  pidst *pid = (pidst*) pidptr;
  pid->maxSpeed = abs(val);
}

int getTarget(PID pidptr){
  pidst *pid = (pidst*) pidptr;
  return pid->target;
}

void setTarget(PID pidptr, int val){
  pidst *pid = (pidst*) pidptr;
  pid->target = val;
  lcdSetText(uart1, 1, "Target");
  lcdPrint(uart1, 2, "%d", val);
}

int calculatePid(PID pidptr, int process){
  pidst *pid = (pidst*) pidptr;

  int error = pid->target - process;
  double deltaTime = (millis() - pid->lastTime)/1000.0;
  unsigned int absError = abs(error);

  if(pid->lastError == 100000) pid->lastError = error; // uninitialized

  double diff = 0;
  // if(deltaTime > 0) diff = (error - pid->lastError) / deltaTime;
  if(deltaTime > 0) diff = (error - pid->lastError) / deltaTime;





  if(absError <= pid->outerIntegLimit){
    if(absError >= pid->innerIntegLimit) pid->integ += error * deltaTime;
  }
  else pid->integ = 0;

  int pwr = (error * pid->kp) + (pid->integ * pid->ki) + (diff * pid->kd);


  pwr = rectify(pwr, 127);
  pwr = rectify(pwr, pid->maxSpeed);

  lcdPrint(uart1, 1, "Pos %d Tar %d", process, pid->target);
  lcdPrint(uart1, 2, "Err %d Pwr %d", error, pwr);

  pid->lastTime = millis();
  pid->lastError = error;

  return pwr;
}
