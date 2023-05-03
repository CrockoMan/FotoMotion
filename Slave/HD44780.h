//**********************************************************
//
// DieHard
// Header ��� �������������� ���������� LCD ��� HD44780 ���� 
//
//**********************************************************
#ifndef	HD44780_h
#define	HD44780_h

#define LCDPORT PORTC
#define LCDDDR  DDRC
#define LCDPIN  PINC

#define LCD_RS  0
#define LCD_RD  2
#define LCD_E   1
#define LCD_DB4 3
#define LCD_DB5 4
#define LCD_DB6 5
#define LCD_DB7 6


#pragma used+
void lcd_init(char );                               // ������������� LCD � ������ ��������� ��������������� (0/1)
void lcd_clear(void);                               // ������� LCD
void lcd_gotoxy(unsigned char , unsigned char );    // ���������������� �������
void lcd_putchar(unsigned char );                   // ����� ������� � ������� ������� �������
void lcd_puthex(unsigned char );                    // ����� Hex � ������� ������� �������
void lcd_puts(unsigned char *);                     // ����� ������ �� ������� � ������� ������� �������
void lcd_putsf(char flash *);                       // ����� ������ �� Flash � ������� ������� �������

void lcd(unsigned char );
void lcd_cmd(unsigned char );
#pragma used-
#endif	//HD44780_h
