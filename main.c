/*    Partner(s) Name & E-mail: Alex Madera & amade002@ucr.edu
 *    Lab Section: 023
 *    Assignment: Lab # 3 Exercise # 1 slave
 *    Exercise Description: [optional - include for your own benefit]
 *    
 *    I acknowledge all content contained herein, excluding template 
 *     or example code, is my own original work.
 */


#include <util/delay.h>
#include "scheduler.h"
#include "spi.h"
#include "mfrc522.h"
#include <bit.h>

#define PORT PORTB
#define DDR_SPI DDRB

#define DD_MOSI PB5
#define DD_MISO PB6
#define DD_SCK PB7
#define DD_SS PB4
#define SPI_SS1 PB0

uint8_t byte = 0x00;
uint8_t str[MAX_LEN];
unsigned char input = 0x00;
char reader = 0x00;


void SPI_SlaveInit(void){
	DDRB = 0x40;
	DDR_SPI = (1<<DD_MISO)|(0<<DD_MOSI)| (0<<DD_SCK);
	SPCR |= (1<<SPE)| (1<<SPIE);
	sei();
}


ISR(SPI_STC_vect){
	input = SPDR;
}

char temp = 0x00;
enum readRFC_states{r_init, r_check, r_wait} readRFC_state;
int readRFC(int state){
	switch(state){
		case r_init:
			state = r_check;
			break;
		case r_check:
			state = r_check;
			byte = mfrc522_request(PICC_REQALL,str);
			if(byte == CARD_FOUND)
				state = r_wait;
			else
				state = r_check;
			break;
		case r_wait:
			temp = (~PINC & 0x01);
			if(temp){
				state = r_check;
			}else{
				state = r_wait;
			}
			break;
		default:
			state = r_init;
			break;
	}
	
	switch(state){
		case r_init:
			break;
		case r_check:
			PORTA = reader;
			PORTB &= ~(1<< 0); // Goes low
			break;
		case r_wait:
			PORTA = 0xFF;
			PORTB |= (1 << 0); // Goes high
			break;
		default:
			break;
	}
	readRFC_state = state;
	return state;
}


int main(void)
{
    /* Replace with your application code */
	// Output
	DDRA = 0xFF;
	PORTA = 0x00;
	// Input
	DDRC = 0x00;
	PORTC = 0xFF;
	// Variables
	spi_init(1);
	//Initialize card reader
	mfrc522_init();
	byte = mfrc522_read(VersionReg);
	
	if(byte == 0x92){
		PORTA = 0x01;
		reader = 0x01;
	}else if(byte == 0x91||byte==0x90){
		PORTA = 0x02;
		reader = 0x02;
	}else{
		PORTA = 0x04;
		reader = 0x04;
	}
	
	byte = mfrc522_read(ComIEnReg);
	mfrc522_write(ComIEnReg, byte|0x20);
	byte = mfrc522_read(DivIEnReg);
	mfrc522_write(DivIEnReg,byte|0x80);
	_delay_ms(1500);
	
	// Start doing shit here
	//SPI_SlaveInit();
	
	task myTasks[1];
	tasksNum = 1;
	
	myTasks[0].state = -1;
	myTasks[0].period = 100;
	myTasks[0].elapsedTime = 100;
	myTasks[0].TickFct = &readRFC;
	myTasks[0].active = 0x01;
	
	tasks = myTasks;
	TimerSet(100);
	TimerOn();

    while (1) 
    {
    }
}



