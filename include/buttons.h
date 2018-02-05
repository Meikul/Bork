#ifndef BUTTONS_H_
#define BUTTONS_H_

#include <API.h>

/**
 * Renames the input channels to be more descriptive
 */
typedef enum {
	btn5d = 0,
	btn5u = 1,
	btn6d = 4,
	btn6u = 5,
	btn7d = 8,
	btn7u = 9,
	btn7l = 10,
	btn7r = 11,
	btn8d = 12,
	btn8u = 13,
	btn8l = 14,
	btn8r = 15,

	lcdLeft = 16,
	lcdMid = 17,
	lcdRight = 18
} button_t;

/**
 * Checks for a new button press
 * @param  button which button to check
 * @return        if button is being newly pressed
 */
bool isNewPress(button_t button);
/**
 * Checks if a button is pressed
 * @param  button which button to check
 * @return        if button is being pressed
 */
bool isPressed(button_t button);

#endif
