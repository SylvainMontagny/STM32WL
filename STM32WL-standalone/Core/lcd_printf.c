/**
 ******************************************************
 * @file 	lcd_printf.c
 * @brief 	This file provides all the functions needed to
 * 			display logs on a GFX01M2 LCD screen with st7789
 * 			driver.
 * @author Sacha USMB
 */

#include "lcd_printf.h"

int8_t buf_start, buf_end;
static line_t lcd_log_buffer[BUF_LEN]; // Buffer of lines

void LCD_Buffer_Init(void)
{
	buf_start = -1;
	buf_end = -1;
}

/**
 * @brief 	Print buffer values in the LCD screen
 */
void lcd_print_buf(void)
{
	for (uint8_t nbline = 0; nbline < BUF_LEN; nbline ++) {
		uint8_t line = (nbline+buf_start) % BUF_LEN;
		ST7789_WriteString(10, LINE_HEIGHT/2 + LINE_HEIGHT*nbline, lcd_log_buffer[line].line, FONT, lcd_log_buffer[line].color, DEFAULT_BACKGROUND);

		if (line == buf_end) nbline = BUF_LEN;
	}
}

/**
 * @brief	Add a new line to the buffer of lines. Call lcd_print_buf then to display the lines on the
 * 			LCD screen
 */
void lcd_printf(uint16_t color, const char* format, ...)
{
	buf_end = (buf_end + 1) % BUF_LEN;
	//APP_LOG(0, 1, "s=%d; e=%d \r\n", buf_start, buf_end);

	// Adjust start and end buffer pointers
	if (buf_start == buf_end) {
		buf_start = (buf_start + 1) % BUF_LEN;
	}
	else if (buf_start == -1) buf_start = 0;

	lcd_log_buffer[buf_end].color = color;
	va_list args;

	// Initialiser les arguments variadiques
	va_start(args, format);
	vsnprintf(lcd_log_buffer[buf_end].line, sizeof(lcd_log_buffer[buf_end].line), format, args); // Format string
	va_end(args);
}
