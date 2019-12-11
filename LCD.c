/*    Partner(s) Name & E-mail: Alex Madera & amade002@ucr.edu
 *    Lab Section: 023
 *    Assignment: Lab # 3 Exercise # 1 slave
 *    Exercise Description: [optional - include for your own benefit]
 *    
 *    I acknowledge all content contained herein, excluding template 
 *     or example code, is my own original work.
 */


#include <avr/io.h>
#include <util/delay.h>
#include "scheduler.h"
#include "usart_ATmega1284.h"
#include "lcd.h"
#include <bit.h>
#include <avr/interrupt.h>
unsigned char input = 0x00;

#define PORT PORTB
#define DDR_SPI DDRB

#define DD_MOSI PB5
#define DD_MISO PB6
#define DD_SCK PB7
#define DD_SS PB4
char cinput = 0x00;
#define button1 (~PINC & 0x08)
#define button2 (~PINC & 0x10)
#define button3 (~PINC & 0x20)

void SPI_SlaveInit(void){
	DDRB = 0x40;
	DDR_SPI = (1<<DD_MISO)|(0<<DD_MOSI)| (0<<DD_SCK);
	SPCR |= (1<<SPE)| (1<<SPIE);
	sei();
}


ISR(SPI_STC_vect){
	input = SPDR;
	if(input == 0xFF){
		cinput = input;
	}
}


void SPI_slaveReceive(char cData){
	PORTB &= ~(1<<DD_SS); //SS is low
	SPDR = cData;
	while(!(SPSR & (1<<SPIF)));
	PORTB |= (1 << DD_SS); // SS is set High
}

char temp = 0x00;
enum check_state{c_init, c_transmit, c_transmitbw, c_transmitbin, c_wait, c_display, c_pick, c_piwait, c_pisend} check_state;
int check_sm(int state){
	switch(state){
		case c_init:
			state = c_wait;
			break;
		case c_wait:
			if(USART_HasReceived(0)){
				temp = USART_Receive(0);
				USART_Flush(0);
				if(temp == 0x01){
					state = c_display;
					temp = 0x00;
				}else{
					state = c_wait;
				}
			}
			break;
		case c_display:
			state = c_pick;
			break;
		case c_pick:
			if(button1){
				state = c_transmit;
			}else if(button2){
				state = c_transmitbin;
			}else if(button3){
				state = c_transmitbw;
			}else{
				state = c_pick;
			}
			break;
		case c_transmitbin:
			state = c_piwait;
			break;
		case c_transmitbw:
			state = c_piwait;
			break;
		case c_transmit:
			state = c_piwait;
			break;
		case c_piwait:
			if(cinput == 0xFF){
				cinput = 0x00;
				state = c_pisend;
			}
			break;
		case c_pisend:
			state = c_wait;
			break;
		default:
			state = c_init;
			break;
	}
	switch(state){
		case c_init:
			break;
		case c_wait:
			break;
		case c_display:
			LCD_DisplayString(0," 1. Reg   2. B&W    3. Binary");
			break;
		case c_pick:
			break;
		case c_transmit:
			LCD_DisplayString(0," ctrans");
			SPI_slaveReceive(10);
			break;
		case c_transmitbin:
			LCD_DisplayString(0," ctransbw");
			SPI_slaveReceive(11);
			break;
		case c_transmitbw:
			LCD_DisplayString(0," ctransbin");
			SPI_slaveReceive(12);
			break;
		case c_piwait:
			break;
		case c_pisend:
			if(USART_IsSendReady(0)){
				USART_Send(0x01,0);
			}
			break;
		default:
			break;
	}	
	return state;
};
int main(void)
{
    /* Replace with your application code */

	DDRA = 0xFF;
	PORTA = 0x00;
	DDRC = 0xC0;
	PORTC = 0x3F;
	// Variables
	LCD_init();
	LCD_WriteData('0'+2);
	task myTasks[1];
	tasksNum = 1;
	SPI_SlaveInit();
	initUSART(0);
	// set tasks
	myTasks[0].state = -1;
	myTasks[0].period = 1000;
	myTasks[0].elapsedTime = 100;
	myTasks[0].TickFct = &check_sm;
	myTasks[0].active = 0x01;
	tasks = myTasks;
	TimerSet(100);
	TimerOn();
    while (1) 
    {
    }
}



