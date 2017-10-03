/*----------------------------------------------------------------------------
 * Name:    IRQ.c
 * Purpose: IRQ Handler
 * Note(s):
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2011 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include "LPC17xx.H"                         /* LPC17xx definitions           */
#include "LED.h"
#include "ADC.h"

/*----------------------------------------------------------------------------
  Systick Interrupt Handler
  SysTick interrupt happens every 10 ms
 *----------------------------------------------------------------------------*/
 uint8_t buttonPress;



void Button_init () {
	LPC_PINCON->PINSEL4 &= ~( 3 << 20 );
	LPC_GPIO2->FIODIR &= ~( 1 << 10 );
	LPC_GPIOINT->IO2IntEnF |= ( 1 << 10 );
	NVIC_EnableIRQ( EINT3_IRQn );
}

void EINT3_IRQHandler(void) {
	LPC_GPIOINT->IO2IntClr |= (1 << 10);
	buttonPress=1;
}

