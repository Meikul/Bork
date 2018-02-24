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
// void launchFirstBase();
void firstTraverse();

void autonomous() {
  switch(autonState){
    case 0:
      firstBase();
      break;
    case 1:
      launchFirstBase();
    case 2:
      firstTraverse();
  }
}

void firstBase(){
  frontGrabSet(false);
  gateSet(true);
  driveDist(4, 't');
  delay(200);
  smartGrabFront(60);
  delay(300);
  stopOnLine(-2.5);
  driveStop();
  // delay(200);
  // driveTurnDeg(-65);
  // driveWaitRamp(-0.5, -70, 10);
  // driveDist(-1.75);
  // delay(300);
  // smartGrabBack(-40);
  // delay(200);
  // driveTurnDeg(-85);
}

void launchFirstBase(){
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
    delay(20);
  }
  // driveTime(300, -127);
  driveStop();
  delay(50);
  driveTime(600, -127);
  punchSet(false);
  stopOnLine(-0.5);
  delay(500);
  frontGrabSet(false);
  driveStop();
  delay(300);
}

void firstTraverse(){

}
