/*
 * lcd_printf.h
 *
 *  Created on: May 19, 2025
 *      Author: Sacha USMB
 */

#ifndef INC_LCD_PRINTF_H_
#define INC_LCD_PRINTF_H_

//#include "main.h"
#include <string.h>
#include "st7789.h"
#include <stdio.h>
#include <stdarg.h>
#include "sys_app.h"
#include <stdlib.h>

#define LCD_DISPLAY			// Allow logs to be display on the LCD screen
#define DISPLAY_NB_LINES	// Add line number to the logs

#define LCD_DEFAULT_BACKGROUND LCD_WHITE
#define LCD_DEFAULT_FONT_COLOR LCD_BLACK
/* Choose font size */
#define SMALL_FONT
//#define MEDIUM_FONT
//#define LARGE_FONT

#ifdef SMALL_FONT
	#define FONT Font_7x10
	#define LINE_HEIGHT 12
#ifdef DISPLAY_NB_LINES
	#define LEFT_MARGIN 3
#else
	#define LEFT_MARGIN 10
#endif /* DISPLAY_NB_LINES */
	#define BUF_LEN 26 // Default 26
	#define LINE_SIZE 34 // Please consider 3 characters less when DISPLAY_NB_LINE is define and 1 less for the final character (end of line)
#endif /* SMALL_FONT */

#ifdef MEDIUM_FONT
	#define FONT Font_11x18
	#define LINE_HEIGHT 20
	#define LEFT_MARGIN 3
	#define BUF_LEN 15
	#define LINE_SIZE 20
#endif /* MEDIUM_FONT */

#ifdef LARGE_FONT
	#define FONT Font_16x26
	#define LINE_HEIGHT 30
	#define LEFT_MARGIN 3
	#define BUF_LEN 11
	#define LINE_SIZE 14
#endif /* LARGE_FONT */


void LCD_Buffer_Init(void);
void lcd_print_buf(void);
void lcd_printf(uint16_t, const char*, ...);

typedef struct _line {
	uint16_t color;
	char line[LINE_SIZE];
} line_t;

#endif /* INC_LCD_PRINTF_H_ */
