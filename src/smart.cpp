#include "main.h"
#include <math.h>

TaskHandle drivingTask;

void drivePidArgs(double kp, double ki, double kd, int targetLeft, int targetRight, int slew);
void driveTurnPid(double kp, double ki, double kd, int target, int slew);

char drivePidStopOn = 's';

bool signChanged(unsigned int index, int val){
  return signChanged(index, (double)val);
}

bool signChanged(unsigned int index, double val){
  static bool wasNegative[2] = {false, false};
  bool isNegative = (val != abs(val));
  bool changed = (wasNegative[index] != isNegative);
  wasNegative[index] = isNegative;
  return changed;
}

bool isMoving(){
  static int counts = 0;
  long encL = encoderGet(leftEnc);
  long encR = encoderGet(rightEnc);
  static long prevEncL = 0;
  static long prevEncR = 0;
  if((abs(encL - prevEncL) < 5 || abs(encR - prevEncR) < 5)) counts++;
  else counts = 0;
  if(counts > 40){
    counts = 0;
    return false;
  }
  prevEncL = encL;
  prevEncR = encR;
  return true;
}

int feetToTicksLeft(double feet){
  const int ticksPerFoot = 290;
  return (feet * ticksPerFoot);
}

int degToTicksLeft(int degrees){
  const double footPerDeg = 0.02617994;
  double driveDistance = degrees * footPerDeg / 1.95;
  return -feetToTicksLeft(driveDistance);
}

int feetToTicksRight(double feet){
  const int ticksPerFoot = 280;
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
  // drivingTask = taskCreate(drivePid, TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT);
  driveTurnPid(5.0, 0.0, 20.0, degrees, 127);
}

void driveTurnPid(double kp, double ki, double kd, int target, int slew){
  bool done = false;
  double integ = 0;
  gyroReset(gyro);
  int error = target - gyroGet(gyro);
  int prevError = error;
  int stoppedCycles = 0;
  int capInteg = 127 / ki;
  while(!done && !isNewPress(btn8r)){

    error = target - gyroGet(gyro);

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
    driveSetRamp(-pwr, pwr, slew);
    prevError = error;
    if(abs(deltaError) < 1 && abs(error) < 5){
      stoppedCycles++;
    }
    else{
      stoppedCycles = 0;
    }
    if(stoppedCycles > 20){
      done = true;
      driveSet(0, 0);
    }
    lcdPrint(uart1, 1, "Tar %d Val %d", target, gyroGet(gyro));
    lcdPrint(uart1, 2, "Err %d Pwr %d", error, pwr);
    delay(20);
  }
}

void drivePidArgs(double kp, double ki, double kd, int targetLeft, int targetRight){
  drivePidArgs(kp, ki, kd, targetLeft, targetRight, 30);
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
  bool initErrIsNeg = targetLeft < 0;
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

    bool errIsNeg = (errorL < 0);
    if(abs(deltaErrorL) < 5 && abs(deltaErrorR) < 5){
      stoppedCycles++;
    }
    else{
      stoppedCycles = 0;
    }
    if(drivePidStopOn == 's' && stoppedCycles > 15){
      done = true;
      driveSet(0, 0);
    }
    else if(drivePidStopOn == 't' && (errIsNeg != initErrIsNeg)){
      done = true;
      driveSet(0, 0);
    }
    lcdPrint(uart1, 1, "L %d R %d", errorL, errorR);
    lcdPrint(uart1, 2, "L %d R %d", pwrL, pwrR);
    delay(20);
  }
  drivePidStopOn = 's';
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


void driveDist(double feet, char stopOnCode){
  int targetRight = feetToTicksRight(feet);
  int targetLeft = feetToTicksLeft(feet);
  drivePidStopOn = stopOnCode;
  drivePidArgs(0.3, 0.0, 1.0, targetLeft, targetRight);
}

void driveDist(double feet){
  driveDist(feet, 's');
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
  while(abs(encL) < abs(targetTicks) && counts < 40){
    encL = encoderGet(leftEnc);
    encR = encoderGet(rightEnc);
    driveSetRamp(leftPwr, rightPwr, slew);
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
  driveWaitRamp(feet, leftPwr, rightPwr, 30);
}

void smartGrabFront(int power){
  power = rectify(power);
  bool done = false;
  int timeOut = 0;
  while(!done && isMoving() && timeOut < 100){
    driveSet(power, power);
    if(isUnderBase(frontLight)) break;
    timeOut++;
    delay(20);
  }
  frontGrabSet(true);
  gateSet(false);
  driveSetImm(0, 0);
}

void smartGrabBack(int power){
  power = rectify(power);
  bool done = false;
  int timeOut = 0;
  while(!done && isMoving() && timeOut < 100){
    driveSet(power, power);
    if(isUnderBase(backLight)) break;
    timeOut++;
    delay(20);
  }
  frontGrabSet(true);
  gateSet(false);
  driveSetImm(0, 0);
}
