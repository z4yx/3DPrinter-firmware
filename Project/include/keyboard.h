#ifndef __KEYBOARD_H
#define	__KEYBOARD_H

#include "common.h"

#define NUM_KEYS 5

enum { KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_DOWN, KEY_CENTER };

void KeyBoard_Init(void);
bool KeyBoard_isPressed(uint8_t num);

#endif /* __KEYBOARD_H */