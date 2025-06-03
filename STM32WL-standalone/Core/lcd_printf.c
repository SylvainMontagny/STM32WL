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
char buffer_mutex;								// 0: available buffer, 1: buffer in use
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
	while (buffer_mutex != 0);
	buffer_mutex = 1;

	for (uint8_t nbline = 0; nbline < BUF_LEN; nbline ++) {
		int8_t line = (nbline+buf_start) % BUF_LEN;
		int8_t prev_line = (nbline+prev_buf_start) % BUF_LEN;
		int16_t x = LEFT_MARGIN;
		int32_t y = LINE_HEIGHT/2 + LINE_HEIGHT*nbline;
		if (strlen(prev_lcd_log_buffer[(prev_line)%BUF_LEN].line) > strlen(lcd_log_buffer[line].line)) {
			// Remove old line
			ST7789_WriteString(x, y, prev_lcd_log_buffer[(prev_line)%BUF_LEN].line, FONT, LCD_DEFAULT_BACKGROUND, LCD_DEFAULT_BACKGROUND);
		}
		// Write new line
		ST7789_WriteString(x, y, lcd_log_buffer[line].line, FONT, lcd_log_buffer[line].color, LCD_DEFAULT_BACKGROUND);

		if (line == buf_end) nbline = BUF_LEN;
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
