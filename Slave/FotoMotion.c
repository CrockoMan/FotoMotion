/*****************************************************
Project : 
Version : 
Date    :
Author  :
Company : 
Comments: 


Chip type               : ATmega32
Program type            : Application
AVR Core Clock frequency: 8,000000 MHz
Memory model            : Small
External RAM size       : 0
Data Stack size         : 512
*****************************************************/

#include <mega32.h>
#include <delay.h>
#include <stdio.h>
#include <HD44780.h>
#include <buttons.h>
#include <trc.h>

#define SetBit(x,y)    (x|=(1<<y))
#define ClearBit(x,y)  (x&=~(1<<y))
#define InvertBit(x,y) (x^=(1<<y))
#define IsBitSet(pin, bit)   ((pin)  & (1 << (bit)))
#define IsBitClear(pin, bit) (~(pin) & (1 << (bit)))

#define ADC_VREF_TYPE 0xC0

#define LED_ON()  ClearBit(PORTD,7)
#define LED_OFF() SetBit(PORTD,7)

#define KEY_SECONDS     1
#define KEY_MSECONDS    2
#define KEY_USECONDS    3
#define KEY_START       4

void InitAVR(void);

volatile char cTick=0; 

unsigned int read_adc(unsigned char adc_input)
{
ADMUX=adc_input | (ADC_VREF_TYPE & 0xff);
delay_us(10);
ADCSRA|=0x40;
while ((ADCSRA & 0x10)==0);
ADCSRA|=0x10;
return ADCW;
}

interrupt [TIM0_OVF] void timer0_ovf_isr(void)
{
    TCNT0 = 0xB2;   //перезагрузка таймера
    BUT_Debrief();  //опрос клавиатуры
    cTick=cTick+1;
}


void Beep()
{
    char i=0;
    for(i=0;i<15;i++)
    {
        InvertBit(PORTD,5);
        delay_ms(1);
    }
    ClearBit(PORTD,5);
}


void main(void)
{
unsigned char cStr[]={0,0,0,0,0}, cKey=0, cDelay[]={0,1,5}, cFlag=1;
int cPresDelay=0, cPrevDelay=0;
int ADCVal=0, PrevADCVal=0, iLightOn=0;
unsigned char StartCmd[] = "G_ON";
unsigned char StopCmd[]  = "GOFF";
 
InitAVR();      // Инициализация AVR
BUT_Init();     // Инициализация кнопок
lcd_init(1);    // Инициализация LCD

LED_OFF();

lcd_gotoxy(1,0);
lcd_putsf("LCD OK");
read_adc(0);

//lcd_clear();
lcd_gotoxy(1,0);
lcd_putsf("Трансивер...");
delay_ms(1000);
lcd_putsf(" OK");
delay_ms(300);

TrcPortInit();
TrcInitSnd();

TrcSendPocket(StartCmd, 4);         // Отправить команду СТАРТ

delay_ms(500);
lcd_clear();

#asm("sei")

while (1)
      {
        if(cTick>=20) { cKey=BUT_GetKey(); cTick=0; }
        else          { cKey=0;   }
// Обработка кнопок
        if(cKey>0)    {Beep(); }
        
        if(cKey==KEY_SECONDS)
        {
            if(cDelay[0]!=9)    cDelay[0]=cDelay[0]+1;
            else                cDelay[0]=0;
        }
        if(cKey==KEY_MSECONDS)
        {
            if(cDelay[1]!=9)    cDelay[1]=cDelay[1]+1;
            else                cDelay[1]=0;
        }
        if(cKey==KEY_USECONDS)
        {
            if(cDelay[2]!=9)    cDelay[2]=cDelay[2]+1;
            else                cDelay[2]=0;
        }
        
// Ожидание пересечения линии
        if(cKey==KEY_START)
        {
            lcd_gotoxy(0,0);
            lcd_putsf(" Жду ...  ");
            cFlag=1;
            
            while( cFlag==1 )
            {
                iLightOn=read_adc(0);
                if( iLightOn <= (ADCVal-50) )
                { 
                    LED_ON();
                    delay_ms(cPresDelay);
                    LED_OFF();

                    lcd_gotoxy(0,0);
                    lcd_putsf("          ");
                    cFlag=0;
                }
            };
        }
        else                    
        {
            LED_OFF(); 
        }
        
// Датчик сработал, вывод данных, очистка дисплея
        if(iLightOn!=0) 
        {
            iLightOn=0; 
            delay_ms(2000);
            lcd_gotoxy(12,0);
            lcd_putsf("    ");
        }

// Отрисовка данных ADC
        ADCVal=read_adc(0);
        if(ADCVal!=PrevADCVal)
        {
            sprintf(cStr, "%4d", ADCVal);
            lcd_gotoxy(0,0);
            lcd_putsf("Освещ        ");
            lcd_gotoxy(5,0);
            lcd_puts(cStr);
            delay_ms(200);
            PrevADCVal=ADCVal;
        }

// Отрисовка данных клавиатуры        
        cPresDelay=cDelay[0]*1000+cDelay[1]*100+cDelay[2]*10;
        if(cPresDelay!=cPrevDelay)
        {
            cPrevDelay=cPresDelay;
            lcd_gotoxy(0,1);
            lcd_putchar(cDelay[0]+0x30);
            lcd_putsf(".");
            lcd_putchar(cDelay[1]+0x30);
            lcd_putchar(cDelay[2]+0x30);
            lcd_putsf("0 сек");
        }
      }
}
