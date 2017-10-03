/*----------------------------------------------------------------------------
* Name:    Blinky.c
* Purpose: LED Flasher
* Note(s): possible defines set in "options for target - C/C++ - Define"
*            __USE_LCD   - enable Output on LCD
*----------------------------------------------------------------------------
* This file is part of the uVision/ARM development tools.
* This software may only be used under the terms of a valid, current,
* end user licence from KEIL for a compatible version of KEIL software
* development tools. Nothing else gives you the right to use this software.
*
* This software is supplied "AS IS" without warranties of any kind.
*
* Copyright (c) 2008-2011 Keil - An ARM Company. All rights reserved.
*----------------------------------------------------------------------------*/
//Limits 320x240

#include <stdio.h>
#include "LPC17xx.H"                         /* LPC17xx definitions */
#include <RTL.h>
#include "GLCD.h"
#include "Serial.h"
#include "LED.h"
#include "rit.h"
#include "ADC.h"
#include <stdlib.h>

#define __FI        1                       /* Font index 16x24               */
#define BG Black
#define FG Green
#define LZ Red
#define AL Purple
#define PRIORITY 10
#define HEIGHT_OF_SCREEN 320
#define WIDTH_OF_SCREEN 240
#define WIDTH_OF_SHOOTER 22
#define WIDTH_OF_ALIEN 22
#define max(x,y) ((((x)-(y))<((y)-(x))) ? ((x)-(y)) : ((y)-(x)))

typedef struct{
    uint16_t x,y;
}laser_t;

typedef struct{
    laser_t laser;
    unsigned char priority;
} laser_task_parameters_t;

typedef struct{
    uint16_t x,y;
    uint16_t speed;
    uint16_t height, width;
}alien_t;

typedef struct{
    alien_t alien;
    unsigned char priority;
} alien_task_parameters_t;

typedef struct{
    alien_t* alien;
    laser_t* laser;
    unsigned char priority;
}collision_task_paramaters_t;

//----------------------------------------------------Bitmaps---------------------------------------------
unsigned short Shooter[] = {0x00,0x00,0x6a,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x6a,0x00,0x00
    ,0x00,0x93,0x12,0x13,0x6e,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x6e,0x13,0x12,0x93,0x00
    ,0xdb,0x26,0x13,0x1f,0x02,0x00,0xdb,0xdb,0xdb,0xdb,0xdb,0xdb,0xdb,0xdb,0xdb,0xdb,0x00,0x02,0x1f,0x13,0x26,0xdb
    ,0x01,0x03,0x17,0x1f,0x03,0x01,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x01,0x03,0x1f,0x17,0x03,0x01
    ,0x01,0x03,0x17,0x1f,0x03,0x01,0x13,0x13,0x13,0x1f,0x1f,0x1f,0x1f,0x13,0x13,0x13,0x01,0x03,0x1f,0x17,0x03,0x01
    ,0x01,0x03,0x0b,0x0f,0x03,0x01,0x01,0x06,0x13,0x13,0x13,0x13,0x13,0x13,0x06,0x01,0x01,0x03,0x0f,0x0b,0x03,0x01
    ,0x01,0x03,0x0b,0x0f,0x03,0x01,0x6e,0x4e,0x07,0x13,0x0f,0x0f,0x13,0x07,0x4e,0x6e,0x01,0x03,0x0f,0x0b,0x03,0x01
    ,0x01,0x03,0x0b,0x0f,0x03,0x01,0x00,0xb3,0x01,0x0b,0x0f,0x0f,0x0b,0x01,0xb3,0x00,0x01,0x03,0x0f,0x0b,0x03,0x01
    ,0x01,0x03,0x0b,0x0f,0x03,0x01,0x00,0x93,0x01,0x03,0x0f,0x0f,0x03,0x01,0x93,0x00,0x01,0x03,0x0f,0x0b,0x03,0x01
    ,0x00,0x4a,0x0b,0x0f,0x01,0x00,0x00,0x93,0x01,0x03,0x0f,0x0f,0x03,0x01,0xb7,0x00,0x00,0x01,0x0f,0x0b,0x4a,0x00
    ,0x00,0xdb,0x2a,0x06,0xdb,0x00,0x4a,0x26,0x03,0x03,0x0f,0x0f,0x03,0x03,0x26,0x4a,0x00,0xdb,0x06,0x2a,0xdb,0x00
    ,0x00,0x00,0x4a,0x02,0x00,0x00,0x02,0x02,0x07,0x07,0x0f,0x0f,0x07,0x07,0x02,0x02,0x00,0x00,0x02,0x4a,0x00,0x00
    ,0x00,0x00,0x6e,0x4a,0x00,0x00,0x02,0x06,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x06,0x02,0x00,0x00,0x4a,0x6e,0x00,0x00
    ,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x02,0x03,0x03,0x03,0x03,0x03,0x03,0x02,0x01,0x00,0x00,0x00,0x00,0x00,0x00
    ,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x93,0x01,0x1f,0x0b,0x0b,0x1f,0x01,0x93,0x00,0x00,0x00,0x00,0x00,0x00,0x00
    ,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x93,0x01,0x13,0x03,0x03,0x13,0x01,0x93,0x00,0x00,0x00,0x00,0x00,0x00,0x00
    ,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x93,0x01,0x03,0x03,0x03,0x03,0x01,0x93,0x00,0x00,0x00,0x00,0x00,0x00,0x00
    ,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x1b,0x1b,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
    ,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x1b,0x1b,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
    ,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xdb,0x2a,0x2a,0xdb,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
    ,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x26,0x26,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x26,0x26,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

unsigned short bg[] = 		 {BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,
    BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,
    BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,
    BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,
    BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,
    BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,
    BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,
    BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,
    BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,
    BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,
    BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,
    BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,
    BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,
    BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,
    BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,
    BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,
    BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,
    BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,
    BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,
    BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,
    BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,
    BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,
    BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,
BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG};

unsigned short lazer[]=   { LZ,LZ,
    LZ,LZ,
    LZ,LZ,
    LZ,LZ,
    LZ,LZ,
    LZ,LZ,
    LZ,LZ,
    LZ,LZ,
    LZ,LZ,
LZ,LZ};

unsigned short laserBG[]=   {BG,BG,
    BG,BG,
    BG,BG,
    BG,BG,
    BG,BG,
    BG,BG,
    BG,BG,
    BG,BG,
    BG,BG,
BG,BG};

unsigned short Alien[] = 	 {0x00,0x00,0x00,0x00,0x00,0x00,0x92,0x92,0x92,0xb6,0x00,0x00,0xb6,0x92,0x92,0x92,0x00,0x00,0x00,0x00,0x00,0x00
    ,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00
    ,0x92,0xdb,0x00,0x00,0xb6,0xdb,0x24,0x24,0x49,0x49,0x00,0x00,0x49,0x49,0x24,0x49,0xdb,0x92,0x00,0x24,0xdb,0x6d
    ,0xdb,0xff,0x00,0x00,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xdb,0x00,0x49,0xff,0x92
    ,0xb6,0xff,0x00,0x00,0xff,0xff,0xdb,0xdb,0xdb,0xdb,0xdb,0xdb,0xdb,0xdb,0xdb,0xdb,0xff,0xdb,0x00,0x49,0xff,0x92
    ,0xb6,0xff,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xdb,0x00,0x24,0xff,0x92
    ,0xb6,0xff,0xff,0xdb,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xdb,0xff,0xff,0x92
    ,0xdb,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xb6
    ,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x24,0xff,0xff,0xff,0xb6,0x00,0x00
    ,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0xff,0xff,0xff,0xdb,0x00,0x00
    ,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xdb,0x00,0x00,0x00,0x00
    ,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00
    ,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xdb,0x00,0x00,0x00,0x00,0x00,0x00
    ,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00
    ,0x00,0x00,0x00,0x00,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0x00,0x00,0x00,0x00
    ,0x00,0x00,0x00,0x00,0xdb,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xb6,0x00,0x00,0x00,0x00
};


unsigned short AlienBG[] = {BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,
    BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,
    BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,
    BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,
    BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,
    BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,
    BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,
    BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,
    BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,
    BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,
    BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,
    BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,
    BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,
    BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,
    BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,
BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG,BG};
char text[10];
/* Import external variables from IRQ.c file                                  */
extern uint8_t  buttonPress;
volatile uint8_t laserExist, alienExist =0, collision, level, GAMEOVER, counter = 0, debugger; //DEBUGGING: counter
int ShooterTaskID, AlienTaskID, LaserTaskID;
OS_SEM respawn, boom;
volatile alien_t* alien1;
volatile laser_t* laser1;
volatile laser_task_parameters_t laser_param;
volatile alien_task_parameters_t alien_param;
//Score keeping functions
void decimalToBinary(uint16_t decimal, uint8_t* binary){
    uint8_t i =1;
    uint16_t quotient = decimal;
    
    while(quotient!=0){
        binary[i++] = quotient%2;
        quotient = quotient/2;
    }
}
void updateScore(uint16_t score){
    uint8_t scoreBinary[8]={0}, i;
    decimalToBinary(score,scoreBinary);
    LED_Out(0);
    for(i=7;i>0;i--){
        if(scoreBinary[i]!=0){
            LED_On(abs(i-8));
        }
    }
}


//---------------------Laser Task---------------
__task void laser_task( void* void_ptr){
    uint8_t shoot, speed =10;
    LaserTaskID = os_tsk_self();
		os_sem_wait(&boom, 0xffff); //JENNY2
    while(1){
        if(laserExist){
            shoot = 1;
            GLCD_Bitmap ((laser_param.laser).x, (laser_param.laser).y, 2, 10,(unsigned char*)lazer);
            while(shoot){
                os_sem_wait(&mutex,0xffff);
								os_dly_wait(1); //JENNY2
                GLCD_Bitmap ((laser_param.laser).x,(laser_param.laser).y, 2, 10,(unsigned char*)laserBG);
                (laser_param.laser).y -=speed ;
                GLCD_Bitmap ((laser_param.laser).x,(laser_param.laser).y, 2, 10,(unsigned char*)lazer);
                
							if(((laser_param.laser).y < 5) || !laserExist){
                    shoot=0;
                    GLCD_Bitmap ((laser_param.laser).x,(laser_param.laser).y, 2, 10,(unsigned char*)laserBG);
                    laserExist=0;
                    laser1=NULL;
										os_sem_send(&boom); //JENNY2
										os_tsk_delete_self();
                }
            }
        }
        
    }
}

//Set up and start Laser task
void fireLaser(uint16_t position){
    laser_param.priority = PRIORITY;
    (laser_param.laser).x = position;
    (laser_param.laser).y = HEIGHT_OF_SCREEN - WIDTH_OF_SHOOTER - 10;
    laserExist=1;
}

//Set up Alien task
void spawnAlien(uint16_t speed){
		os_sem_wait(&respawn, 0xeeee);
    alien_param.priority = PRIORITY;
    (alien_param.alien).width = WIDTH_OF_ALIEN;
    (alien_param.alien).height = 16;
	  (alien_param.alien).x = rand() % WIDTH_OF_SCREEN;
    (alien_param.alien).y = alien_param.alien.height;
		(alien_param.alien).speed = speed;
    alienExist=1;
		os_sem_send(&respawn);
}

//---------------------Alien Task---------------
__task void alien_task (void* void_ptr){
    uint16_t i;
    AlienTaskID = os_tsk_self();
		os_sem_wait(&respawn, 0xeeee);
    while(1){
        if(alienExist){
						counter++;//DEBUGGING
						sprintf(text,"%d", counter);  				/* format text for print out     */
            GLCD_DisplayString(5,  19, __FI,  (unsigned char *)text);
					
            alien1 = &(alien_param.alien);
            GLCD_Bitmap ((alien_param.alien).x, (alien_param.alien).y , (alien_param.alien).width, (alien_param.alien).height, (unsigned char*)Alien);
            while(alienExist){ //CHANGE: move to the if statement below
                os_dly_wait(100 / alien_param.alien.speed);
                GLCD_Bitmap((alien_param.alien).x, (alien_param.alien).y, (alien_param.alien).width, (alien_param.alien).height, (unsigned char*)AlienBG);
                (alien_param.alien).y += 10;
                GLCD_Bitmap ((alien_param.alien).x, (alien_param.alien).y , (alien_param.alien).width, (alien_param.alien).height, (unsigned char*)Alien);
                
                if((alien_param.alien).y > (HEIGHT_OF_SCREEN - 25 - alien_param.alien.height)){
                    alienExist = 0;
                    for (i = 0; i < 4; i++){ //Flash to indicate death
                        GLCD_Bitmap((alien_param.alien).x, (alien_param.alien).y, (alien_param.alien).width, (alien_param.alien).height, (unsigned char*)AlienBG);
                        os_dly_wait(3);
                        GLCD_Bitmap ((alien_param.alien).x, (alien_param.alien).y , (alien_param.alien).width, (alien_param.alien).height, (unsigned char*)Alien);
                        os_dly_wait(3);
                    }
										GLCD_Bitmap((alien_param.alien).x, (alien_param.alien).y, (alien_param.alien).width, (alien_param.alien).height, (unsigned char*)AlienBG);
                    GAMEOVER=1;
                    os_tsk_delete(ShooterTaskID);
                    os_tsk_delete_self();
                }
                //         sprintf(text, "%d", ((task_param->alien).y));  				/* format text for print out     */
                // 				GLCD_DisplayString(5,  9, __FI,  (unsigned char *)text);
            }
        }
				
    }
    
}


//Movement of Character based off potentiometer
__task void moveShooter_task( void* void_ptr){
    uint32_t ad_avg = 0;
    uint16_t* ad_val = (uint16_t*) void_ptr;
    uint16_t ad_val_ = 0xFFFF, tolerance = 10;
    uint16_t pot_left_edge = 0x0DAD;
    ShooterTaskID = os_tsk_self();
    while(1){
        /* AD converter input                                                     */
        if (AD_done) {                           /* If conversion has finished    */
            AD_done = 0;
            ad_avg += AD_last << 8;                /* Add AD value to averaging     */
            ad_avg ++;
            if ((ad_avg & 0xFF) == 0x10) {         /* average over 16 values        */
                *ad_val = (ad_avg >> 8) >> 4;         /* average divided by 16				*/
                if (*ad_val > pot_left_edge)
                    *ad_val = pot_left_edge;
                ad_avg = 0;
            }
        }
        
        if (abs(*ad_val-ad_val_)>tolerance) {                  /* AD value changed   	*/
            GLCD_Bitmap (220-(ad_val_>>4), HEIGHT_OF_SCREEN - WIDTH_OF_SHOOTER, WIDTH_OF_SHOOTER, WIDTH_OF_SHOOTER, (unsigned char*)bg);
            ad_val_ = *ad_val;
            GLCD_Bitmap (220-(*ad_val>>4), HEIGHT_OF_SCREEN - WIDTH_OF_SHOOTER, WIDTH_OF_SHOOTER, WIDTH_OF_SHOOTER, (unsigned char*)Shooter);
        }
    }
}

__task void collision_task (void * void_ptr) {
    uint8_t i;
    while(1){
        if(laserExist && alienExist){
            if(((laser_param.laser.y-alien_param.alien.y) < 45) && ((alien_param.alien.x)<(laser_param.laser.x)) && ((laser_param.laser.x)<(alien_param.alien.x+alien_param.alien.width))){
                os_sem_wait(&boom, 0xeeee);
								collision = 1;
                laserExist = 0;
                alienExist = 0;
                GLCD_Bitmap ((laser_param.laser).x,(laser_param.laser).y, 2, 10,(unsigned char*)laserBG);
                for (i = 0; i < 3; i++){ //Flash to indicate death
                    GLCD_Bitmap(alien_param.alien.x, alien_param.alien.y, alien_param.alien.width, alien_param.alien.height, (unsigned char*)AlienBG);
                    os_dly_wait(3);
									  GLCD_Bitmap(alien_param.alien.x, alien_param.alien.y, alien_param.alien.width, alien_param.alien.height, (unsigned char*)Alien);
                    os_dly_wait(3);
                }
								GLCD_Bitmap(alien_param.alien.x, alien_param.alien.y, alien_param.alien.width, alien_param.alien.height, (unsigned char*)AlienBG);
                level++;
                updateScore(level);
								os_dly_wait(50);
// 								debugger = counter + 1; //DEBUGGING
								counter++; 
								os_sem_send(&respawn);
								os_sem_send(&boom);
                spawnAlien(level + 3);
            } else {
								os_sem_send(&boom); //JENNY2
						}
        } 
    }
}


__task void base_task( void ) {
    uint16_t ad_val =0, score = 0;
    collision_task_paramaters_t collision_param;
    level = 1, GAMEOVER=0;
    os_tsk_prio_self(PRIORITY);
    os_sem_init(&mutex,1);
		os_sem_init(&respawn,1);
    os_tsk_create_ex( moveShooter_task, PRIORITY, &ad_val );
    spawnAlien(level);
    collision_param.priority = PRIORITY;
//     os_tsk_create_ex(laser_task, PRIORITY, NULL );
    os_tsk_create_ex(alien_task, PRIORITY , NULL);
    os_tsk_create_ex(collision_task, collision_param.priority , NULL);
    
    
    while (1) {                                /* Loop forever                  */
        //Trigger at interupt
        if(buttonPress){
            os_dly_wait(10);
            buttonPress=0;
            if(!laserExist){
                fireLaser(220-(ad_val>>4)+11);
							  os_tsk_create_ex(laser_task, PRIORITY, NULL );
            }
        }
        if(GAMEOVER){
            sprintf(text,"GAME OVER");  				/* format text for print out     */
            GLCD_DisplayString(5,  19, __FI,  (unsigned char *)text);
        }
    }
}




/*----------------------------------------------------------------------------
Main Program
*----------------------------------------------------------------------------*/
int main (void) {
    LED_Init();                                /* LED Initialization            */
    SER_Init();                                /* UART Initialization           */
    ADC_Init();                                /* ADC Initialization            */
    Button_init();
    #ifdef __USE_LCD
    GLCD_Init();                               /* Initialize graphical LCD      */
    GLCD_Clear(Black);                         /* Clear graphical LCD display   */
    GLCD_SetBackColor(Black);
    GLCD_SetTextColor(White);
    #endif
    
    //SysTick_Config(SystemCoreClock/100);       /* Generate interrupt each 10 ms */
    rit_init();
    collision = 0;
    os_sys_init( base_task );
    while(1){
        
    }
}
