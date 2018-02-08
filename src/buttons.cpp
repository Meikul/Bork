#include "buttons.h"

bool isPressed(button_t button){
  bool value = false;
  if(button < lcdLeft){
    int group = int(button / 4) + 5;
    int btn = button%4;
    switch (btn) {
      case 0: btn = 1; // down
        break;
      case 1: btn = 4; // up
        break;
      case 2: btn = 2; // left
        break;
      case 3: btn = 8; // right
    }
    value = joystickGetDigital(1, group, btn);
  }
  else{
    int btns = lcdReadButtons(uart1);
    if(button == lcdLeft){
      value = (btns%2 == 1);
    }
    else if(button == lcdMid){
      value = (btns%4 == 2 || btns%4 == 3);
    }
    else if(button == lcdRight){
      value = (btns >= 4);
    }
  }
  return value;
}

bool lastPress[19] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

bool isNewPress(button_t button){
  bool currentButton = isPressed(button);

	if(!currentButton) lastPress[button] = false;

	if(currentButton && !lastPress[button]) {
		lastPress[button] = true;
		return true;
	}

	return false;
}

bool isNewRelease(button_t button){
  bool currentButton = isPressed(button);

	if(currentButton) lastPress[button] = true;

	else if(!currentButton && lastPress[button]) {
		lastPress[button] = false;
		return true;
	}

	return false;
}
