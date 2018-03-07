#include "pid.h"
#include "main.h"

struct pidst{
  double kp;
  double ki;
  double kd;
  int integ;
  unsigned int outerIntegLimit;
  unsigned int innerIntegLimit;
  int lastInput;
  int target;
  unsigned long lastTime;
  unsigned int maxSpeed;
  unsigned char arg;
  IntFunction input;
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
  pid->lastInput = process;
  pid->lastTime = millis();
  pid->maxSpeed = 127;
  pid->target = initTarget;
  pid->input = NULL;
  calculatePid(pid, process);
  return pid;
}

PID initPid(double kp, double ki, double kd, IntFunction input, int initTarget){
  PID pidptr = initPid(kp, ki, kd, input(), initTarget);
  pidst* pid = (pidst*) pidptr;
  pid->input = input;
  calculatePid(pid);
  return pid;
}

void setInputFunction(PID pidptr, IntFunction input){
  pidst* pid = (pidst*) pidptr;
  pid->input = input;
}

void setGains(PID pidptr, double kp, double ki, double kd){
  pidst* pid = (pidst*) pidptr;
  pid->kp = kp;
  pid->ki = ki;
  pid->kd = kd;
}

void setMaxSpeed(PID pidptr, int val){
  pidst* pid = (pidst*) pidptr;
  val = rectify(val, 127);
  pid->maxSpeed = abs(val);
}

int getTarget(PID pidptr){
  pidst* pid = (pidst*) pidptr;
  return pid->target;
}

void setTarget(PID pidptr, int val){
  pidst* pid = (pidst*) pidptr;
  pid->target = val;
  lcdSetText(uart1, 1, "Target");
  lcdPrint(uart1, 2, "%d", val);
}

int calculatePid(PID pidptr, int process){
  pidst* pid = (pidst*) pidptr;

  int error = pid->target - process;
  double deltaTime = (millis() - pid->lastTime)/1000.0;
  unsigned int absError = abs(error);
  unsigned int initInteg = pid->integ;

  if(pid->lastInput == 100000) pid->lastInput = process; // uninitialized

  double deltaInput = 0;
  if(deltaTime > 0) deltaInput = (process - pid->lastInput) / deltaTime;

  if(absError <= pid->outerIntegLimit){
    if(absError >= pid->innerIntegLimit) pid->integ += (error * deltaTime) * pid->ki;
  }
  else pid->integ = 0;


  int pwr = (error * pid->kp) + (pid->integ) + (deltaInput * pid->kd);

  pwr = rectify(pwr, pid->maxSpeed);

  if((unsigned int)abs(pwr) >= pid->maxSpeed) pid->integ = initInteg;

  // lcdPrint(uart1, 1, "%d %d %d", int(error * pid->kp), int(pid->integ), int(deltaInput * pid->kd));
  // lcdPrint(uart1, 2, "Err %d Pwr %d", error, pwr);

  pid->lastTime = millis();
  pid->lastInput = process;

  return pwr;
}

int calculatePid(PID pidptr){
  pidst* pid = (pidst*) pidptr;
  int out = 0;
  if(pid->input != NULL) out = calculatePid(pidptr, pid->input());
  return out;
}
