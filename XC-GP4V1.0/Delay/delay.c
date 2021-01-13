#include "delay.h"

void delay_ms(uint32_t ms)
{ /// delay in ms

	RCC->APB2ENR |= RCC_APB2ENR_TIM16EN; //
	TIM16->PSC = 48 - 1;								 //
																			 //
	TIM16->ARR = 0x03E8;								 //
	TIM16->CR1 = 0x0084;								 //

	TIM16->EGR |= TIM_EGR_UG;	//
	TIM16->CR1 |= TIM_CR1_CEN; //
	while (ms > 0)
	{
		while ((TIM16->SR & TIM_SR_UIF) == 0x0000)
			; //

		TIM16->SR &= ~TIM_SR_UIF; //
		ms--;
	}
	TIM16->CR1 &= ~TIM_CR1_CEN;						//
	RCC->APB2ENR &= ~RCC_APB2ENR_TIM16EN; //
}

void delay_us(uint32_t us)
{ /// delay in us

	RCC->APB2ENR |= RCC_APB2ENR_TIM16EN; //
	TIM16->PSC = 0x0001 - 0x0001;				 //
//
	TIM16->ARR = 0x0030;								 //
	TIM16->CR1 = 0x0084;								 //
																			 //

	TIM16->EGR |= TIM_EGR_UG;	//
	TIM16->CR1 |= TIM_CR1_CEN; //
	while (us > 0)
	{
		while ((TIM16->SR & TIM_SR_UIF) == 0x0000)
			; //

		TIM16->SR &= ~TIM_SR_UIF; //
		us--;
	}
	TIM16->CR1 &= ~TIM_CR1_CEN;						//
	RCC->APB2ENR &= ~RCC_APB2ENR_TIM16EN; //
}
