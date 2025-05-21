/*
 * lcd_printf.h
 *
 *  Created on: May 19, 2025
 *      Author: Sacha USMB
 */

#ifndef INC_LCD_PRINTF_H_
#define INC_LCD_PRINTF_H_

#include "main.h"
#include "st7789.h"
#include <stdio.h>
#include <stdarg.h>
//#include "sys_app.h"

#define DEFAULT_BACKGROUND LCD_WHITE
/* Choose font size */
//#define SMALL_FONT
#define MEDIUM_FONT
//#define LARGE_FONT

#ifdef SMALL_FONT
	#define FONT Font_7x10
	#define LINE_HEIGHT 12
	#define BUF_LEN 25
	#define LINE_SIZE 32
#endif

#ifdef MEDIUM_FONT
	#define FONT Font_11x18
	#define LINE_HEIGHT 20
	#define BUF_LEN 15
	#define LINE_SIZE 20
#endif

#ifdef LARGE_FONT
	#define FONT Font_16x26
	#define LINE_HEIGHT 30
	#define BUF_LEN 11
	#define LINE_SIZE 14
#endif


void LCD_Buffer_Init(void);
void lcd_print_buf(void);
void lcd_printf(uint16_t, const char*, ...);

typedef struct _line {
	uint16_t color;
	char line[LINE_SIZE];
} line_t;

#endif /* INC_LCD_PRINTF_H_ */
