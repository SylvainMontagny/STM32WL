/**
 ******************************************************
 * @file 	lcd_printf.c
 * @brief 	This file provides all the functions needed to
 * 			display logs on a GFX01M2 LCD screen with st7789
 * 			driver.
 * @author Sacha USMB
 */

#include "lcd_printf.h"
#include <string.h>

int8_t buf_start, buf_end;
int8_t prev_buf_start, prev_buf_end;
static line_t lcd_log_buffer[BUF_LEN]; 			// Buffer of lines
static line_t prev_lcd_log_buffer[BUF_LEN];		// Previous printed lines

void LCD_Buffer_Init(void)
{
	// Init buffer's pointers
	buf_start = -1;
	buf_end = -1;
	prev_buf_start = 0;
	prev_buf_end = 0;

	// Init buffers with empty values
	char empty_str[LINE_SIZE] = "";
	for (uint8_t l = 0; l < BUF_LEN; l++){
		strcpy(prev_lcd_log_buffer[l].line, empty_str);
		strcpy(lcd_log_buffer[l].line, empty_str);
		lcd_log_buffer[l].color = LCD_DEFAULT_BACKGROUND;
	}
}

/**
 * @brief 	Print buffer values in the LCD screen
 */
void lcd_print_buf(void)
{
#ifdef LCD_DISPLAY
	for (uint8_t nbline = 0; nbline < BUF_LEN; nbline ++) {
		int8_t line = (nbline+buf_start) % BUF_LEN;
		int8_t prev_line = (nbline+prev_buf_start) % BUF_LEN;
		int16_t x = 10;
		int32_t y = LINE_HEIGHT/2 + LINE_HEIGHT*nbline;
		// Remove old line
		ST7789_WriteString(x, y, prev_lcd_log_buffer[(prev_line)%BUF_LEN].line, FONT, LCD_DEFAULT_BACKGROUND, LCD_DEFAULT_BACKGROUND);
		// Write new line
		ST7789_WriteString(x, y, lcd_log_buffer[line].line, FONT, lcd_log_buffer[line].color, LCD_DEFAULT_BACKGROUND);

		if (line == buf_end) nbline = BUF_LEN;
	}

	// Save new buffer to previous buffer
	memcpy(prev_lcd_log_buffer, lcd_log_buffer, BUF_LEN*LINE_SIZE);
	prev_buf_start = buf_start;
	prev_buf_end = buf_end;
#endif
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

	// Init variadiques arguments
	va_start(args, format);
	vsnprintf(lcd_log_buffer[buf_end].line, sizeof(lcd_log_buffer[buf_end].line), format, args); // Format string
	va_end(args);
}
