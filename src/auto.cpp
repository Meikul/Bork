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
void launchFirstBase();
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
  driveDist(4);
  smartGrabFront(40);
  gateSet(false);
  delay(700);
  driveWait(-0.25, -50, -50);
  driveWait(-0.5, -70, -70);
  driveSetImm(0, 0);
  delay(200);
  driveTurnDeg(-53);
}

void launchFirstBase(){

}

void firstTraverse(){

}
