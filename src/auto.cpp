/** @file auto.c
 * @brief File for autonomous code
 *
 * This file should contain the user autonomous() function and any functions related to it.
 *
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/
 *
 * PROS contains FreeRTOS (http://www.freertos.org) whose source code may be
 * obtained from http://sourceforge.net/projects/freertos/files/ or on request.
 */

#include "main.h"

// void firstBase();
// void cornerLaunch();
void firstTraverse();

void autonomous() {
  firstBase();
  cornerLaunch();
  firstCorner();
  firstTraverse();
  cornerLaunch();
  secondCorner();
  dejaVu();
  mwuah();
}

void firstBase(){
  frontGrabSet(false);
  gateSet(true);
  resetGyro();
  driveDist(4.0, 't');
  smartGrabFront(60);
  driveTurnDeg(-5-getGyro());
  resetGyro();
  while(isOverLine()){ //back off of tile
    driveSet(-127, -127);
    delay(20);
  }
  delay(100);
  driveToLine(-110);
  driveDist(-1.1, 's');
  driveTurnDeg(-90-getGyro());
  resetGyro();
  backGrabSet(false);
  driveDist(-1.75);
  delay(300);
  smartGrabBack(-60);
  // driveWait(-0.06, -60);
  driveTurnDeg(-64-getGyro(), 120, 120);
}

void cornerLaunch(){
  driveWait(1.5, 127);
  bool done = false;
  while(!done){
    driveSet(127, 127);
    if(isOverLine()) {
      encoderReset(leftEnc);
      while(encoderGet(leftEnc) < launchDelay){
        delay(10);
      }
      punchSet(true);
      done = true;
    }
    delay(10);
  }
  driveStop();
  delay(200);
  punchSet(false);
  frontGrabSet(false);
  driveWait(-0.3, -90);
  backGrabSet(false);
  driveWait(-0.25, -120);
  smartGrabFront(-60);
  digitalWrite(claw, true);
  delay(200);
  driveToLine(100);
  driveWait(0.35, 100);
  punchSet(true);
  // driveTime(200, -60);
  lineUp(-1.4);
  punchSet(false);
  frontGrabSet(false);
  // lineUp(-1.4);
}

void firstCorner(){
  unsigned int wallDist = getProx();
  while(abs(wallDist) > 100){
    wallDist = getProx();
    delay(20);
  }
  const double fullHypo = 66;
  double curHypo = sqrt(2 * (wallDist * wallDist));
  double dist = (fullHypo - curHypo) / 12.0;
  frontGrabSet(false);
  backGrabSet(false);
  gateSet(false);
  driveTurnDeg(90);
  resetGyro();
  driveDist(-dist, 's');
  driveTurnDeg(-44-getGyro());
  resetGyro();
  driveDist(-3.5);
  smartGrabFront(-60);
  driveTurnDeg(-getGyro());
  resetGyro();
  for(int counts=0; counts < 5;){
    if(isOverLine(lineRight)) counts++;
    else counts = 0;
    driveSet(127, 127);
    delay(20);
  }
  for(int counts=0; counts < 5;){
    if(!isOverLine()) counts++;
    else counts = 0;
    driveSet(127, 127);
    delay(20);
  }
  driveToLine(127);
  driveWait(1, 127);
  driveSetImm(70, 70);
  driveDist(1, 10, 's');
  driveTurnDeg(-45-getGyro(), 127, 30);
  driveWait(0.2, 127);
  punchSet(true);
  driveWait(-0.7, -100);
  punchSet(false);
  frontGrabSet(false);
}

void firstTraverse(){
  resetGyro();
  driveWait(-1.25, -127);
  while(!isOverBase(frontLight)){
    driveSet(-60, -60);
  }
  driveWait(-0.5, -60, -60);
  gateSet(true);
  delay(20);
  smartGrabFront(70);
  driveStop();
  delay(200);
  driveTurnDeg(-getGyro());
  resetGyro();
  driveWait(-0.4, -110, -110);
  driveSetImm(-30, -30);
  smartGrabBack(-30);
  driveStop();
  delay(200);
  driveTurnDeg(-10-getGyro());
  resetGyro();
  driveWait(-0.25, -110);
  driveDist(-0.5, 's');
  driveTurnDeg(165-getGyro(), 110, 105);
}

void secondCorner(){
  unsigned int wallDist = getProx();
  while(abs(wallDist) > 100){
    wallDist = getProx();
    delay(20);
  }
  const double offset = 10;
  double curHypo = sqrt(2 * (wallDist * wallDist));
  double dist = (curHypo - offset) / 12.0;
  frontGrabSet(false);
  backGrabSet(false);
  gateSet(false);
  driveTurnDeg(-90);
  resetGyro();
  driveDist(-dist, 's');
  driveTurnDeg(45-getGyro());
  resetGyro();
  driveDist(-4.3);
  driveTurnDeg(-getGyro());
  resetGyro();
  smartGrabFront(-60);
  driveTurnDeg(-getGyro());
  resetGyro();
  for(int counts=0; counts < 4;){
    if(isOverLine(lineLeft)) counts++;
    else counts = 0;
    driveSet(127, 127);
    delay(20);
  }
  for(int counts=0; counts < 4;){
    if(!isOverLine()) counts++;
    else counts = 0;
    driveSet(127, 127);
    delay(20);
  }
  driveToLine(127);
  driveWait(1.25, 127);
  driveSetImm(70, 70);
  driveDist(1, 10, 's');
  driveTurnDeg(45-getGyro(), 30, 127);
  driveWait(0.3, 90);
  punchSet(true);
  // driveTime(400, -60);
  driveWait(-0.4, -100);
  lineUp(-1.4);
  punchSet(false);
  frontGrabSet(false);
}

void dejaVu(){
  resetGyro();
  driveDist(-4.6, 's');
  driveTurnDeg(120-getGyro());
  resetGyro();
  driveWait(-0.7, -110);
  driveSetImm(-40, -40);
  smartGrabFront(-30);
  delay(200);
  if(getGyro() < -10){
    driveTurnDeg(-getGyro());
    resetGyro();
  }
  driveDist(-2.5, 15, 's');
  driveTurnDeg(-72-getGyro());
  resetGyro();
  driveWait(-1.4, -90);
  smartGrabBack(-60);
  // driveTurnDeg(-9-getGyro(), 120, 30);
  driveTurnDeg(-12-getGyro());
  resetGyro();
  for(int counts=0; counts < 4;){
    if(isOverLine()) counts++;
    else counts = 0;
    driveSet(110, 110);
    delay(10);
  }
  for(int counts=0; counts < 4;){
    if(!isOverLine()) counts++;
    else counts = 0;
    driveSet(110, 110);
    delay(10);
  }
  driveToLine(110);
  driveWait(1, 110);
  // driveSetImm(80, 80);
  driveDist(1.2, 10, 's');
  driveTurnDeg(-46-getGyro(), 127, 10);
  driveWait(0.5, 127);
  punchSet(true);
  driveTime(200, -100);
  frontGrabSet(false);
  punchSet(false);
  backGrabSet(false);
  smartGrabFront(-60);
  driveWait(-0.5, -90);
  driveTurnDeg(50, 10, 127, 't');
  driveWait(0.2, 90);
  driveTurnDeg(-50, 127, 10, 't');
  driveWait(-0.25, -110);
  driveWait(0.3, 110);
  punchSet(true);
  driveWait(-0.25, -110);
  punchSet(false);
  frontGrabSet(false);
  driveStop();
}

void mwuah(){
  driveTurnDeg(45);
  driveWait(-6, -127);
  driveStop();
}

void backOverFront(){
  backGrabSet(false);
  frontGrabSet(false);

}

//TODO: shorten waits after stop pids
//      increase kd for turn pid
//      make driveTurnDeg auto adjust and reset gyro
//      remove stops from transitions
