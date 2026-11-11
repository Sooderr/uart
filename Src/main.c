#include <stdint.h>

#define RCC_APB1ENR (*(volatile uint32_t*)0x40023840U)
#define RCC_AHB1ENR (*(volatile uint32_t*)0x40023830U)

#define GPIOA_MODER (*(volatile uint32_t*)0x40020000U)

#define GPIOA_AFRL (*(volatile uint32_t*)0x40020020U)


#define USART2_BRR (*(volatile uint32_t*)0x40004408U)
#define USART2_CR1 (*(volatile uint32_t*)0x4000440CU)

#define USART2_SR  (*(volatile uint32_t*)0x40004400U)
#define USART2_DR  (*(volatile uint32_t*)0x40004404U)

#define GPIOA_ODR (*(volatile uint32_t*)0x40020014U)


void uart_send_char (char c)
{
	while((USART2_SR  & (1U << 7)) == 0)
	{

	}

	USART2_DR = c;
}

void uart_send_string (const char *s)
{

while(*s != '\0')
{
	uart_send_char(*s);
	s++;
}

}

char uart_read_char(void)
{
    while ((USART2_SR & (1U << 5)) == 0)
    {
    }
    return (char)USART2_DR;
}


int main(void)
{

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


	GPIOA_MODER &= ~(3U << 10);
	GPIOA_MODER |=  (1U << 10);

while(1)
{

char c = uart_read_char();
	if(c == '1')
	{
		GPIOA_ODR |= (1U << 5);
	}
	else if(c == '0')
	{
		GPIOA_ODR &= ~(1U << 5);
	}
}

}
