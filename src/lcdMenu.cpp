#include "main.h"

int sysState = 0;
int autonState = 0;
int sensorIndex = 0;

void autonSelect();
void sensorView();
void pidTune();
void lcdSet(const char *line1, const char *line2);
void lcdSet(int line, const char *string);

Semaphore btnSem = semaphoreCreate();
Mutex runMsg = mutexCreate();

void msg(void * ignore){
  for(int i=0; i < 5; i++){
    lcdPrint(uart1, 2, "Count %d", i);
    delay(1000);
  }
}

void semMsg(void * ignore){
  for(int i=0; i < 5; i++){
    lcdPrint(uart1, 2, "Count %d", i);
    delay(1000);
    semaphoreTake(btnSem, -1);
  }
}

void waitMsg(void * ignore){
  unsigned long now = millis();
  for(int i=0; i < 5; i++){
    lcdPrint(uart1, 2, "Count %d", i);
    waitUntil(&now, 1000);
  }
}

void msgPause(void * ignore){
  for(int i=0; i < 5; i++){
    mutexTake(runMsg, -1);
    lcdPrint(uart1, 2, "Count %d", i);
    mutexGive(runMsg);
    delay(1000);
  }
}

double ticksToFeetLeft(int ticks){
  // const double circumference = 12.566372;
  return (ticks / 290.0);
}

double ticksToFeetRight(int ticks){
  return (ticks / 280.0);
}

void lcdControl(void * ignore){
  // while(true){
    switch (sysState) {
      case 0: // Auton select
        autonSelect();
        break;
      case 1: // Sensor view
        sensorView();
        break;
      case 2:
        pidTune();
        break;
    }
    if(isNewPress(lcdRight)) {
      sensorIndex++;
      autonState++;
    }
    else if(isNewPress(lcdLeft)) {
      sensorIndex--;
      autonState--;
    }
  //   delay(20);
  // }
}

void pidTune(){
  static double kp = 0;
  static double ki = 0;
  static double kd = 0;
  static double target = 0;
  lcdPrint(uart1, 1, "P %f I %f", kp, ki);
  lcdPrint(uart1, 2, "D %f T %f", kd, target);
  if(isNewPress(btn8u)) kp+=0.05;
  else if(isNewPress(btn8d)) kp-=0.05;
  if(isNewPress(btn6u)) ki+=0.001;
  else if(isNewPress(btn6d)) ki-=0.001;
  if(isNewPress(btn5u)) kd+=0.05;
  else if(isNewPress(btn5d)) kd-=0.05;
  if(isPressed(btn7u)) target+=0.01;
  else if(isPressed(btn7d)) target-=0.01;
  // if(isNewPress(btn8r)) drivePidArgs(kp, ki, kd, target);
}

void autonSelect(){
  static bool msgTaskRunning = false;
  static bool autonBtnsEnabled = true;
  static TaskHandle msgTask = NULL;
  switch (autonState) {
    case -1:
      lcdSet("To Sensors", "");
      if(isNewPress(lcdMid)) sysState = 1;
      break;
    case 0:
      lcdSet(1, "Run Auton");
      if(autonBtnsEnabled) {
        lcdSet(2, "Buttons Enabled");
        if(isPressed(btn7l)){
          firstBase();
        }
        else if(isPressed(btn7r)){
          launchFirstBase();
        }
        else if(isPressed(btn7d)){
          firstBase();
          launchFirstBase();
        }
      }
      else lcdSet(2, "Buttons Disabled");
      // if(isPressed(btn5d) && isPressed(btn6d)){
      //   autonBtnsEnabled = !autonBtnsEnabled;
      //   delay(500);
      // }
      break;
    case 1:
      lcdSet(1, "Count 5s");
      if(!msgTaskRunning){
        lcdSet(2, "");
        if(isNewPress(lcdMid)){
          msgTask = taskCreate(msg, TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT);
          msgTaskRunning = true;
        }
      }
      else{
        if(isNewPress(lcdMid)){
          if(msgTask != NULL) taskDelete(msgTask);
          msgTaskRunning = false;
        }
      }
      break;
    case 2:
      lcdSet(1, "Wait 5s");
      if(!msgTaskRunning){
        lcdSet(2, "");
        if(isNewPress(lcdMid)){
          msgTask = taskCreate(waitMsg, TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT);
          msgTaskRunning = true;
        }
      }
      else{
        if(isNewPress(lcdMid)){
          if(msgTask != NULL) taskDelete(msgTask);
          msgTaskRunning = false;
        }
      }
      break;
    default:
      if(autonState > 0) autonState--;
      else if(autonState < 0) autonState++;
  }
  if(isNewPress(lcdMid)) sysState = 1;
}

void sensorView(){
  lcdPrint(uart1, 1, "Sensors %d", sensorIndex);
  static int accMax = 0;
  switch (sensorIndex) {
    case -3:
      lcdPrint(uart1, 2, "L %d R %d", joystickGetAnalog(1, 3), joystickGetAnalog(1, 2));
      break;
    case -2:
      lcdPrint(uart1, 2, "Front %d", analogRead(frontLight));
      if(isNewPress(lcdMid)) sysState = 0;
      break;
    case -1:
      lcdPrint(uart1, 2, "Back %d", analogRead(backLight));
      if(isNewPress(lcdMid)) sysState = 0;
      break;
    case 0:
      lcdPrint(uart1, 1, "Line Readers");
      lcdPrint(uart1, 2, "L %d R %d", analogRead(lineLeft), analogRead(lineRight));
      break;
    case 1:
      lcdPrint(uart1, 2, "Gyro %d", gyroGet(gyro));
      if(isPressed(lcdMid)) gyroReset(gyro);
      if(isNewPress(btn7l)){
        gyroShutdown(gyro);
        delay(100);
        gyro = gyroInit(gyroPort, 0);
      }
      if(isPressed(btn8l)) driveTurnDeg(45);
      if(isPressed(btn8r)) driveTurnDeg(90);
      if(isPressed(btn7r)) driveTurnDeg(180);
      break;
    case 2:
      lcdPrint(uart1, 2, "L %d R %d", encoderGet(leftEnc), encoderGet(rightEnc));
      if(isPressed(lcdMid)){
        encoderReset(leftEnc);
        encoderReset(rightEnc);
      }
      break;
    case 3:
      lcdPrint(uart1, 2, "T %d Ft %f", encoderGet(leftEnc), ticksToFeetLeft(encoderGet(leftEnc)));
      if(isPressed(lcdMid)){
        encoderReset(leftEnc);
        encoderReset(rightEnc);
      }
      break;
    case 4:
      lcdPrint(uart1, 2, "T %d Ft %f", encoderGet(rightEnc), ticksToFeetLeft(encoderGet(rightEnc)));
      if(isPressed(lcdMid)){
        encoderReset(leftEnc);
        encoderReset(rightEnc);
      }
      break;
    case 5:
      int acc;
      acc = getAccel();
      lcdPrint(uart1, 2, "C %d M %d", acc, accMax);
      if(abs(acc) > abs(accMax)) accMax = acc;
      if(isPressed(lcdMid)) accMax = 0;
      break;

    default:
      if(sensorIndex > 0) sensorIndex--;
      else if(sensorIndex < 0) sensorIndex++;
  }
}

void lcdSet(const char *line1, const char *line2){
  lcdSetText(uart1, 1, line1);
  lcdSetText(uart1, 2, line2);
}

void lcdSet(int line, const char *string){
  lcdSetText(uart1, line, string);
}
