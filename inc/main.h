#ifndef __MAIN_H
#define __MAIN_H
//fl bits,my
#define PRESSED 0x01		//simple btn press
#define MDLPRESSED 0x02	//0.5 <= 2sec btn
#define LONGPRESSED 0x04	//>2 sec btn press
//#define BTNDOWN 0xxx		//1 if button was pressed,used for btn handler
#define COMRX 0x08		//set if com data from user rdy
#define TXF 0x10			//first time flag
#define RXF 0x20			//
#define TXPLF 0x40		//transmitt with ack payload
#define RXPLF 0x80		//receive with ack payload

//for NRF
#define  NRF_CHANNEL 120
#define  NRF_POWER_UP_DELAY 4
#define  NRF_PAYLOAD_LENGTH 3		//also buffer for com data
#define RF_RETRANSMITS 5
#if (RF_PAYLOAD_LENGTH <= 18)
#define RF_RETRANS_DELAY 250
#else
#define RF_RETRANS_DELAY 500
#endif
//func nrfmodes
#define MODETX 0		//transmitt wo ack
#define MODERX 1		//receive wo ack
#define MODETXA 2		//transmitt with ack
#define MODERXA 3
#define MODETXB 4		//transmitt with data back
#define MODERXB 5

//mode of alarm
#define IDLE 0   //at startup
#define ARMED 1  //read accelerometer,compare with treshold
#define ALARM 2   //ALARM signal!


//base funcs
void tmr1_init();
void tmr4_init();
void init();
//variable functions
void mode_armed();
void mode_disarmed();
void mode_test();

#endif