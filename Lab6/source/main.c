/*	Author: sspon001
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum states {start, begin, init, reset, plus, minus, plusOn, minusOn, add, sub} state ;
unsigned char t = 0x00 ;

volatile unsigned char TimerFlag = 0;
void TimerISR() { TimerFlag = 1;}
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;
void TimerOn() {
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}
void TimerOff() {
	TCCR1B = 0x00;
}
ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}
void TimerSet (unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}
void Tick(){
	switch(state){
		case start:
			state = begin ;
			break ;
		case begin:
			state = init ;
			break ;
		case init:
			if((~PINA & 0x03) == 0x03) state = reset ;
			else if((~PINA & 0x03) == 0x01) state = add ;
			else if((~PINA & 0x03) == 0x02) state = sub ;
			break ;
		case reset:
			if((~PINA & 0x03) == 0x03) state = reset ;
			else state = init ;
			break ;	
		case plus:
			if((~PINA & 0x03) == 0x01) state = plus ;
			else state = init ;
			break ;
		case plusOn:
			if((~PINA & 0x03) == 0x01){
				t++ ;
				state = plusOn ;
				if(t >= 0x0A) state = add ;
			}
			else state = plus ;
			break ;
		case add:
			if(t >= 0x0A) t = 0x00 ;
			state = plusOn ;
			break ;
		case minus:
			if((~PINA & 0x03) == 0x02) state = minus ;
			else state = init ;
			break ;
		case minusOn:
			if((~PINA & 0x03) == 0x02){
				state = minusOn ;
				t++ ;
				if(t >= 0x0A) state = sub ;
			}
			else state = minus ;
			break ;
		case sub:
			if(t >= 0x0A) t = 0x00 ;
			state = minusOn ;
			break ;
		default:
			state = start ;
			break ;
	}
	switch(state){
		case start:
			PORTB = 0x07 ;
			break ;
		case begin:
			PORTB = 0x07 ;
			break ;
		case init:
			t = 0x00 ;
		case plusOn:
			break ;
		case plus:
			break ;
		case minusOn:
			break ;
		case minus:
			break ;
		case add:
			if(PORTB < 0x09) PORTB = PORTB + 1 ;
			break ;
		case sub:
			if(PORTB > 0x00) PORTB = PORTB - 1 ;
			break ;
		case reset:
			PORTB = 0x00 ;
			break ;
		default:
			PORTB = 0x07 ;
			break ;
	}
}


int main(void) {
	DDRB = 0xFF ; PORTB = 0x00 ;
	DDRA = 0x00 ; PORTA = 0xFF ;
	TimerSet(100) ;
	TimerOn() ;
    while (1) {
	    Tick() ;
	    while(!TimerFlag) {} ;
	    TimerFlag = 0 ;
    }
    return 1 ;
}
