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
  firstTraverse();
  cornerLaunch();
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
  driveDist(-1.4, 's');
  driveTurnDeg(-90-getGyro());
  resetGyro();
  backGrabSet(false);
  driveDist(-1.75);
  delay(300);
  smartGrabBack(-60);
  delay(200);
  driveTurnDeg(-63-getGyro());
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
  driveTime(400, -60);
  punchSet(false);
  backGrabSet(false);
  frontGrabSet(false);
  driveWait(-0.15, -100);
  driveStop();
  delay(200);
  smartGrabFront(-60);
  delay(400);
  digitalWrite(claw, true);
  driveToLine(100);
  driveWait(0.25, 100);
  punchSet(true);
  stopOnLine(-0.3);
  punchSet(false);
  frontGrabSet(false);
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
  driveDist(0.8, 10, 's');
  driveTurnDeg(-45-getGyro(), 127, 20);
  stopOnLine(-0.5);
  driveDist(1);
  punchSet(true);
  stopOnLine(-0.5, 127);
  punchSet(false);
  frontGrabSet(false);
}

void firstTraverse(){
  resetGyro();
  driveWait(-1.25, -90);
  smartGrabFront(-40);
  driveStop();
  delay(200);
  driveTurnDeg(-getGyro());
  driveWait(-0.25, -90, -90);
  driveWait(-0.15, -20, -20);
  smartGrabBack(-40);
  driveStop();
  delay(200);
  driveTurnDeg(180-getGyro());
  resetGyro();
  driveWait(0.4, 127);
  driveDist(0.2);
  driveTurnDeg(-10-getGyro());
}

void backOverFront(){
  backGrabSet(false);
  frontGrabSet(false);

}
