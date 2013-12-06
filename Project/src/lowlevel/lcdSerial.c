/*
 * =====================================================================================
 *
 *       Filename:  lcdSerial.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2013/1/18 9:33:38
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  madcc (good lucky), madcc09@163.com
 *                  zhangyuxiang
 *   Organization:
 *
 * =====================================================================================
 */

#include "stm32f10x.h"
#include "lcdSerial.h"
#include "common.h"

static uint8_t lcdSerial_displayfunction;
static uint8_t lcdSerial_displaycontrol;
static uint8_t lcdSerial_displaymode;

static uint8_t lcdSerial_initialized;

static uint8_t lcdSerial_xcursor;
static uint8_t lcdSerial_ycursor;

static uint8_t lcdSerial_numLines, lcdSerial_numCols;

static void delay_us_(uint32_t us)
{
    Delay_us(us*9);
}

// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set:
//    DL = 1; 8-bit interface data
//    N = 0; 1-line display
//    F = 0; 5x8 dot character font
// 3. Display on/off control:
//    D = 0; Display off
//    C = 0; Cursor off
//    B = 0; Blinking off
// 4. Entry mode set:
//    I/D = 1; Increment by 1
//    S = 0; No shift
//
// Note, however, that resetting the Arduino doesn't reset the LCD, so we
// can't assume that its in that state when a sketch starts (and the
// LiquidCrystal constructor is called).

void lcdSerialGPIOinit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_GPIOClockCmd(LCD_Port, ENABLE);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;          //口线翻转速度为50MHz

    GPIO_InitStructure.GPIO_Pin = LCD_DB4 | LCD_DB5 | LCD_DB6 | LCD_DB7 | LCD_RS | LCD_RW | LCD_E;
    GPIO_Init(LCD_Port, &GPIO_InitStructure);
}
void lcdSerialInit(void)
{
    bool fourbitmode = true;
    lcdSerialGPIOinit();
    if (fourbitmode)
        lcdSerial_displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
    else
        lcdSerial_displayfunction = LCD_8BITMODE | LCD_1LINE | LCD_5x8DOTS;

    GPIO_ResetBits(LCD_Port, LCD_E);

    lcdSerialBegin(LCD_SCREEN_WIDTH, LCD_SCREEN_HEIGHT);
    // begin(16, 1);
}

void lcdSerialBegin(uint8_t cols, uint8_t lines)
{
    // program special characters
    uint8_t right[] = {0, 4, 2, 1, 2, 4, 0};
    uint8_t down[] = {0, 0, 0, 0, 0, 0x11, 0xA, 4};

    if (lines > 1)
    {
        lcdSerial_displayfunction |= LCD_2LINE;
    }
    lcdSerial_numLines = lines;
    lcdSerial_numCols = cols;

    //  // for some 1 line displays you can select a 10 pixel high font
    //  if ((dotsize != 0) && (lines == 1)) {
    //    lcdSerial_displayfunction |= LCD_5x10DOTS;
    //  }
    //
    // SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
    // according to datasheet, we need at least 40ms after power rises above 2.7V
    // before sending lcdSerialCommands. Arduino can turn on way befer 4.5V so we'll wait 50
    Delay_ms(300);
    // Now we pull both RS and R/W low to begin lcdSerialCommands
    //lcdSerialWriteSerial(0x00);toFix
    GPIO_ResetBits(LCD_Port, LCD_RS);
    GPIO_ResetBits(LCD_Port, LCD_RW);
    GPIO_ResetBits(LCD_Port, LCD_E);
    delay_us_(4);
    //lcdSerialWriteSerial(0b00000000);

    //put the LCD into 4 bit or 8 bit mode
    if (! (lcdSerial_displayfunction & LCD_8BITMODE))
    {
        // this is according to the hitachi HD44780 datasheet
        // figure 24, pg 46

        // we start in 8bit mode, try to set 4 bit mode
        lcdSerialLoad(0x03 << 4);
        delay_us_(4500); // wait min 4.1ms

        // second try
        lcdSerialLoad(0x03 << 4);
        delay_us_(4500); // wait min 4.1ms

        // third go!
        lcdSerialLoad(0x03 << 4);
        delay_us_(150);

        // finally, set to 4-bit interface
        lcdSerialLoad(0x02 << 4);
        delay_us_(500);
    }
    else
    {
        // this is according to the hitachi HD44780 datasheet
        // page 45 figure 23

        // Send function set lcdSerialCommand sequence
        lcdSerialCommand(LCD_FUNCTIONSET | lcdSerial_displayfunction);
        delay_us_(4500);  // wait more than 4.1ms

        // second try
        lcdSerialCommand(LCD_FUNCTIONSET | lcdSerial_displayfunction);
        delay_us_(150);

        // third go
        lcdSerialCommand(LCD_FUNCTIONSET | lcdSerial_displayfunction);
    }

    // finally, set # lines, font size, etc.
    lcdSerialCommand(LCD_FUNCTIONSET | lcdSerial_displayfunction);
    delay_us_(3000);
    // turn the display on with no cursor or blinking default
    lcdSerial_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    lcdSerialDisplay();

    // clear it off
    lcdSerialClear();

    // Initialize to default text direction (for romance languages)
    lcdSerial_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    // set the entry mode
    lcdSerialCommand(LCD_ENTRYMODESET | lcdSerial_displaymode);

    // write each character twice as sometimes there are signal issues
    lcdSerialCreateChar(0, right);
    lcdSerialCreateChar(0, right);
    lcdSerialCreateChar(1, down);
    lcdSerialCreateChar(1, down);

}

/********** high level lcdSerialCommands, for the user! */
void lcdSerialClear()
{
    lcdSerialCommand(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
    delay_us_(2000);  // this lcdSerialCommand takes a long time!
}

void lcdSerialHome()
{
    lcdSerialCommand(LCD_RETURNHOME);  // set cursor position to zero
    delay_us_(2000);  // this lcdSerialCommand takes a long time!
}

void lcdSerialSetCursor(uint8_t col, uint8_t row)
{
    int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
    if ( row > lcdSerial_numLines )
    {
        row = lcdSerial_numLines - 1;  // we count rows starting w/0
    }

    lcdSerial_xcursor = col; lcdSerial_ycursor = row;
    lcdSerialCommand(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void lcdSerialNoDisplay(void)
{
    lcdSerial_displaycontrol &= ~LCD_DISPLAYON;
    lcdSerialCommand(LCD_DISPLAYCONTROL | lcdSerial_displaycontrol);
}
void lcdSerialDisplay(void)
{
    lcdSerial_displaycontrol |= LCD_DISPLAYON;
    lcdSerialCommand(LCD_DISPLAYCONTROL | lcdSerial_displaycontrol);
}

// Turns the underline cursor on/off
void lcdSerialNoCursor(void)
{
    lcdSerial_displaycontrol &= ~LCD_CURSORON;
    lcdSerialCommand(LCD_DISPLAYCONTROL | lcdSerial_displaycontrol);
}
void lcdSerialCursor(void)
{
    lcdSerial_displaycontrol |= LCD_CURSORON;
    lcdSerialCommand(LCD_DISPLAYCONTROL | lcdSerial_displaycontrol);
}

// Turn on and off the blinking cursor
void lcdSerialNoBlink(void)
{
    lcdSerial_displaycontrol &= ~LCD_BLINKON;
    lcdSerialCommand(LCD_DISPLAYCONTROL | lcdSerial_displaycontrol);
}
void lcdSerialBlink(void)
{
    lcdSerial_displaycontrol |= LCD_BLINKON;
    lcdSerialCommand(LCD_DISPLAYCONTROL | lcdSerial_displaycontrol);
}

// These lcdSerialCommands scroll the display without changing the RAM
void lcdSerialScrollDisplayLeft(void)
{
    lcdSerialCommand(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void lcdSerialScrollDisplayRight(void)
{
    lcdSerialCommand(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void lcdSerialLeftToRight(void)
{
    lcdSerial_displaymode |= LCD_ENTRYLEFT;
    lcdSerialCommand(LCD_ENTRYMODESET | lcdSerial_displaymode);
}

// This is for text that flows Right to Left
void lcdSerialRightToLeft(void)
{
    lcdSerial_displaymode &= ~LCD_ENTRYLEFT;
    lcdSerialCommand(LCD_ENTRYMODESET | lcdSerial_displaymode);
}

// This will 'right justify' text from the cursor
void lcdSerialAutoscroll(void)
{
    lcdSerial_displaymode |= LCD_ENTRYSHIFTINCREMENT;
    lcdSerialCommand(LCD_ENTRYMODESET | lcdSerial_displaymode);
}

// This will 'left justify' text from the cursor
void lcdSerialNoAutoscroll(void)
{
    lcdSerial_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
    lcdSerialCommand(LCD_ENTRYMODESET | lcdSerial_displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void lcdSerialCreateChar(uint8_t location, uint8_t charmap[])
{
    int i;
    location &= 0x7; // we only have 8 locations 0-7
    lcdSerialCommand(LCD_SETCGRAMADDR | (location << 3));
    for (i = 0; i < 8; i++)
    {
        lcdSerialWrite(charmap[i]);
    }
}

/*********** mid level lcdSerialCommands, for sending data/cmds */

void lcdSerialCommand(uint8_t value)
{
    lcdSerialSend(value, false);
}

void lcdSerialWrite(uint8_t value)
{
    lcdSerialSend(value, true);
    lcdSerial_xcursor++;
    if (lcdSerial_xcursor >= lcdSerial_numCols)
        lcdSerialSetCursor(0, lcdSerial_ycursor + 1);
}

void lcdSerialWriteInt(uint16_t value, uint8_t digits)
{

    uint16_t currentDigit;
    uint16_t nextDigit;
    uint8_t i;

    switch (digits)
    {
    case 1:     currentDigit = 10;      break;
    case 2:     currentDigit = 100;     break;
    case 3:     currentDigit = 1000;    break;
    case 4:     currentDigit = 10000;   break;
    default:    return;
    }

    for ( i = 0; i < digits; i++)
    {
        nextDigit = currentDigit / 10;
        lcdSerialWrite((value % currentDigit) / nextDigit + '0');
        currentDigit = nextDigit;
    }
}


char *lcdSerialWriteLine(char *message)
{
    char *letter = message;
    while (*letter != 0 && *letter != '\n')
    {
        //INTERFACE_RLED.setValue(true);
        lcdSerialWrite(*letter);
        letter++;

    }
    return letter;
}

void lcdSerialWriteString(char message[])
{
    char *letter = message;
    while (*letter != 0)
    {
        lcdSerialWrite(*letter);
        letter++;
    }
}

//void lcdSerialWriteFromPgmspace(const prog_uchar message[]) {
//  char letter;
//  while (letter = pgm_read_byte(message++)) {
//      lcdSerialWrite(letter);
//  }
//}
//
/************ low level data pushing lcdSerialCommands **********/

// lcdSerialWrite either lcdSerialCommand or data, with automatic 4/8-bit selection
void lcdSerialSend(uint8_t value, bool mode)
{
    uint8_t modeBits;
    // set mode value
    if (mode)
        modeBits = 0x02;
    else
        modeBits = 0x00;

    //serial assumes 4 bit mode
    lcdSerialLoad((value & 0xF0) + modeBits);
    lcdSerialLoad(((value << 4) & 0xF0) + modeBits);
    delay_us_(200);//according to the datasheet,should check the busy signal,but we can not read ,so delay.
    //however ,without the delay ,it can work. RY
    //lcdSerialCheckBusy();
}
void lcdSerialRead(uint8_t value, bool mode)
{
    uint8_t modeBits;
    if (mode)
        modeBits = 0x06;
    else
        modeBits = 0x04;
    //  lcdSerialLoad((value&0xF0)+modeBits);
    //  lcdSerialLoad(((value<<4)&0xF0)+modeBits);
    GPIO_SetBits(LCD_Port, LCD_RW);
    GPIO_ResetBits(LCD_Port, LCD_RS);
    GPIO_SetBits(LCD_Port, LCD_E);
    delay_us_(4);
    GPIO_ResetBits(LCD_Port, LCD_E);
    delay_us_(4);
    GPIO_SetBits(LCD_Port, LCD_E);
    delay_us_(4);
    GPIO_ResetBits(LCD_Port, LCD_E);
    delay_us_(4);
}
bool lcdSerialCheckBusy(void)
{
    lcdSerialRead(0x00, false);
}
void lcdSerialLoad(uint8_t value)
{
    if (value & 0x80)
        GPIO_SetBits(LCD_Port, LCD_DB7);
    else
        GPIO_ResetBits(LCD_Port, LCD_DB7);
    if (value & 0x40)
        GPIO_SetBits(LCD_Port, LCD_DB6);
    else
        GPIO_ResetBits(LCD_Port, LCD_DB6);
    if (value & 0x20)
        GPIO_SetBits(LCD_Port, LCD_DB5);
    else
        GPIO_ResetBits(LCD_Port, LCD_DB5);
    if (value & 0x10)
        GPIO_SetBits(LCD_Port, LCD_DB4);
    else
        GPIO_ResetBits(LCD_Port, LCD_DB4);
    if (value & 0x04)
        GPIO_SetBits(LCD_Port, LCD_RW);
    else
        GPIO_ResetBits(LCD_Port, LCD_RW);
    if (value & 0x02)
        GPIO_SetBits(LCD_Port, LCD_RS);
    else
        GPIO_ResetBits(LCD_Port, LCD_RS);
    delay_us_(1);
    GPIO_SetBits(LCD_Port, LCD_E);
    delay_us_(4);
    GPIO_ResetBits(LCD_Port, LCD_E);
    delay_us_(1);
    //  lcdSerialWriteSerial(value);
    //  lcdSerialPulseEnable(value);
}

//void lcdSerialPulseEnable(uint8_t value) {
//
//  delay_us_(1);
//   // set enable to true
//   //value |= 0b01000;
//   value |= 0x08;
//   lcdSerialWriteSerial(value);
//  delay_us_(4);    // enable pulse must be >450ns  //may be this can be delay_us_(1);RY
//   //set enable to false
//   //value &= 0b11110111;
//   value &= 0xF7;
//   lcdSerialWriteSerial(value);
//  delay_us_(1);   // lcdSerialCommands need > 37us to settle [citation needed]
//}
//
//void lcdSerialWriteSerial(uint8_t value)
//{
//
//  int i;
//
//  for (i = 7; i >= 0; i--)
//  {
//        GPIO_ResetBits(LCD_CLK_Port,LCD_CLK_Pin);
//    if( (value >> i) & 0x01 )
//        GPIO_SetBits(LCD_DATA_Port,LCD_DATA_Pin);
//    else
//        GPIO_ResetBits(LCD_DATA_Port,LCD_DATA_Pin);
//
//    delay_us_(4); //may be this can be delay_us_(1);RY
//    GPIO_SetBits(LCD_CLK_Port,LCD_CLK_Pin);
//    delay_us_(4);
//  }
//
//    GPIO_SetBits(LCD_STROBE_Port,LCD_STROBE_Pin);
//    delay_us_(4);//may be this can be delay_us_(1);RY
//    GPIO_ResetBits(LCD_STROBE_Port,LCD_STROBE_Pin);
//
//}
//bool lcdSerialCheckBusy()
//{
//  lcdSerialWriteSerial(0x0C);
//  lcdSerialWriteSerial(0x04);
//
//}
