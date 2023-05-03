#include <mega32.h>
#define ADC_VREF_TYPE 0xC0

void InitAVR()
{
    PORTC=0b10000000;
    DDRC =0b01111111;

    PORTD=0b11001001;
    DDRD =0b10100000;


//TIMER0 initialize - prescale:1024
// WGM: Normal
// desired value: 100Hz
// actual value: 100,160Hz (0,2%)
    TCNT0 = 0xB2; //set count
    OCR0  = 0x4E;  //set compare
    TCCR0 = 0x05; //start timer
    TIMSK = 0x01; //timer interrupt sources

    ADMUX=ADC_VREF_TYPE & 0xff;
    ADCSRA=0x83;
}