#define F_CPU 16000000UL
#include <avr/io.h>
#include "timer.h"
#include <stdint.h>
#include "LCD.h"
#include "TECLADO.h"
#include "UART.h"
#include <string.h>
// UART Config
#define BAUD 9600
#define MYUBRR (F_CPU / 16 / BAUD - 1)
#define entrada 20
enum Estado {
	ESTADO_LOGIN_USUARIO,
	ESTADO_LOGIN_SENHA,
	ESTADO_ENVIANDO_LOGIN,
	ESTADO_AGUARDANDO_RESPOSTA_LOGIN,
	ESTADO_MENU,
	ESTADO_PROCESSANDO
};

// ?? Aqui pode declarar a variável de controle do estado
enum Estado estado;

// Outras variáveis globais também aqui
char usuario[entrada+1] = {0};
char senha[entrada+1] = {0};
char buffer_digitacao[20] = {0};
uint8_t pos = 0;    
void simular_resposta_login_ok(void) {
	uart_send_byte('S');
	uart_send_byte('E');
	uart_send_byte(9); // Tamanho = 9

	uart_send_string("Acesso OK");
}
int main(void) {
	uart_init();
	timer0_init();
	lcd_init();
	keypad_init();

	estado = ESTADO_LOGIN_USUARIO;

	lcd_clear();
	lcd_set_cursor(0, 0);
	lcd_print("Usuario:");

	lcd_set_cursor(1, 0);

	while (1) {
		char tecla = keypad_get_key();
		if (tecla == '*') {
		simular_resposta_login_ok();
		}
		switch (estado) {

			// ?? Entrada do usuário
			case ESTADO_LOGIN_USUARIO:
			if (tecla) {
				if (tecla == '#') {
					buffer_digitacao[pos] = '\0';
					strcpy(usuario, buffer_digitacao);
					pos = 0;

					lcd_clear();
					lcd_set_cursor(0, 0);
					lcd_print("Senha:");

					estado = ESTADO_LOGIN_SENHA;
					} else if (pos < 6) {
					buffer_digitacao[pos++] = tecla;
					lcd_write_char(tecla);
				}
			}
			break;

			// ?? Entrada da senha
			case ESTADO_LOGIN_SENHA:
			if (tecla) {
				if (tecla == '#') {
					buffer_digitacao[pos] = '\0';
					strcpy(senha, buffer_digitacao);
					pos = 0;

					estado = ESTADO_ENVIANDO_LOGIN;
					} else if (pos < 6) {
					buffer_digitacao[pos++] = tecla;
					lcd_write_char('*'); // Mostra asterisco
				}
			}
			break;

			// ?? Envia login ao servidor
			case ESTADO_ENVIANDO_LOGIN:
			lcd_clear();
			lcd_set_cursor(0, 0);
			lcd_print("Aguardando servidor");

			uart_send_byte('C');
			uart_send_byte('E');
			uart_send_string(usuario);
			uart_send_string(senha);

			estado = ESTADO_AGUARDANDO_RESPOSTA_LOGIN;
			break;

			// ?? Aguarda e processa resposta
			case ESTADO_AGUARDANDO_RESPOSTA_LOGIN:
			if (uart_available() >= 3) {
				if (uart_read_byte() == 'S' && uart_read_byte() == 'E') {
					uint8_t tamanho = uart_read_byte();
					char resposta[32] = {0};

					for (uint8_t i = 0; i < tamanho; i++) {
						while (!uart_available());
						resposta[i] = uart_read_byte();
					}
					resposta[tamanho] = '\0';

					lcd_clear();
					lcd_set_cursor(0, 0);
					lcd_print(resposta);

					delay_ms(2000);

					if (strcmp(resposta, "Acesso OK") == 0) {
						lcd_clear();
						lcd_set_cursor(0, 0);
						lcd_print("1-Saldo 2-Saque");
						lcd_set_cursor(1, 0);
						lcd_print("3-Deposito #-Sair");

						estado = ESTADO_MENU;
						} else {
						lcd_clear();
						lcd_set_cursor(0, 0);
						lcd_print("Acesso Negado");
						delay_ms(2000);

						lcd_clear();
						lcd_set_cursor(0, 0);
						lcd_print("Usuario:");
						estado = ESTADO_LOGIN_USUARIO;
					}
				}
			}
			break;

			// ?? Menu principal
			case ESTADO_MENU:
			if (tecla) {
				if (tecla == '1') {
					lcd_clear();
					lcd_set_cursor(0, 0);
					lcd_print("Saldo...");
					estado = ESTADO_PROCESSANDO;
					} else if (tecla == '2') {
					lcd_clear();
					lcd_set_cursor(0, 0);
					lcd_print("Saque...");
					estado = ESTADO_PROCESSANDO;
					} else if (tecla == '3') {
					lcd_clear();
					lcd_set_cursor(0, 0);
					lcd_print("Deposito...");
					estado = ESTADO_PROCESSANDO;
					} else if (tecla == '#') {
					lcd_clear();
					lcd_set_cursor(0, 0);
					lcd_print("Saindo...");
					delay_ms(2000);

					lcd_clear();
					lcd_set_cursor(0, 0);
					lcd_print("Usuario:");

					estado = ESTADO_LOGIN_USUARIO;
				}
			}
			break;

			// ?? Placeholder para processamento futuro
			case ESTADO_PROCESSANDO:
			delay_ms(2000); // Simula tempo de operação
			lcd_clear();
			lcd_set_cursor(0, 0);
			lcd_print("Operacao concluida");
			delay_ms(2000);

			lcd_clear();
			lcd_set_cursor(0, 0);
			lcd_print("1-Saldo 2-Saque");
			lcd_set_cursor(1, 0);
			lcd_print("3-Deposito #-Sair");

			estado = ESTADO_MENU;
			break;

			default:
			estado = ESTADO_LOGIN_USUARIO;
			break;
		}
	}
}
