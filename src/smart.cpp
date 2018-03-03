#include "main.h"

TaskHandle drivingTask;

void drivePidArgs(double kp, double ki, double kd, int targetLeft, int targetRight, int slew);
void driveTurnPid(double kp, double ki, double kd, int target, int maxLeft, int maxRight);
bool isMoving();
bool isMoving(int milWait);
void rightHoldTask(void * args);
void leftHoldTask(void * args);

char drivePidStopOn = 's';
bool killDrivePid = false;
bool isRunningDriveTask = false;

bool isMoving(){
  return isMoving(2000);
}

bool isMoving(int milWait){
  long encL = encoderGet(leftEnc);
  long encR = encoderGet(rightEnc);
  static long prevEncL = 0;
  static long prevEncR = 0;
  long now = millis();
  static long startTime = -1;
  static long lastSampleTime = -1;
  long timeElapsed = -1;
  const int newCallUpper = 50;
  const int newCallLower = 15;

  if(lastSampleTime == -1) lastSampleTime = now;

  bool isNewLoop = (((now - lastSampleTime) > newCallUpper) || ((now - lastSampleTime) < newCallLower));

  if(startTime == -1 || isNewLoop) startTime = now;

  if((abs(encL - prevEncL) < 2 || abs(encR - prevEncR) < 2)) timeElapsed = now - startTime;

  if(timeElapsed > milWait){
    startTime = -1;
    return false;
  }

  prevEncL = encL;
  prevEncR = encR;
  return true;
}

int feetToTicksLeft(double feet){
  const int ticksPerFoot = 310;
  return (feet * ticksPerFoot);
}

int degToTicksLeft(int degrees){
  const double footPerDeg = 0.02617994;
  double driveDistance = degrees * footPerDeg / 1.95;
  return -feetToTicksLeft(driveDistance);
}

int feetToTicksRight(double feet){
  const int ticksPerFoot = 310;
  return (feet * ticksPerFoot);
}

int degToTicksRight(int degrees){
  const double footPerDeg = 0.02617994;
  double driveDistance = degrees * footPerDeg / 1.95;
  return feetToTicksRight(driveDistance);
}

int driveTargetRight = 0;
int driveTargetLeft = 0;

void driveTurnDeg(int degrees){
  driveTurnPid(5.5, 0.0, 20.0, degrees, 127, 127);
}

void driveTurnDeg(int degrees, int maxLeft, int maxRight){
  maxLeft = abs(maxLeft);
  maxRight = abs(maxRight);
  driveTurnPid(5.5, 0.0, 20.0, degrees, maxLeft, maxRight);
}

void driveTurnPid(double kp, double ki, double kd, int target, int maxLeft, int maxRight){
  bool done = false;
  double integ = 0;
  resetGyro();
  int error = target - getGyro();
  int prevError = error;
  int stoppedCycles = 0;
  int capInteg = 127 / ki;
  while(!done && !isNewPress(btn8r)){

    error = target - getGyro();

    int deltaError = (error - prevError);

    if(abs(error) < 30){
      integ += error;
      integ = rectify(integ, -capInteg, capInteg);
    }
    else{
      integ = 0;
    }

    int pwr = (error * kp) + (integ * ki) + (deltaError * kd);

    pwr = rectify(pwr, -100, 100);

    int pwrL = rectify(pwr, -maxLeft, maxLeft);
    int pwrR = rectify(pwr, -maxRight, maxRight);

    driveSetRamp(-pwrL, pwrR, 127);
    prevError = error;
    if(abs(deltaError) < 2){
      stoppedCycles++;
    }
    else{
      stoppedCycles = 0;
    }
    if(abs(error) < 5){
      if(stoppedCycles > 7){
        done = true;
      }
    }
    else if(stoppedCycles > 20){ // give up for burnout
      done = true;
    }
    lcdPrint(uart1, 1, "Tar %d Val %d", target, getGyro());
    lcdPrint(uart1, 2, "Err %d Pwr %d", error, pwr);
    delay(20);
  }
}

void drivePidArgs(double kp, double ki, double kd, int targetLeft, int targetRight){
  drivePidArgs(kp, ki, kd, targetLeft, targetRight, DEFAULT_SLEW);
}

void drivePidArgs(double kp, double ki, double kd, int targetLeft, int targetRight, int slew){
  bool done = false;
  encoderReset(rightEnc);
  encoderReset(leftEnc);
  double integL = 0;
  double integR = 0;
  int encL = 0;
  int encR = 0;
  int errorL = targetLeft - encL;
  int errorR = targetRight - encR;
  int prevErrorL = errorL;
  int prevErrorR = errorR;
  int stoppedCycles = 0;
  int capInteg = 127 / ki;
  bool initErrLIsNeg = targetLeft < 0;
  bool initErrRIsNeg = targetRight < 0;
  while(!done && !isNewPress(btn8r)){
    encL = encoderGet(leftEnc);
    encR = encoderGet(rightEnc);

    errorL = targetLeft - encL;
    errorR = targetRight - encR;

    int deltaErrorL = (errorL - prevErrorL);
    int deltaErrorR = (errorR - prevErrorR);

    if(abs(errorL) < 150){
      integL += errorL;
      integL = rectify(integL, -capInteg, capInteg);
    }
    else{
      integL = 0;
    }

    if(abs(errorR) < 150){
      integR += errorR;
      integR = rectify(integR, -capInteg, capInteg);
    }
    else{
      integR = 0;
    }

    int pwrL = (errorL * kp) + (integL * ki) + (deltaErrorL * kd);
    int pwrR = (errorR * kp) + (integR * ki) + (deltaErrorR * kd);

    pwrL = rectify(pwrL);
    pwrR = rectify(pwrR);
    driveSetRamp(pwrL, pwrR, slew);
    prevErrorL = errorL;
    prevErrorR = errorR;

    bool errLIsNeg = (errorL < 0);
    bool errRIsNeg = (errorR < 0);

    if(abs(deltaErrorL) < 4 && abs(deltaErrorR) < 4){
      stoppedCycles++;
    }
    else{
      stoppedCycles = 0;
    }
    if(drivePidStopOn == 's'){ // Robot Stopped
      if(stoppedCycles > 7){
        done = true;
        driveStop();
      }
    }
    else if(drivePidStopOn == 't'){ // Target reached
      if(errLIsNeg != initErrLIsNeg || errRIsNeg != initErrRIsNeg || stoppedCycles > 20){
        done = true;
        driveStop();
      }
    }
    else if(drivePidStopOn == 'l'){
      if(isOverLine()){
        done = true;
        driveStop();
      }
    }
    else if(drivePidStopOn == 'g'){ // Global changed
      if(killDrivePid == true){
        done = true;
        killDrivePid = false;
        driveStop();
      }
    }
    else{
      drivePidStopOn = 't';
    }
    lcdPrint(uart1, 1, "L %d R %d", errorL, errorR);
    lcdPrint(uart1, 2, "Stop %c", drivePidStopOn);
    delay(20);
  }
  // drivePidStopOn = 's';
}

// void drivePid(void * ignore){
//   bool done = false;
//   encoderReset(rightEnc);
//   encoderReset(leftEnc);
//   double integL = 0;
//   double integR = 0;
//   int encL = encoderGet(leftEnc);
//   int encR = encoderGet(rightEnc);
//   int errorL = driveTargetLeft - encL;
//   int errorR = driveTargetRight - encR;
//   int prevErrorL = errorL;
//   int prevErrorR = errorR;
//   while(!done && ! isNewPress(btn8l)){
//     const double kp = 0.3;
//     const double ki = 0.0;
//     const double kd = 0.9;
//
//     encL = encoderGet(leftEnc);
//     encR = encoderGet(rightEnc);
//
//     errorL = driveTargetLeft - encL;
//     errorR = driveTargetRight - encR;
//
//     int deltaErrorL = (errorL - prevErrorL);
//     int deltaErrorR = (errorR - prevErrorR);
//
//     if(abs(errorL) < 400){
//       integL += errorL;
//       rectify(integL);
//     }
//     else{
//       integL = 0;
//     }
//
//     if(abs(errorR) < 400){
//       integR += errorR;
//       rectify(integR);
//     }
//     else{
//       integR = 0;
//     }
//
//     int pwrL = (errorL * kp) + (integL * ki) + (deltaErrorL * kd);
//     int pwrR = (errorR * kp) + (integR * ki) + (deltaErrorR * kd);
//
//     pwrL = rectify(pwrL);
//     pwrR = rectify(pwrR);
//     driveSet(pwrL, pwrR);
//     prevErrorL = errorL;
//     prevErrorR = errorR;
//     if(abs(deltaErrorL) == 0 && abs(errorL) < 20 && abs(deltaErrorR) == 0){
//       done = true;
//       driveSet(0, 0);
//     }
//     delay(20);
//   }
// }

void asyncDriveDist(void * feetPtr){
  double feet = *(double *)feetPtr;
  isRunningDriveTask = false;
  driveDist(feet, 't');
  isRunningDriveTask = true;
}

void driveDist(double feet, int slew, char stopOnCode){
  int targetRight = feetToTicksRight(feet);
  int targetLeft = feetToTicksLeft(feet);
  drivePidStopOn = stopOnCode;
  if(feet > 0) driveSetImm(10, 10);
  else driveSetImm(-10, -10);
  delay(80);
  drivePidArgs(0.3, 0.0, 1.2, targetLeft, targetRight, slew);
}

void driveDist(double feet, char stopOnCode){
  int targetRight = feetToTicksRight(feet);
  int targetLeft = feetToTicksLeft(feet);
  drivePidStopOn = stopOnCode;
  if(feet > 0) driveSetImm(10, 10);
  else driveSetImm(-10, -10);
  delay(80);
  drivePidArgs(0.3, 0.0, 1.2, targetLeft, targetRight);
}

void driveDist(double feet){
  driveDist(feet, 't');
}

void driveWaitRamp(double feet, int leftPwr, int rightPwr, int slew){
  encoderReset(rightEnc);
  encoderReset(leftEnc);
  int targetTicks = feetToTicksLeft(feet);
  int prevEncL = encoderGet(leftEnc);
  int encL = encoderGet(leftEnc);
  int prevEncR = encoderGet(rightEnc);
  int encR = encoderGet(rightEnc);
  int counts = 0;
  while(abs(encL) < abs(targetTicks) && counts < 15){
    encL = encoderGet(leftEnc);
    encR = encoderGet(rightEnc);
    driveSetRamp(leftPwr, rightPwr, slew);
    lcdPrint(uart1, 2, "T %d C %d", encL, counts);
    lcdPrint(uart1, 2, "D %d M %d", (encL-prevEncL), motorGet(dr1));
    if((abs(encL - prevEncL) < 5 || abs(encR - prevEncR) < 5)) counts++;
    else counts = 0;
    prevEncL = encL;
    prevEncR = encR;
    delay(20);
  }
}

void driveWaitTicks(int ticks, int speed){
  encoderReset(rightEnc);
  encoderReset(leftEnc);
  int prevEncL = encoderGet(leftEnc);
  int encL = encoderGet(leftEnc);
  int prevEncR = encoderGet(rightEnc);
  int encR = encoderGet(rightEnc);
  int counts = 0;
  while(abs(encL) < abs(ticks) && counts < 15){
    encL = encoderGet(leftEnc);
    encR = encoderGet(rightEnc);
    driveSetRamp(speed, speed, DEFAULT_SLEW);
    lcdPrint(uart1, 1, "T %d C %d", encL, counts);
    lcdPrint(uart1, 2, "D %d M %d", (encL-prevEncL), motorGet(dr1));
    if((abs(encL - prevEncL) < 5 || abs(encR - prevEncR) < 5)) counts++;
    else counts = 0;
    prevEncL = encL;
    prevEncR = encR;
    delay(20);
  }
}

void driveWait(double feet, int leftPwr, int rightPwr){
  driveWaitRamp(feet, leftPwr, rightPwr, DEFAULT_SLEW);
}

void driveWait(double feet, int power){
  driveWait(feet, power, power);
}

void driveWaitRamp(double feet, int power, int slew){
  driveWaitRamp(feet, power, power, slew);
}

void driveTime(unsigned long waitTime, int power){
  unsigned long startTime = millis();
  while((millis() - startTime) <= waitTime){
    driveSet(power, power);
  }
  driveStop();
}

void smartGrabFront(int power){
  power = rectify(power);
  bool done = false;
  int timeOut = 0;
  while(!done && isMoving() && timeOut < 100){
    driveSet(power, power);
    if(isOverBase(frontLight)) {
      encoderReset(leftEnc);
      encoderReset(rightEnc);
      while(!done){
        driveSet(power, power);
        int enc = encoderGet(leftEnc);
        if(gateGet() && (enc > 90 || enc < -2)) done = true;
        else if(!gateGet() && (enc > 80 || enc < -2)) done = true;
        delay(20);
      }
    }
    timeOut++;
    delay(20);
  }
  gateSet(false);
  frontGrabSet(true);
  driveStop();
}

void smartGrabBack(int power){
  power = rectify(power);
  bool done = false;
  int timeOut = 0;
  while(!done && isMoving() && timeOut < 100){
    driveSet(power, power);
    if(isOverBase(backLight)) {
      encoderReset(leftEnc);
      encoderReset(rightEnc);
      while(!done){
        driveSet(power, power);
        if(power >= 0 && encoderGet(leftEnc) > (30)) done = true;
        else if(power < 0 && encoderGet(leftEnc) < (-70 - power)) done = true;
        delay(20);
      }
    }
    timeOut++;
    delay(20);
  }
  backGrabSet(true);
  driveStop();
}


void stopOnLine(double feet){
  stopOnLine(feet, DEFAULT_SLEW);
}

void stopOnLine(double feet, int slew){
  int timeout = 0;
  void* feetPtr = &feet;
  TaskHandle driveTask = taskCreate(asyncDriveDist, TASK_DEFAULT_STACK_SIZE, feetPtr, TASK_PRIORITY_DEFAULT);
  delay(100);
  while(!isOverLine() && isRunningDriveTask && timeout < 250){
    timeout++;
    delay(20);
  }
  if(isRunningDriveTask){
    taskDelete(driveTask);
    isRunningDriveTask = false;
  }
  else{
    while(!isOverLine()){
      driveSet(-60, -60);
      delay(20);
    }
  }
  driveDist(0, 's');
  driveStop();
}

void stopOnLineBlind(int speed){
  stopOnLine(speed, DEFAULT_SLEW);
}

void stopOnLineBlind(int speed, int slew){
  int timeout = 0;
  while(!isOverLine() && timeout < 200 && isMoving()){
    driveSetRamp(speed, speed, slew);
    timeout++;
    delay(20);
  }
  driveDist(0, 's');
  driveStop();
}

void lineUp(double feet){
  void* feetPtr = &feet;
  TaskHandle driveTask = taskCreate(asyncDriveDist, TASK_DEFAULT_STACK_SIZE, feetPtr, TASK_PRIORITY_DEFAULT);
  while(isOverLine() && isRunningDriveTask){
    delay(20);
  }
  if(isRunningDriveTask){
    taskDelete(driveTask);
    isRunningDriveTask = false;
    int timeOut = 0;
    driveSet(-60, -60);
    while(!isOverLine() && timeOut < 150){
      timeOut++;
      delay(20);
    }
  }
  if(isOverLine()){
    if(isOverLine(lineLeft) && !isOverLine(lineRight)){
      while (!isOverLine(lineRight)) {
        if(feet < 0) driveSet(0, -40);
      }
    }
    else if(!isOverLine(lineLeft) && isOverLine(lineRight)){
    }
  }
}

void driveToLine(int speed){
  int timeout = 0;
  while(!isOverLine() && timeout < 250){
    driveSet(speed, speed);
    timeout++;
    delay(10);
  }
}

bool rightHoldDone = false;

TaskHandle driveHoldRight(){
  rightHoldDone = false;
  double gainsArr[2] = {10.0, 20.0};
  TaskHandle task = taskCreate(rightHoldTask, TASK_DEFAULT_STACK_SIZE, gainsArr, TASK_PRIORITY_DEFAULT);
  return task;
}

void rightHoldTask(void* argsPtr){
  double* gains = (double *)argsPtr;
  double kp = gains[0];
  double kd = gains[1];
  encoderReset(rightEnc);
  int error = 0;
  int prevError = error;
  int stoppedCycles = 0;
  while(true){

    error = -encoderGet(rightEnc);

    int deltaError = (error - prevError);

    int pwr = (error * kp) + (deltaError * kd);

    pwr = rectify(pwr);

    driveSetRight(pwr);

    if(abs(deltaError) < 2){
      stoppedCycles++;
    }
    else{
      stoppedCycles = 0;
    }
    if(abs(error) < 5){
      if(stoppedCycles > 7){
        rightHoldDone = true;
      }
    }
    else if(stoppedCycles > 100){
      rightHoldDone = true;
    }

    prevError = error;

    delay(20);
  }
}

bool leftHoldDone = false;

TaskHandle driveHoldLeft(){
  leftHoldDone = false;
  double gainsArr[2] = {10.0, 20.0};
  TaskHandle task = taskCreate(leftHoldTask, TASK_DEFAULT_STACK_SIZE, gainsArr, TASK_PRIORITY_DEFAULT);
  return task;
}

void leftHoldTask(void* argsPtr){
  double* gains = (double *)argsPtr;
  double kp = gains[0];
  double kd = gains[1];
  encoderReset(leftEnc);
  int error = 0;
  int prevError = error;
  int stoppedCycles = 0;
  while(true){

    error = -encoderGet(leftEnc);

    int deltaError = (error - prevError);

    int pwr = (error * kp) + (deltaError * kd);

    pwr = rectify(pwr);

    driveSetLeft(pwr);

    if(abs(deltaError) < 2){
      stoppedCycles++;
    }
    else{
      stoppedCycles = 0;
    }
    if(abs(error) < 4){
      if(stoppedCycles > 7){
        leftHoldDone = true;
      }
    }
    else if(stoppedCycles > 100){
      leftHoldDone = true;
    }

    prevError = error;

    delay(20);
  }
}

void lineUp(int speed){
  while(!isOverLine()){
    driveSet(speed, speed);
    delay(20);
  }
  TaskHandle leftTask;
  TaskHandle rightTask;
  if(isOverLine(lineLeft)){
    leftTask = driveHoldLeft();
    while(!isOverLine(lineRight)){
      delay(20);
    }
    rightTask = driveHoldRight();
  }
  else {
    rightTask = driveHoldRight();
    while(!isOverLine(lineLeft)){
      delay(20);
    }
    leftTask = driveHoldLeft();
  }
  while(!leftHoldDone || !rightHoldDone){
    delay(40);
  }
  taskDelete(leftTask);
  taskDelete(rightTask);
  driveStop();
}
//
// void driveStraight(double feet, int speed){
//   driveStraight(feet, speed, DEFAULT_SLEW);
// }
//
// void driveStraight(double feet, int speed, int slew){
//   encoderReset(leftEnc);
//   encoderReset(rightEnc);
//   resetGyro();
//   int target = feetToTicksLeft(feet);
//   bool targetAhead = encoderGet(leftEnc) < target;
//   bool prevTargetAhead = targetAhead;
//   bool targetPassed = false;
//   int prevError = error;
//   // Controller pid = initController(4.0, 0.0, 9.0);
//
//   while(targetPassed){
//     targetAhead = encoderGet(leftEnc) < target;
//     targetPassed = targetAhead != prevTargetAhead;
//
//     int error = getGyro();
//
//     if(adjustment < 0) driveSet(speed, speed+adjustment);
//     else driveSet(speed-adjustment, speed);
//
//     prevTargetAhead = targetAhead;
//     delay(20);
//   }
//
//   bool done = false;
//   double integ = 0;
//   resetGyro();
//   int error = target - getGyro();
//   int stoppedCycles = 0;
//   int capInteg = 127 / ki;
//   while(!done && !isNewPress(btn8r)){
//
//     error = target - getGyro();
//
//     int deltaError = (error - prevError);
//
//     if(abs(error) < 30){
//       integ += error;
//       integ = rectify(integ, -capInteg, capInteg);
//     }
//     else{
//       integ = 0;
//     }
//
//     int pwr = (error * kp) + (integ * ki) + (deltaError * kd);
//
//     pwr = rectify(pwr, -100, 100);
//
//     int pwrL = rectify(pwr, -maxLeft, maxLeft);
//     int pwrR = rectify(pwr, -maxRight, maxRight);
//
//     driveSetRamp(-pwrL, pwrR, 127);
//     prevError = error;
//     if(abs(deltaError) < 2){
//       stoppedCycles++;
//     }
//     else{
//       stoppedCycles = 0;
//     }
//     if(abs(error) < 5){
//       if(stoppedCycles > 7){
//         done = true;
//       }
//     }
//     else if(stoppedCycles > 20){ // give up for burnout
//       done = true;
//     }
//     lcdPrint(uart1, 1, "Tar %d Val %d", target, getGyro());
//     lcdPrint(uart1, 2, "Err %d Pwr %d", error, pwr);
//     delay(20);
//   }
// }
