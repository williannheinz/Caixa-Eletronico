#define F_CPU 16000000UL
#include <avr/io.h>
#include "timer.h"
#include "lcd.h"

// RS -> PD2, E -> PD3, D4–D7 -> PD4–PD7
#define LCD_DDR DDRD
#define LCD_PORT PORTD

#define RS PD2
#define E  PD3

void lcd_enable_pulse() {
	LCD_PORT |= (1 << E);
	delay_ms(1);
	LCD_PORT &= ~(1 << E);
	delay_ms(1);
}

void lcd_send_nibble(uint8_t nibble) {
	// Zera os bits de dados
	LCD_PORT &= 0x0F;

	// Coloca os bits de dados (D4–D7)
	LCD_PORT |= (nibble & 0x0F) << 4;

	lcd_enable_pulse();
}

void lcd_send_byte(uint8_t byte, uint8_t is_data) {
	if (is_data)
	LCD_PORT |= (1 << RS);
	else
	LCD_PORT &= ~(1 << RS);

	lcd_send_nibble(byte >> 4);
	lcd_send_nibble(byte & 0x0F);
}

void lcd_command(uint8_t cmd) {
	lcd_send_byte(cmd, 0);
	delay_ms(2);
}

void lcd_write_char(char c) {
	lcd_send_byte(c, 1);
	delay_ms(1);
}

void lcd_print(const char* str) {
	while (*str) {
		lcd_write_char(*str++);
	}
}

void lcd_set_cursor(uint8_t row, uint8_t col) {
	const uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
	lcd_command(0x80 | (col + row_offsets[row]));
}

void lcd_clear(void) {
	lcd_command(0x01);
	delay_ms(2);
}

void lcd_init(void) {
	LCD_DDR |= 0xFC; // PD2–PD7 como saída
	delay_ms(50);

	lcd_send_nibble(0x03); delay_ms(5);
	lcd_send_nibble(0x03); delay_ms(1);
	lcd_send_nibble(0x03); delay_ms(150);
	lcd_send_nibble(0x02); delay_ms(1); // 4-bit mode

	lcd_command(0x28); // 4-bit, 2 linhas, 5x8
	lcd_command(0x0C); // Display on, cursor off
	lcd_command(0x06); // Incrementa cursor
	lcd_clear();
}
