/*
velo tx2
via SPI and NRF24L01
nrf,mydelay included in stm8l15x_conf.h
prepare
*/
//Includes ------------------------------------------------------------------*/

#include "stm8l15x.h"
#include "discover_board.h" //ok
#include "main.h"
#include "stdio.h"  //for virual com port
#include "mydelay.h"	//delay,holds all the programm
#include "nrf24l01p.h"  //NRF24L01 library

//defines see in main.h

//Global Variables
volatile unsigned char fl,btnState,btnDelay;		//flags
volatile unsigned char rxreg;	//receive byte for UART
unsigned char rxdata[NRF_PAYLOAD_LENGTH];  //array of rec data from UART usartRx: 0,1..5 cells
unsigned char rxcnt=0; //rec data bytes counter UART usartRx
unsigned char cmd;		//collected data from UART usartRx as command
//nrf address
uint8_t address[5] = { 0xE2, 0xE4, 0x23, 0xE4, 0x02 };	//nrf address

//Functions

//timer4: clock 2000000Hz/Prescaler(1-32768)or(2^0 - 2^15)/value ARR=Ftmr4
void tmr4_init(void){
	CLK->PCKENR1|=0x04; //enable TIM4 clock
  TIM4_DeInit();
  //TIM4_TimeBaseInit(TIM4_Prescaler_32768,100); // every 1.6sec
	//TIM4_TimeBaseInit(TIM4_Prescaler_32768,6); // every 98ms=almost 100ms
	TIM4_TimeBaseInit(TIM4_Prescaler_16384,1); // every almost 10ms
	//TIM4_TimeBaseInit(TIM4_Prescaler_1,1); // every xx us
	TIM4_SetCounter(0x00); //reset counter
  TIM4_ITConfig(TIM4_IT_Update,ENABLE);
  TIM4_Cmd(ENABLE);		//for button handle
	//TIM4_Cmd(DISABLE);
}
//blink :2000000(sysclock)/200(prescaler)=10000/100(Arr)=100Hz=10ms
//blink :2000000(sysclock)/200(prescaler)=10000/5(Arr)=2000Hz=0.5ms
//blink :2000000(sysclock)/200(prescaler)=10000/2(Arr)=5000Hz=0.2ms
//blink :2000000(sysclock)/200(prescaler)=10000/10000(Arr)=1Hz=1000ms
void tmr1_init(void){
	CLK->PCKENR2|=0x02;   //enable TIM1 clock
  TIM1->PSCRH=0;
  TIM1->PSCRL=199;  //prescaler
	TIM1->ARRH=(1000)>>8;  //ARR High
  TIM1->ARRL=(1000)& 0xff;   //ARR low
  TIM1->EGR|=TIM1_EGR_UG;	//update generation,reinit counter
  TIM1->CR1|=TIM1_CR1_URS;
  TIM1->CR1&=~TIM1_CR1_DIR;	//
  TIM1->IER|=TIM1_IER_UIE;
 	//TIM1->CR1|=TIM1_CR1_CEN;  //enable TMR1
	TIM1->CR1&=~TIM1_CR1_CEN;  //disable TMR1
}
void init(){
  //init
  //CLK->CKDIVR|=0x03;//sysclock prescaler sysclocksource/8	= 2Mhz by default
  //CLK->CKDIVR|=0x04;//sysclock prescaler sysclocksource/16 1Mhz
	GPIO_Init(LED1_PORT, LED1_PIN, GPIO_Mode_Out_PP_Low_Slow);
  GPIO_Init(LED2_PORT, LED2_PIN, GPIO_Mode_Out_PP_Low_Slow);
	GPIO_Init(BTN_PORT,BTN_PIN,GPIO_Mode_In_FL_No_IT);
	EXTI->CR1|=0x08;	//falling edge only	1<<3 for button
	//set interrupt priority
	//ITC->ISPR6&=0x3f;	//set 00 for TIM1
	//ITC->ISPR7&=0xf3;	//set 00 for TIM4
	//ITC->ISPR3&=0xf3;	//set 00 for EXTI1 btn
	//SPI init NRF24L01
	SYSCFG_REMAPPinConfig(REMAP_Pin_SPI1Full,ENABLE);  //remap SPI to PC,PA if set
  GPIO_Init(NRF_CE,NRF_CE_PIN,GPIO_Mode_Out_PP_Low_Fast);	//NRF tx rx activation
  GPIO_Init(NRF_CSN,NRF_CSN_PIN,GPIO_Mode_Out_PP_High_Fast);	//NRF chip select,act low
  GPIO_Init(SPI_MOSI,SPI_MOSI_PIN,GPIO_Mode_Out_PP_High_Fast);
  GPIO_Init(SPI_SCK,SPI_SCK_PIN,GPIO_Mode_Out_PP_High_Fast);
  GPIO_Init(SPI_MISO,SPI_MISO_PIN,GPIO_Mode_In_FL_No_IT); //PU as variant
  SPI_DeInit(SPI1);
  CLK->PCKENR1|=0x10; //enable clock for SPI1
  SPI_Init(
  SPI1,SPI_FirstBit_MSB,SPI_BaudRatePrescaler_64,			//32us pulse 32kHz
  SPI_Mode_Master,SPI_CPOL_Low,SPI_CPHA_1Edge,	//
  SPI_Direction_2Lines_FullDuplex,SPI_NSS_Soft,0x00
  );
  SPI_Cmd(SPI1,ENABLE);  //disable while uart acts
}
//craft
void nrf_init2(uint8_t *address,unsigned char nrfmode){
	//init
	mydelay_ms(200);
	GPIO_LOW(NRF_CE,NRF_CE_PIN);
	if(nrfmode==MODETX){
		nrf24l01p_get_clear_irq_flags();
   	nrf24l01p_close_pipe(NRF24L01P_ALL);
		nrf24l01p_open_pipe(NRF24L01P_TX, FALSE);
   	nrf24l01p_set_rx_payload_width(NRF24L01P_PIPE0, NRF_PAYLOAD_LENGTH);
 		nrf24l01p_set_crc_mode(NRF24L01P_CRC_16BIT);
   	nrf24l01p_set_address_width(NRF24L01P_AW_5BYTES);
   	nrf24l01p_set_address(NRF24L01P_TX, address);
		nrf24l01p_set_address(NRF24L01P_PIPE0, address);
   	nrf24l01p_set_operation_mode(NRF24L01P_PTX);
   	nrf24l01p_set_rf_channel(NRF_CHANNEL);
 		nrf24l01p_set_power_mode(NRF24L01P_PWR_UP);
   	mydelay_ms(NRF_POWER_UP_DELAY);
		//usartMsg("MODETX configured\r\n");
    //
		//usartMsg("TRANSM selected\r\n");
	}
	if(nrfmode==MODERX){
		nrf24l01p_get_clear_irq_flags();
		nrf24l01p_close_pipe(NRF24L01P_ALL);
   	nrf24l01p_open_pipe(NRF24L01P_PIPE0, FALSE);
		nrf24l01p_set_crc_mode(NRF24L01P_CRC_16BIT);
		nrf24l01p_set_address_width(NRF24L01P_AW_5BYTES);
		nrf24l01p_set_address(NRF24L01P_TX, address);
		nrf24l01p_set_address(NRF24L01P_PIPE0, address);
		nrf24l01p_set_operation_mode(NRF24L01P_PRX);
		nrf24l01p_set_rx_payload_width(NRF24L01P_PIPE0, NRF_PAYLOAD_LENGTH);
		nrf24l01p_set_rf_channel(NRF_CHANNEL);
		nrf24l01p_set_power_mode(NRF24L01P_PWR_UP);
		mydelay_ms(NRF_POWER_UP_DELAY);
		mydelay_ms(NRF_POWER_UP_DELAY);
		GPIO_HIGH(NRF_CE,NRF_CE_PIN);
		//usartMsg("MODERX configured\r\n");
		//
		//usartMsg("REC selected\r\n");
	}
	if(nrfmode==MODETXA){
		nrf24l01p_get_clear_irq_flags();
   	nrf24l01p_close_pipe(NRF24L01P_ALL);
		nrf24l01p_open_pipe(NRF24L01P_PIPE0,TRUE);
   	nrf24l01p_set_crc_mode(NRF24L01P_CRC_16BIT);
		nrf24l01p_set_auto_retr(RF_RETRANSMITS, RF_RETRANS_DELAY); //retransm on
   	nrf24l01p_set_address_width(NRF24L01P_AW_5BYTES);	//addr
   	nrf24l01p_set_address(NRF24L01P_TX, address);		//addr
		nrf24l01p_set_address(NRF24L01P_PIPE0, address);	//addr
		
		//nrf24l01p_set_rx_payload_width(NRF24L01P_PIPE0, NRF_PAYLOAD_LENGTH);
   	nrf24l01p_set_operation_mode(NRF24L01P_PTX);
		
   	nrf24l01p_set_rf_channel(NRF_CHANNEL);
 		nrf24l01p_set_power_mode(NRF24L01P_PWR_UP);
   	mydelay_ms(NRF_POWER_UP_DELAY);
		mydelay_ms(NRF_POWER_UP_DELAY);
		//usartMsg("MODETXA configured\r\n");
		//
		//usartMsg("TRANSM selected\r\n");
	}
	if(nrfmode==MODERXA){
		nrf24l01p_get_clear_irq_flags();
   	nrf24l01p_close_pipe(NRF24L01P_ALL);
		nrf24l01p_open_pipe(NRF24L01P_PIPE0, TRUE);
   	nrf24l01p_set_crc_mode(NRF24L01P_CRC_16BIT);
		nrf24l01p_set_auto_retr(RF_RETRANSMITS, RF_RETRANS_DELAY); //retransm on
   	nrf24l01p_set_address_width(NRF24L01P_AW_5BYTES);	//addr
   	nrf24l01p_set_address(NRF24L01P_TX, address);		//addr
		nrf24l01p_set_address(NRF24L01P_PIPE0, address);	//addr
		
		nrf24l01p_set_operation_mode(NRF24L01P_PRX);
		nrf24l01p_set_rx_payload_width(NRF24L01P_PIPE0, NRF_PAYLOAD_LENGTH);
		
   	nrf24l01p_set_rf_channel(NRF_CHANNEL);
 		nrf24l01p_set_power_mode(NRF24L01P_PWR_UP);
		mydelay_ms(NRF_POWER_UP_DELAY);
		mydelay_ms(NRF_POWER_UP_DELAY);
		GPIO_HIGH(NRF_CE,NRF_CE_PIN);
		//usartMsg("MODERXA configured\r\n");
		//
		//usartMsg("REC selected\r\n");
	}
} //end of nrf_init2

//tasks
//func to trigger nrf mode
void RX_mode(void){	//receiver mode:
	if(fl&RXF){			//first time rx_mode run
		fl&=~RXF;	//clear flag
		nrf_init2(address,MODERX);	//change to receive mode
		//usartMsg("Changed to Receive mode",1);
	}
	//if received radio data,show it via com port
	if(nrf24l01p_get_irq_flags() & (1 << NRF24L01P_IRQ_RX_DR)){
	  nrf24l01p_clear_irq_flag(NRF24L01P_IRQ_RX_DR);
	  unsigned char payload[NRF_PAYLOAD_LENGTH];
	  nrf24l01p_read_rx_payload((uint8_t*)payload);//, sizeof(payload));
		//usartMsg("Received from transmitter",0);
	  unsigned char k;
		for(k=0; k<NRF_PAYLOAD_LENGTH; k++){
      //show data to uart
			//usartShowByteDec(payload[k]);								//print rx buf
			//usartTx(&payload[k]);
		}
  	//usartMsg(".",1);
		//flush buffer?
	}
	//if we have data from com to radio send
	if(fl&COMRX){
		fl&=~COMRX;	//clear com data from user flag
		fl|=RXF;	//set flag to reinitialize to rx mode
		nrf_init2(address,MODETX);	//change mode to TX
		static uint8_t payload[NRF_PAYLOAD_LENGTH];
		unsigned char i;
		for(i=0;i<NRF_PAYLOAD_LENGTH;i++){
			payload[i] = rxdata[i];	//fill tx buffer
			rxdata[i]=0x20;	//clear array: fill with spaces
		}
    //send data
		nrf24l01p_write_tx_payload(payload,NRF_PAYLOAD_LENGTH);
		GPIO_HIGH(NRF_CE,NRF_CE_PIN);	//on
		mydelay_ms(10);
		GPIO_LOW(NRF_CE,NRF_CE_PIN);		//and off CE
    //end of sending,wait for ack
		unsigned char timecnt=15;
		do{
			timecnt--;
			mydelay_ms(1);
		} while ((timecnt!=0)&&(!(nrf24l01p_get_irq_flags() & (1 << NRF24L01P_IRQ_TX_DS))));
		nrf24l01p_clear_irq_flag(NRF24L01P_IRQ_TX_DS);
		//usartMsg("NRF TX send",1);
	}
}
//rxdata[] as a main data holder
void mode_armed(){
  fl|=COMRX;  //set flag to say that we want send some data
  rxdata[0]='a';
  rxdata[1]='r';
  rxdata[2]='m';
  return;
  //
}
//rxdata[] as a main data holder
void mode_disarmed(){
  fl|=COMRX;  //set flag to say that we want send some data
  rxdata[0]='d';
  rxdata[1]='i';
  rxdata[2]='s';
  return;
  //
}
//rxdata[] as a main data holder
void mode_test(){
  fl|=COMRX;  //set flag to say that we want send some data
  rxdata[0]='o';
  rxdata[1]='s';
  rxdata[2]='o';
  return;
  //
}
//MAIN
void main(void){
  fl=0;   //clear all flags
  init();				//base cpu init
  tmr1_init();
	tmr4_init();
  nrf_init2(address,MODETX);	//init by default
	GPIO_HIGH(LED1_PORT,LED1_PIN);
  GPIO_HIGH(LED2_PORT,LED2_PIN);
	mydelay_ms(500);
	GPIO_LOW(LED1_PORT,LED1_PIN);
  GPIO_LOW(LED2_PORT,LED2_PIN);
  mydelay_ms(500);
	//TIM4_Cmd(ENABLE);
	//TIM1->CR1|=TIM1_CR1_CEN;  //enable TMR1
	enableInterrupts();
  while(1){
    if((fl&PRESSED)==PRESSED){
			mode_armed();
      GPIO_HIGH(LED1_PORT,LED1_PIN);
      fl&=~PRESSED;
      RX_mode();
		}
		if((fl&MDLPRESSED)==MDLPRESSED){
			mode_disarmed();
      GPIO_HIGH(LED2_PORT,LED2_PIN);
      fl&=~MDLPRESSED;
      RX_mode();
		}
		if((fl&LONGPRESSED)==LONGPRESSED){
			mode_test();
      RX_mode();
      fl&=~LONGPRESSED;
		}
		mydelay_ms(100);
    GPIO_LOW(LED1_PORT,LED1_PIN);
    GPIO_LOW(LED2_PORT,LED2_PIN);
	}
}
//----------------------------------------------------------------