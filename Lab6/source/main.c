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

enum states {start, init, nextLed, pause, r, rr} state ;
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
			state = init ;
			break ;
		case init:
			state = nextLed ;
			break ;
		case nextLed:
			if((~PINA & 0x01) == 0x01) state = pause ;
			else state = nextLed ;
			break ;
		case pause:
			if((~PINA & 0x01) == 0x01) state = pause ;
			else state = r ;
			break ;
		case r:
			if((~PINA & 0x01) == 0x01) state = rr ;
			else state = r ;
			break ;
		case rr:
			if((~PINA & 0x01) == 0x01) state = rr ;
			else state = init ;
			break ;
		default:
			state = start ;
			break ;
	}
	switch(state){
		case start:
			break ;
		case init:
			PORTB = 0x01 ;
			break ;
		case nextLed:
			if(t == 0x00){
				if(PORTB == 0x04){
					PORTB = PORTB >> 1 ;
					t = 0x01 ;
				}
				else PORTB = PORTB << 1 ;
			}
			else{
				if(PORTB == 0x01){
					PORTB = PORTB << 1 ;
					t = 0x00 ;
				}
				else PORTB = PORTB >> 1 ;
			}
			break ;
		case pause:
			break ;
		case r:
			break ;
		case rr:
			break ;
		default: break ;
	}
}


int main(void) {
	DDRB = 0xFF ; PORTB = 0x00 ;
	DDRA = 0x00 ; PORTA = 0xFF ;
	TimerSet(300) ;
	TimerOn() ;
    while (1) {
	    Tick() ;
	    while(!TimerFlag) {} ;
	    TimerFlag = 0 ;
    }
    return 1 ;
}
