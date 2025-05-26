#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"

volatile uint16_t contador_ms = 0;

void timer0_init(void) {
	// Configura Timer0 para overflow a cada 1ms
	TCCR0A = 0;                           // Normal mode
	TCCR0B = (1 << CS01) | (1 << CS00);   // Prescaler 64

	// C�lculo para overflow:
	// (16MHz / 64) = 250kHz ? 1 tick = 4us
	// 256 ticks = 1.024ms ? Bem pr�ximo de 1ms

	TIMSK0 = (1 << TOIE0);  // Habilita interrup��o de overflow

	sei();  // Habilita interrup��es globais
}

ISR(TIMER0_OVF_vect) {
	contador_ms++;
}

void delay_ms(uint16_t ms) {
	contador_ms = 0;
	while (contador_ms < ms);
}
