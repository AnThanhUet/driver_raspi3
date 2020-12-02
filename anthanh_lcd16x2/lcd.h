#ifndef _LCD_
#define _LCD_

#include <linux/delay.h>
#include <linux/gpio.h>

/*Define for GPIO*/
#define RS		    2
#define RW			3
#define EN			4


/*Define data*/
#define D0          21
#define D1			20
#define D2			16
#define D3			12
#define D4		    1
#define D5		    7
#define D6		    8
#define D7		    25

int gpio_data[8] = {D0, D1, D2, D3, D4, D5, D6, D7}; 
int gpio_pin[11] = {D0, D1, D2, D3, D4, D5, D6, D7, RS, RW, EN}; 

void LCD_command(unsigned char);
void LCD_char(unsigned char);
void LCD_init(void);
void LCD_string(char*);
void LCD_string_xy(char, char, char*);
void LCD_clear(void);

#endif

