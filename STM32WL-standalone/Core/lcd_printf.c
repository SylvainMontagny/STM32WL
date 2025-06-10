/**
 ******************************************************
 * @file 	lcd_printf.c
 * @brief 	This file provides all the functions needed to
 * 			display logs on a GFX01M2 LCD screen with st7789
 * 			driver.
 * @author 	Sacha USMB
 * @Date 	apr 2025
 */

#include "lcd_printf.h"

int8_t buf_start, buf_end;
int8_t prev_buf_start, prev_buf_end;
char buffer_mutex;								// 0: available buffer,
												// 1: buffer in use by lcd_printf,
												// 2: buffer in use by lcd_print_buf
static line_t lcd_log_buffer[BUF_LEN]; 			// Buffer of lines
static line_t prev_lcd_log_buffer[BUF_LEN];		// Previous printed lines
uint8_t line_nb;

/**
 * @brief	Init LCD variables
 */
void LCD_Buffer_Init(void)
{
	// Init buffer's pointers
	buf_start = -1;
	buf_end = -1;
	prev_buf_start = 0;
	prev_buf_end = 0;

	buffer_mutex = 0;

	line_nb = 1;

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
	while (buffer_mutex == 1);
	buffer_mutex = 2;

	int8_t line;
	int8_t prev_line;
	int16_t x = LEFT_MARGIN;
	int32_t y = -LINE_HEIGHT/2;
	int16_t diff_prev_new_line_len;
	char suffix[LINE_SIZE];
	char line_to_print[LINE_SIZE];

	for (uint8_t nbline = 0; nbline < BUF_LEN; nbline ++) {
		line = (nbline+buf_start) % BUF_LEN;
		prev_line = (nbline+prev_buf_start) % BUF_LEN;
		y += LINE_HEIGHT;

		if ( strcmp(lcd_log_buffer[line].line , prev_lcd_log_buffer[(prev_line)%BUF_LEN].line) != 0 )
		{
			// If lines are different, print new line
			strcpy(line_to_print, lcd_log_buffer[line].line);
			// Handle suffix to only erase the end of line
			strcpy(suffix, "");
			diff_prev_new_line_len = strlen(prev_lcd_log_buffer[(prev_line)%BUF_LEN].line) - strlen(lcd_log_buffer[line].line);
			if ( diff_prev_new_line_len > 0 )
			{
				for (int8_t ch = 0; ch < diff_prev_new_line_len; ch++)
				{
					strcat(suffix, " ");
				}
			}
			// Write new line
			ST7789_WriteString(x, y, strcat(line_to_print, suffix), FONT, lcd_log_buffer[line].color, LCD_DEFAULT_BACKGROUND);
		}
		else
		{
#ifndef DISPLAY_NB_LINES
			// Avoid display issues after a screen refresh
			// Otherwise, strings stored in prev_lcd_log_buffer and that remains the same after a new lcd_printf
			// are not printed again after a screen refresh. This leads to missing lines.
			ST7789_WriteString(x, y, lcd_log_buffer[line].line, FONT, lcd_log_buffer[line].color, LCD_DEFAULT_BACKGROUND);
#endif
		}

		if ( line == buf_end ) nbline = BUF_LEN;
	}

	// Save new buffer to previous buffer
	memcpy(prev_lcd_log_buffer, lcd_log_buffer, BUF_LEN*sizeof(line_t));
	prev_buf_start = buf_start;
	prev_buf_end = buf_end;

	buffer_mutex = 0;
#endif
}

/**
 * @brief	Add a new line to the buffer of lines. Call lcd_print_buf then to display the lines on the
 * 			LCD screen
 */
void lcd_printf(uint16_t color, const char* format, ...)
{
#ifdef LCD_DISPLAY
	// Probeer maar wacht niet op mutex, en naam het. Anders er is een deadlock
	if (buffer_mutex != 0) return;
	buffer_mutex = 1;

	buf_end = (buf_end + 1) % BUF_LEN;

	// Adjust start and end buffer pointers
	if (buf_start == buf_end) {
		buf_start = (buf_start + 1) % BUF_LEN;
	}
	else if (buf_start == -1) buf_start = 0;

	lcd_log_buffer[buf_end].color = color;
	va_list args;

	char str_line_nb[3]; // Max 3 characters: 2 digits and a space
	size_t str_line_nb_len = 0;

#ifdef DISPLAY_NB_LINES
	str_line_nb_len = sizeof(str_line_nb);
	itoa(line_nb, str_line_nb, 10);

	if (line_nb < 0xA){
		strcat(str_line_nb, " ");
	}
	strcat(str_line_nb, " ");

	line_nb = (line_nb + 1) % 100;
#endif // DISPLAY_NB_LINES

	// Init variadiques arguments
	strcpy(lcd_log_buffer[buf_end].line, str_line_nb);
	va_start(args, format);
	vsnprintf(lcd_log_buffer[buf_end].line + str_line_nb_len, LINE_SIZE - str_line_nb_len, format, args); // Format string
	va_end(args);

	// Verhoog/bevrijd mutex
	buffer_mutex = 0;
#endif
	return;
}
