// ST-LINK V2
//  _____
// | * * | rst SWDIO
// | * * | GND GND
// | * * | swim SWCLK
// | * * | 3.3 3.3
// | * * | 5.5 5.5
//  -----

// stm32f103c8t6
//  ___
// | 
// | * - GND
// | * - SWCLK (PA14)
// | * - SWDIO (PA13)
// | * - 3.3
// |___

// PC13 - LED

// 5V tolerance:
// PA13, PA14
// 3V tolerance:
// PC13




#include "stm32f10x.h"

static uint32_t msTick;

//static uint8_t usartBuf[256];
//static uint32_t usartBufIdx = 0;

void USART_Config(void);
void USART_SendCharacter(uint8_t iData);
void USART_SendSTR(uint8_t* iStr);

void SysTick_Handler(void) {
	msTick++;
}
//void USART1_IRQHandler(void) {
//	if (USART1 -> ISR & USART_ISR_RXNE) {
//		usartBuf[usartBufIdx++] = USART1->RDR;	
//	}
//}

int main() {
	static uint32_t flag; 
	
	SysTick_Config(SystemCoreClock/1000);
	//SystemInit();
	
	USART_Config();
	
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	
	GPIOC->CRH |= GPIO_CRH_MODE13_0;
	GPIOC->CRH &= ~GPIO_CRH_CNF13;
	flag = 0;
	
	while(1) {
		if (msTick>500) {
			msTick = 0;
			if (flag== 0) {
				flag = 1;
				GPIOC->BSRR |= GPIO_BSRR_BS13;
				USART1->DR = 10;	
			}
			else {
				flag = 0;
				GPIOC->BSRR |= GPIO_BSRR_BR13;
				USART1->DR = 1;	
			}
		}
	}	
}
//void USART_SendSTR(uint8_t* iStr) {
//	uint8_t a;   
//  while (1) {
//		a = *iStr;
//    if(a) {
//			USART_SendCharacter(a);
//    } else {
//			break;
//		}
//    iStr++;
//	} 
//}

//void USART_SendCharacter(uint8_t iData) {
//	while(!(USART1->ISR & USART_ISR_TC)); //waiting for transmission complete
//	USART1->TDR = iData;
//}

void USART_Config(void) {
	
	// USART1 clock enable
	RCC -> APB2ENR |=RCC_APB2ENR_USART1EN;
	
	
	//0x1A0B;//48000000/12;//480000/96;//0x341;//0x1601;//0x9C4;//0x34D;//480000/96;//1388;//0x681; //0x681;//0x9C4;// 0x681; 0x341h 0x64
	//USART1 -> BRR = 0x341; 
	//USART1->BRR = 0xEA6;
	
	// Bodrate for 19200 on 72Mhz
	//USART1->BRR = 0xEA6;
	USART1->BRR = 0xea6;
	
	// USART1 ON, TX ON, RX ON
	USART1->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;

	// GPIOA clock ON. Alter function clock ON
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;
	// Clear CNF bit 9
	GPIOA->CRH	&= ~GPIO_CRH_CNF9;
	// Set CNF bit 9 to 10 - AFIO Push-Pull
	GPIOA->CRH	|= GPIO_CRH_CNF9_1;
	// Set MODE bit 9 to Mode 01 = 10MHz
	GPIOA->CRH	|= GPIO_CRH_MODE9_0;
	
	
	// Clear CNF bit 9
	GPIOA->CRH	&= ~GPIO_CRH_CNF10;
	// Set CNF bit 9 to 01 = HiZ
	GPIOA->CRH	|= GPIO_CRH_CNF10_0;
	// Set MODE bit 9 to Mode 01 = 10MHz
	GPIOA->CRH	&= ~GPIO_CRH_MODE10;
	
	// test
	// USART1->DR = 10;	
}


