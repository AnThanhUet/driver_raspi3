#include "lcd.h"

void LCD_command(unsigned char cmd)
{
    /* cmd - 8bit*/
    //RS = 0 // truyen command
    gpio_set_value(RS, 0);
    //RW = 0 // ghi data
    gpio_set_value(RW, 0);

  //==>  DATA = cmd;
    int i;
    int temp;
    // set data
    for(i = 7; i >= 0; i--)
    {
        temp = (cmd & (1 << (7 - i)));
        if(temp)
        {
            gpio_set_value(gpio_data[i], 1);
        }
        else
        {
            gpio_set_value(gpio_data[i], 0);
        }
    }


    //EN = 1 // bat dau khuyn truyen en = 1
    gpio_set_value(EN, 1);
    msleep(1);// 1milis
    //En = 0 // ket thuc khung truyen
    gpio_set_value(EN, 0);
    msleep(3);
}

void LCD_char(unsigned char char_data)
{
    /* char_data 8 bit */

    //RS = 1 // truyne data
    gpio_set_value(RS, 1);
    //RW = 0 // ghi data
    gpio_set_value(RW, 0);

    //==>  DATA = char data
    int i;
    int temp;
    // set data
    for(i = 7; i >= 0; i--)
    {
        temp = (char_data & (1 << (7 - i)));
        if(temp)
        {
            gpio_set_value(gpio_data[i], 1);
        }
        else
        {
            gpio_set_value(gpio_data[i], 0);
        }
    }


    //EN = 1 // bat dau khuyn truyen en = 1
    gpio_set_value(EN, 1);
    msleep(1);// 1milis
    //En = 0 // ket thuc khung truyen
    gpio_set_value(EN, 0);
    msleep(3);
}

void LCD_init(void)
{
    //Output (d7 -> D0);
    //Output (RS, E, RW)
    int i;
    for(i = 0; i < 11; i++)
    {
        gpio_direction_output(gpio_pin[i], 0);
    }
    msleep(20);

    LCD_command(0x38); // khoi tao lcd sung 8 bit mode va 2 line
    LCD_command(0x0C); // bat lcd va tat con tro
    LCD_command(0x06); // con tro tu dich sang ben phai
    LCD_command(0x01); // xoa mhinh
    LCD_command(0x80); // cho contro ve vtri ban dau
}
void LCD_string(char *str)
{
    int i;
    for(i = 0; str[i]!=0; i++)
    {
        LCD_char(str[i]);
    }

}
void LCD_string_xy(char row, char pos, char *str)
{
    /*
        1st line ))h 27Hex => 1 line hti dc 40 ki tu (27 hex = 39 dec)
        2st line from 40H - 67H
    */
    if(row == 0 && pos < 16)
    {
        LCD_command((pos & 0x0F)|0x80); // set hang va vi trij < 16
    }
    else if(row == 1 && pos < 16)
    {
        LCD_command((pos & 0x0F) | 0xC0);
    }
    LCD_string(str);


}

void LCD_clear(void)
{
    LCD_command(0x01);// xoa hma hinh
    LCD_command(0x80); /// dua con tro ve home osition
}