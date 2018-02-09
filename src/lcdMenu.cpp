#include "main.h"

int sysState = 1;
int autonState = 0;
int sensorIndex = 0;

void autonSelect();
void sensorView();
void lcdSet(const char *line1, const char *line2);
void lcdSet(int line, const char *string);

Semaphore btnSem = semaphoreCreate();

void msg(void * ignore){
  for(int i=0; i < 5; i++){
    lcdPrint(uart1, 2, "Count %d", i);
    delay(5000);
  }
}

void semMsg(void * ignore){
  for(int i=0; i < 5; i++){
    lcdPrint(uart1, 2, "Count %d", i);
    delay(50);
    semaphoreTake(btnSem, -1);
  }
}

void waitMsg(void * ignore){
  unsigned long now = 0;
  for(int i=0; i < 5; i++){
    lcdPrint(uart1, 2, "Count %d", i);
    waitUntil(&now, 5000);
  }
}

double ticksToFeetLeft(int ticks){
  // const double circumference = 12.566372;
  return (ticks / 290.0);
}

double ticksToFeetRight(int ticks){
  return (ticks / 280.0);
}

void lcdControl(){
  switch (sysState) {
    case 0: // Auton select
      autonSelect();
      break;
    case 1: // Sensor view
      sensorView();
      break;
  }
  if(isNewPress(lcdRight)) sensorIndex++;
  else if(isNewPress(lcdLeft)) sensorIndex--;
}

void autonSelect(){
  static double distance = 0;
  static bool msgTaskRunning = false;
  static TaskHandle msgTask = NULL;
  autonState = rectify(autonState, -1, 2);
  switch (autonState) {
    case -1:
      lcdSet("To Sensors", "");
      if(isNewPress(lcdMid)) sysState = 1;
      break;
    case 0: // Whole auton
      lcdSet(1, "Drive Distance");
      lcdPrint(uart1, 2, "%f", distance);
      if(isPressed(btn7r)) distance+=0.1;
      else if(isPressed(btn7l)) distance-=0.1;
      else if(isNewPress(btn7u)) distance++;
      else if(isNewPress(btn7d)) distance--;
      // if(isNewPress(lcdMid)) driveDist(distance);
      break;
    case 2:
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
    case 3:
      lcdSet(1, "");
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
    case 4:
      lcdSet(1, "Semaphore Step");
      if(!msgTaskRunning){
        lcdSet(2, "");
        if(isNewPress(lcdMid)){
          msgTask = taskCreate(semMsg, TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT);
          msgTaskRunning = true;
        }
      }
      else{
        if(isNewPress(lcdMid)){
          semaphoreGive(btnSem);
        }
        if(isNewPress(lcdRight)){
          taskDelete(msgTask);
          msgTaskRunning = false;
        }
      }
      break;
  }
  if(isNewPress(lcdMid)) sysState = 1;
}

void sensorView(){
  lcdPrint(uart1, 1, "Sensors %d", sensorIndex);
  sensorIndex = rectify(sensorIndex, -1, 4);
  static int accMax = 0;
  switch (sensorIndex) {
    case -1:
      lcdPrint(uart1, 2, "L %d R %d", joystickGetAnalog(1, 3), joystickGetAnalog(1, 2));
      break;
    case 0:
      lcdPrint(uart1, 2, "Front %d", analogRead(frontLight));
      if(isNewPress(lcdMid)) sysState = 0;
      break;
    case 1:
      lcdPrint(uart1, 2, "L %d R %d", encoderGet(leftEnc), encoderGet(rightEnc));
      if(isPressed(lcdMid)){
        encoderReset(leftEnc);
        encoderReset(rightEnc);
      }
      break;
    case 2:
      lcdPrint(uart1, 2, "T %d Ft %f", encoderGet(leftEnc), ticksToFeetLeft(encoderGet(leftEnc)));
      if(isPressed(lcdMid)){
        encoderReset(leftEnc);
        encoderReset(rightEnc);
      }
      break;
    case 3:
      lcdPrint(uart1, 2, "T %d Ft %f", encoderGet(rightEnc), ticksToFeetLeft(encoderGet(rightEnc)));
      if(isPressed(lcdMid)){
        encoderReset(leftEnc);
        encoderReset(rightEnc);
      }
      break;
    case 4:
      int acc = getAccel();
      lcdPrint(uart1, 2, "C %d M %d", acc, accMax);
      if(abs(acc) > abs(accMax)) accMax = acc;
      if(isPressed(lcdMid)) accMax = 0;
      break;
  }
}

void lcdSet(const char *line1, const char *line2){
  lcdSetText(uart1, 1, line1);
  lcdSetText(uart1, 2, line2);
}

void lcdSet(int line, const char *string){
  lcdSetText(uart1, line, string);
}
