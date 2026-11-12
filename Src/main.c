#include <stdint.h>
#include <string.h>

#define MODE_IDLE 0U // нет связи
#define MODE_LINK 1U // связь установлена
#define MODE_TRANSFER 2U // идёт передача данных

#define RCC_APB1ENR (*(volatile uint32_t*)0x40023840U)
#define RCC_AHB1ENR (*(volatile uint32_t*)0x40023830U)

#define GPIOA_MODER (*(volatile uint32_t*)0x40020000U)

#define GPIOA_AFRL (*(volatile uint32_t*)0x40020020U)

#define USART2_BRR (*(volatile uint32_t*)0x40004408U)
#define USART2_CR1 (*(volatile uint32_t*)0x4000440CU)

#define USART2_SR  (*(volatile uint32_t*)0x40004400U)
#define USART2_DR  (*(volatile uint32_t*)0x40004404U)

#define GPIOA_ODR (*(volatile uint32_t*)0x40020014U)

#define NVIC_ISER1 (*(volatile uint32_t*)0xE000E104U)

volatile char buf[32];
volatile uint32_t len = 0;
volatile uint32_t cmd_ready = 0;

void uart_send_char(char c) {
	while ((USART2_SR & (1U << 7)) == 0) {

	}

	USART2_DR = c;
}

void uart_send_string(const char *s) {

	while (*s != '\0') {
		uart_send_char(*s);
		s++;
	}

}

void USART2_IRQHandler(void) {
	if ((USART2_SR & (1U << 5)) != 0) {

		char c = (char) USART2_DR;
		uart_send_char(c);

		if (c == '\r') {
			buf[len] = '\0';
			cmd_ready = 1;

		}

		else if (c == 127 || c == '\b') {
			if (len > 0) {
				len--;
			}
		}

		else if (len < 31) {
			buf[len] = c;
			len++;

		}
	}
}

int main(void) {

	RCC_APB1ENR |= (1U << 17);
	RCC_AHB1ENR |= (1U << 0);

	GPIOA_MODER &= ~(3U << 4);
	GPIOA_MODER |= (2U << 4);

	GPIOA_AFRL &= ~(0xFU << 8);
	GPIOA_AFRL |= (7U << 8);

	GPIOA_MODER &= ~(3U << 6);
	GPIOA_MODER |= (2U << 6);

	GPIOA_AFRL &= ~(0xFU << 12);
	GPIOA_AFRL |= (7U << 12);

	USART2_BRR = 139U;

	USART2_CR1 |= (1U << 3);
	USART2_CR1 |= (1U << 13);
	USART2_CR1 |= (1U << 2);

	USART2_CR1 |= (1U << 5);
	NVIC_ISER1 |= (1U << 6);

	GPIOA_MODER &= ~(3U << 10);
	GPIOA_MODER |= (1U << 10);

	uint32_t mode = MODE_IDLE;

	while (1) {
		if (cmd_ready == 1) {
			char cmd[32];
			uint32_t i = 0;
			while (buf[i] != '\0') {
				cmd[i] = buf[i];
				i++;
			}
			cmd[i] = '\0';

			len = 0;
			cmd_ready = 0;

			uart_send_char('\n');
			if (mode == MODE_IDLE) {
				if (strcmp(cmd, "CONNECT") == 0) {
					mode = MODE_LINK;
					uart_send_string("OK: link established\r\n");
				} else {
					uart_send_string("No link. Send CONNECT\r\n");
				}
			} else if (mode == MODE_LINK) {
				if (strcmp(cmd, "START") == 0) {
					mode = MODE_TRANSFER;
					uart_send_string("OK: data transfer started\r\n");
				} else {
					uart_send_string("Unknown command \r\n");
				}
			}

			else if (mode == MODE_TRANSFER) {
				if (strcmp(cmd, "END") == 0) {

					for (uint32_t k = 0; k < 10; k++) {
						GPIOA_ODR ^= (1U << 5);
						for (volatile uint32_t d = 0; d < 100000; d++)
							;
					}
					GPIOA_ODR &= ~(1U << 5);
					uart_send_string("OK: session closed\r\n");
					mode = MODE_IDLE;
				}

				else {
					uart_send_string("Received: ");
					uart_send_string(cmd); //
					uart_send_string("\r\n");
				}
			}
		}

		if (mode == MODE_LINK) {
			GPIOA_ODR ^= (1U << 5);
			for (volatile uint32_t d = 0; d < 1000000; d++)
				;
		} else if (mode == MODE_TRANSFER) {
			GPIOA_ODR ^= (1U << 5);
			for (volatile uint32_t d = 0; d < 150000; d++)
				;
		}

	}

}
