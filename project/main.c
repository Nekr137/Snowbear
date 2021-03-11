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

static volatile uint32_t msTick;


// Using the PC13 LED as a debugger tool
static uint32_t isPC13On = 0;
void Blink(void);
void BlinkingTool_Config(void);


// USART1
static uint16_t usartBuf[256];
static uint32_t usartBufIdx = 0;

void USART1_Config(void);
void USART1_SendCharacter(volatile uint16_t iData);
void USART1_SendSTR(volatile uint16_t* iStr);
void USART1_EraseBuffer(void);

// PWM
void PWM_Config(void);
void PWM_SetValue(const uint16_t iValue);


// Interruptions
void SysTick_Handler(void) {
  msTick++;
}

void USART1_IRQHandler(void) {
  if (USART1 -> SR & USART_SR_RXNE) {
    usartBuf[usartBufIdx++] = USART1->DR;
  }
}


int main() {

  SysTick_Config(SystemCoreClock/1000);
  BlinkingTool_Config();
  USART1_Config();
  PWM_Config();

  while(1) {

    // sent buffer to the usart1
    if (*usartBuf) {
      USART1_SendSTR(usartBuf);
      USART1_EraseBuffer();
    }

    // pwm test
    PWM_SetValue(msTick & 0xFFFF);

    // blinking
    if (msTick>1000) {
      msTick = 0;
      Blink();
    }
  }
}

// ==============================================

void USART1_EraseBuffer(void) {
  *usartBuf = '\0';
  usartBufIdx = 0;
}

void USART1_SendSTR(volatile uint16_t* iStr) {
  volatile uint16_t a;
  while (1) {
    a = *iStr;
    if(a) {
      USART1_SendCharacter(a);
    } else {
      break;
    }
    iStr++;
  } 
}

void USART1_SendCharacter(volatile uint16_t iData) {
  while(!(USART1->SR & USART_SR_TC)); //waiting for transmission complete
  USART1->DR = iData;
}

void USART1_Config(void) {

  RCC->APB2ENR |=RCC_APB2ENR_USART1EN; // USART1 clock enable
  USART1->BRR = 0xea6; // bodrate for 19200 on 72Mhz

  USART1->CR1 |= 
    USART_CR1_UE |    // USART1 on
    USART_CR1_TE |    // TX on
    USART_CR1_RE |    // RX on
    USART_CR1_RXNEIE; // interruption on

  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN; // GPIOA clock ON. Alter function clock ON
  GPIOA->CRH &= ~GPIO_CRH_CNF9;   // clear CNF bit 9
  GPIOA->CRH |= GPIO_CRH_CNF9_1;  // set CNF bit 9 to 10 - AFIO Push-Pull
  GPIOA->CRH |= GPIO_CRH_MODE9_0; // set MODE bit 9 to Mode 01 = 10MHz

  GPIOA->CRH &= ~GPIO_CRH_CNF10;  // clear CNF bit 9
  GPIOA->CRH |= GPIO_CRH_CNF10_0; // set CNF bit 9 to 01 = HiZ
  GPIOA->CRH &= ~GPIO_CRH_MODE10; // set MODE bit 9 to Mode 01 = 10MHz

  NVIC_EnableIRQ(USART1_IRQn);
  __enable_irq();
}

// ==============================================

void Blink() {
  GPIOC->BSRR |= isPC13On == 0 ? GPIO_BSRR_BS13 : GPIO_BSRR_BR13;
  isPC13On = !isPC13On;
}

void BlinkingTool_Config() {
  RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
  GPIOC->CRH |= GPIO_CRH_MODE13_0;
  GPIOC->CRH &= ~GPIO_CRH_CNF13;
  isPC13On = 0;
}

// ==============================================

void PWM_Config() {

  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

  GPIOA->CRL &= ~GPIO_CRL_CNF1;
  GPIOA->CRL |= GPIO_CRL_CNF1_1;

  GPIOA->CRL &= ~GPIO_CRL_MODE1;
  GPIOA->CRL |= GPIO_CRL_MODE1_1;

  TIM2->CR1  = 0;
  TIM2->CR2  = 0;
  TIM2->DIER = 0;
  
  TIM2->CCMR1 = TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2;
  
  TIM2->PSC  = 80-1;
  TIM2->ARR  = 1000;
  TIM2->CCR2 = 40;
  TIM2->CCER = TIM_CCER_CC2E;
  TIM2->EGR  = TIM_EGR_UG;
  TIM2->CR1 |= TIM_CR1_CEN;
}

void PWM_SetValue(const uint16_t iValue) {
  while(!TIM2->SR || !TIM_SR_UIF) {
  }
  TIM2->SR |= ~TIM_SR_UIF;
  TIM2->CCR2 = iValue;
}

