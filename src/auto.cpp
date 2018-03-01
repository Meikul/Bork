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

#include <math.h>

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
  driveDist(-1.5, 's');
  driveTurnDeg(-86-getGyro());
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
  driveStop();
}

void firstCorner(){
  unsigned int wallDist = getProx();
  const double fullHypo = 68.0;
  double curHypo = sqrt(2 * (wallDist * wallDist));
  double dist = (fullHypo - curHypo) / 12.0;
  driveTurnDeg(90);
  driveDist(-dist, 't');
  driveTurnDeg(-45);
  driveDist(-3.5);
  smartGrabFront(-40);
  driveSetImm(100, 100);
  delay(500);
  while(isOverLine()){ //back off of tile
    driveSet(127, 127);
    delay(20);
  }
  driveToLine(127);
  driveDist(2.5, 't');
  driveStop();
  punchSet(true);
  delay(500);
  punchSet(false);
}

void firstTraverse(){
  driveTurnDeg(180);
  driveDist(2.0);
  smartGrabFront(40);
  driveWait(1, 60);
  driveStop();
  delay(400);
  driveTurnDeg(20);
}

void backOverFront(){
  backGrabSet(false);
  frontGrabSet(false);

}
