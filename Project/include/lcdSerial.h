/*
 * =====================================================================================
 *
 *       Filename:  lcdSerial.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2013/1/18 9:36:46
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  madcc (good lucky), madcc09@163.com
 *                  zhangyuxiang
 *   Organization:
 *
 * =====================================================================================
 */
#ifndef LIQUID_CRYSTAL_HH
#define LIQUID_CRYSTAL_HH

// TODO: Proper attribution

#include "stm32f10x.h"
#include "common.h"
// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// TODO:  make variable names for rs, rw, e places in the output vector

void lcdSerialGPIOinit(void);
void lcdSerialInit(void);

void lcdSerialBegin(uint8_t cols, uint8_t rows);

void lcdSerialClear(void);
void lcdSerialHome(void);

void lcdSerialNoDisplay(void);
void lcdSerialDisplay(void);
void lcdSerialNoBlink(void);
void lcdSerialBlink(void);
void lcdSerialNoCursor(void);
void lcdSerialCursor(void);
void lcdSerialScrollDisplayLeft(void);
void lcdSerialScrollDisplayRight(void);
void lcdSerialLeftToRight(void);
void lcdSerialRightToLeft(void);
void lcdSerialAutoscroll(void);
void lcdSerialNoAutoscroll(void);

void lcdSerialCreateChar(uint8_t, uint8_t[]);
void lcdSerialSetCursor(uint8_t, uint8_t);
void lcdSerialWrite(uint8_t);

/** Added by MakerBot Industries to support storing strings in flash **/
void lcdSerialWriteInt(uint16_t value, uint8_t digits);

void lcdSerialWriteString(char message[]);

/** Display the given line until a newline or null is encountered.
 * Returns a pointer to the first character not displayed.
 */
char *lcdSerialWriteLine(char *message);

//toFix
//void lcdSerialWriteFromPgmspace(const prog_uchar message[]);

void lcdSerialCommand(uint8_t);

void lcdSerialSend(uint8_t, bool);
//void lcdSerialWriteSerial(uint8_t);
void lcdSerialLoad(uint8_t);
//void lcdSerialPulseEnable(uint8_t value);

bool lcdSerialCheckBusy(void);

#endif // LIQUID_CRYSTAL_HH


