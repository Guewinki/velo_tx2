//my tst brd:
/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __MY_BOARD_H
#define __MY_BOARD_H

//PB5,PB6,PB7 are dead
//PD5,PD4,PF0 possibly dead,do not use!

/* MACROs for SET, RESET or TOGGLE Output port */

#define GPIO_HIGH(a,b) 		a->ODR|=b
#define GPIO_LOW(a,b)		a->ODR&=~b
#define GPIO_TOGGLE(a,b) 	a->ODR^=b

#define BTN_PORT	GPIOC
#define BTN_PIN		GPIO_Pin_1


#define LED1_PORT	GPIOE     //green
#define LED1_PIN		GPIO_Pin_7
#define LED2_PORT		GPIOC       //blue
#define LED2_PIN		GPIO_Pin_7	//GPIOC->ODR addr 0x500a

//SPI hardware deflt NSS-PB4 SCK-PB5 MOSI-PB6 MISO-PB7;NSS-PC5,SCK-PC6,MOSI-PA3,MISO-PA2
#define SPI_SCK    GPIOC
#define SPI_SCK_PIN   GPIO_Pin_6
#define SPI_MOSI    GPIOA
#define SPI_MOSI_PIN  GPIO_Pin_3
#define SPI_MISO    GPIOA					//port 
#define SPI_MISO_PIN  GPIO_Pin_2

#define NRF_CSN         GPIOB        //chip select,active low
#define NRF_CSN_PIN     GPIO_Pin_3
#define NRF_CE          GPIOB        //NRF tx,rx activation
#define NRF_CE_PIN      GPIO_Pin_4

#endif


/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
/*
//area below doesn't use in this project
//SPI hardware
#define SPI_SCK    GPIOB
#define SPI_SCK_PIN   GPIO_Pin_5
#define SPI_MOSI    GPIOB
#define SPI_MOSI_PIN  GPIO_Pin_6
#define SPI_MISO    GPIOB
#define SPI_MISO_PIN  GPIO_Pin_7

//C55 as in LCD_cellphone.c
#define SIEM_CS  GPIOC								//RED: V+ 3v3
#define SIEM_CS_PIN  GPIO_Pin_3       //RED GREEN: CS
#define SIEM_SCK    GPIOC
#define SIEM_SCK_PIN   GPIO_Pin_6    //BLUE: clock
#define SIEM_MOSI    GPIOC
#define SIEM_MOSI_PIN  GPIO_Pin_5    //GREEN: data out
#define SIEM_DC GPIOC
#define SIEM_DC_PIN GPIO_Pin_4		//YELLOW: data/cmd switcher
#define SIEM_RES GPIOC 			    
#define SIEM_RES_PIN GPIO_Pin_2 //METAL: reset

//radio rec
#define RFREC    GPIOD					//radio signal receive port
#define RFREC_PIN  GPIO_Pin_7

//I2C as in i2c.c
#define I2C_SDA    GPIOD					//i2c data open drain
#define I2C_SDA_PIN  GPIO_Pin_6
#define I2C_SCL    GPIOD					//i2c clock open drain
#define I2C_SCL_PIN  GPIO_Pin_5

//sig SFM-1440 buzzer
#define SIG    GPIOD					//alarm signal
#define SIG_PIN  GPIO_Pin_1		//white to PD1,black to GND

*/
