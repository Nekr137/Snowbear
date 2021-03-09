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

void SysTick_Handler(void) {
	msTick++;
}

int main() {
	static uint32_t flag; 
	
	SysTick_Config(SystemCoreClock/1000);
	
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	
	GPIOC->CRH |= GPIO_CRH_MODE13_0;
	GPIOC->CRH &= ~GPIO_CRH_CNF13;
	flag = 0;
	
	while(1) {
		if (msTick>50) {
			msTick = 0;
			if (flag== 0) {
				flag = 1;
				GPIOC->BSRR |= GPIO_BSRR_BS13;
			}
			else {
				flag = 0;
				GPIOC->BSRR |= GPIO_BSRR_BR13;
			}
		}
	}	
}

