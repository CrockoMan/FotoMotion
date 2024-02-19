/*****************************************************
Chip type               : ATtiny2313
AVR Core Clock frequency: 8,000000 MHz
CKSEL0 CKSEL1 CKSEL3 SUT0 CKDIV8 BODLEVEL1
Memory model            : Tiny
External RAM size       : 0
Data Stack size         : 32
*****************************************************/
#include <tiny2313.h>
#include <stdio.h>
#include <delay.h>
#include <trc.h>

#define LEDDDR      DDRD
#define LEDPIN      PIND
#define LEDPORT     PORTD
#define LED         0
#define LEDON()     PORTD |=  (1<<LED)
#define LEDOFF()    PORTD &= ~(1<<LED)


#define SetBit(y,x)    y |= (1<<x) 
#define ClearBit(y,x)  y &=~(1<<x) 

#define LASER_OFF   0
#define LASER_ON    1
#define LASER_CMD   2
#define LASER_DELAY 3

#define DEBUG

volatile unsigned char  cCounter=0, cRecv=0, cRecvBuf[5], cTimer=0, cLaser=LASER_OFF;

interrupt [EXT_INT1] void ext_int1_isr(void)
{

    ClearBit(GIMSK,INT1);

    LEDON();

    cRecvBuf[cCounter]=SpiRead();
#ifdef DEBUG
    putchar(cRecvBuf[cCounter]);
#endif
                    
    cCounter=cCounter+1;

    if(cCounter==4)
    {
        cCounter=0;
        FIFOReset();
        cRecv=1;
#ifdef DEBUG
    putchar(' ');
#endif
    }
        SetBit(GIMSK,INT1);
/*    else
    {
        SetBit(GIMSK,INT1);
    }
*/
    LEDOFF();
    
}



interrupt [TIM0_OVF] void timer0_ovf_isr(void)
{
 TCNT0 = 0x3D; //reload counter value
 cTimer=cTimer+1;
    switch(cLaser)
    {
        case LASER_ON:
                if(cTimer==5)
                {
                    cTimer=0;
                    LEDOFF();
                    cLaser=LASER_OFF;
                }
            break;
        case LASER_OFF:
                if(cTimer==15)
                {
                    cTimer=0;
                    LEDON();
                    cLaser=LASER_ON;
                }
            break;
        case LASER_CMD:
                    cTimer=0;
        case LASER_DELAY:
                    cTimer=0;
            break;
        default:
    }
}



unsigned char cCompare(unsigned char *Buf1, unsigned char *Buf2)
{ 
	while(*Buf2)
		if(*Buf1++ != *Buf2++)	return 0;
	return 1;
}


void main(void)
{
unsigned char StartCmd[] = "G_ON";
unsigned char StopCmd[]  = "GOFF";
unsigned char DelayCmd[]  ="GDEL";

LEDDDR = (1<<LED);

#ifdef DEBUG
// USART initialization
// Communication Parameters: 8 Data, 1 Stop, No Parity
// USART Receiver: Off
// USART Transmitter: On
// USART Mode: Asynchronous
// USART Baud Rate: 9600
UCSRA=0x00;
UCSRB=0x08;
UCSRC=0x06;
UBRRH=0x00;
UBRRL=0x33;

putchar('>');
#endif

// Crystal Oscillator division factor: 1
#pragma optsize-
CLKPR=0x80;
CLKPR=0x00;
#ifdef _OPTIMIZE_SIZE_
#pragma optsize+
#endif


TrcPortInit();
TrcInitRcv();
FIFOReset();

//LEDOFF();

#ifdef DEBUG
putchar('>');
#endif


 //TIMER0 initialize - prescale:1024
// WGM: Normal
// desired value: 40Hz
// actual value: 40,064Hz (0,2%)
TCCR0B = 0x00; //stop timer
TCNT0  = 0x3D; //set count
OCR0A  = 0xC3; //set count
OCR0B  = 0xC3; //set count
TCCR0A = 0x00; //start timer
TCCR0B = 0x05; //start timer

TIMSK = 0x02;


// INT1: On
// INT1 Mode: Rising Edge
GIMSK=(1<<INT1);
MCUCR=(1<<ISC10) | (1<<ISC11);
EIFR=(1<<INTF1);

#asm("sei")

while (1)
      {
        if(cRecv==1)
        {
            if( cCompare(cRecvBuf, StartCmd)==1 )
            {
                cLaser=LASER_CMD;
                LEDON();
            }

            if( cCompare(cRecvBuf, StopCmd)==1 )
            {
                LEDOFF();
                cLaser=LASER_OFF;
            }

            if( cCompare(cRecvBuf, DelayCmd)==1 )
            {
                LEDOFF();
                cLaser=LASER_DELAY;
            }

            cRecv=0;
            cRecvBuf[0]=0;
            cRecvBuf[1]=0;
            cRecvBuf[2]=0;
            cRecvBuf[3]=0;
            cRecvBuf[4]=0;
        }

      }
}
