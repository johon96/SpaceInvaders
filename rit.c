#include "rit.h"
#include "GLCD.h"

OS_SEM mutex;
//Increase speedLaser to slowdown/ speed up laser
int cntr, speedLaser=1;
extern uint8_t laserExist;
void rit_init(void)
{
		cntr = 0;
    LPC_SC->PCONP |= (1 << 16);               //Power Control for Peripherals register: power up RIT clock
    LPC_SC->PCLKSEL1 |= ((1 << 26) & (1 << 27));  //Peripheral clock selection: divide clock by 8 (run RIT clock by 12MHz)
    LPC_RIT->RICOUNTER = 0;               //set counter to zero
    LPC_RIT->RICOMPVAL = 100000000/1000;     //interrupt tick every second (clock at 100MHz)
    LPC_RIT->RICTRL |= (1 << 1);              // clear timer when counter reaches value
    LPC_RIT->RICTRL |= (1 << 3);              // enable timer
    
    //enable interrupt
    NVIC_SetPriority(RIT_IRQn, 31);
    NVIC_EnableIRQ(RIT_IRQn);
}


void RIT_IRQHandler(void)
{
	static unsigned long ticks = 0;
	static unsigned long shortTicks = 0;
  static unsigned long timetick;
  static unsigned int  leds = 0x01;
    //clear flag
    LPC_RIT->RICTRL |= 1; //write 1 to clear bit
		cntr++;
		ADC_StartCnv();
		if(laserExist){
				 isr_sem_send(&mutex);
		}
}
