#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart.h"

#define BUFFER_SIZE 64

volatile uint8_t rx_buffer[BUFFER_SIZE];
volatile uint8_t rx_head = 0;
volatile uint8_t rx_tail = 0;

void uart_init(void) {
	uint16_t ubrr = (F_CPU / 16 / 9600) - 1;
	UBRR0H = (ubrr >> 8);
	UBRR0L = ubrr;

	UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0); // RX, TX e RX Interrupt
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);               // 8 bits, 1 stop

	sei(); // Habilita interrupções globais
}

void uart_send_byte(uint8_t data) {
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = data;
}

void uart_send_string(const char* str) {
	while (*str) {
		uart_send_byte(*str++);
	}
}

ISR(USART0_RX_vect) {
	uint8_t data = UDR0;
	uint8_t next_head = (rx_head + 1) % BUFFER_SIZE;

	if (next_head != rx_tail) {
		rx_buffer[rx_head] = data;
		rx_head = next_head;
	}
}

int uart_read_byte(void) {
	if (rx_head == rx_tail) {
		return -1; // Buffer vazio
		} else {
		uint8_t data = rx_buffer[rx_tail];
		rx_tail = (rx_tail + 1) % BUFFER_SIZE;
		return data;
	}
}

uint8_t uart_available(void) {
	return (rx_head != rx_tail);
}