#include "main.h"

int sysState = 1;
int autonState = 0;
int sensorIndex = 0;

void autonSelect();
void sensorView();
void lcdSet(const char *line1, const char *line2);
void lcdSet(int line, const char *string);


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
      if(isNewPress(lcdMid)) driveDist(distance);
      break;
    case 2:
      lcdSet("Task Test", "Msg 2 sec");
      break;

  }
  if(isNewPress(lcdMid)) sysState = 1;
}

void sensorView(){
  lcdPrint(uart1, 1, "Sensors %d", sensorIndex);
  sensorIndex = rectify(sensorIndex, -1, 3);
  switch (sensorIndex) {
    case -1:
      lcdPrint(uart1, 2, "L %d R %d", joystickGetAnalog(1, 3), joystickGetAnalog(1, 2));
      break;
    case 0:
      lcdPrint(uart1, 2, "Front %d", analogRead(frontLight));
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
  }
  if(isNewPress(lcdMid)) sysState = 0;
}

void lcdSet(const char *line1, const char *line2){
  lcdSetText(uart1, 1, line1);
  lcdSetText(uart1, 2, line2);
}

void lcdSet(int line, const char *string){
  lcdSetText(uart1, line, string);
}
