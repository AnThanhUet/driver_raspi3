#include "lcd.h"
#include <stdlib.h>
#include <linux/delay.h>

#define CE  23 // set chan raspi
#define RST 26
#define DC  22                          
#define SDA_LCD 27 // DIN
#define CLK_LCD 17             


#define swap(a, b) {int t = a; a = b; b = t; }    // chuong trinh con ( hoan doi gia tri a va b) 
#define setpx(x) (1 << (x))


void LCD_gui1byte(unsigned char x, unsigned char byte)  // ham gui 1 byte du lieu den LCD
{
  unsigned char i;
  bit outbit;
  DC=x;		// x =1 hien thi ra man hinh, x=0 la gui lenh dieu khien LCD
  for(i=0;i<8;i++)
  {
    outbit=byte&0x80;	  // lay ra bit thu 7
	gpio_set_value(SDA_LCD, outbit);			  // gan chan SDA cho gia tri cua bit thu 7
	gpio_set_value(CLK_LCD, 1);				  // tao xung nhip day bit qua LCD
	gpio_set_value(CLK_LCD, 0);
	byte = byte << 1;	  // dich trai 1 lan de chuan bi lay bit tiep theo
  }
}

void LCD_khoitao()
{
    gpio_set_value(CLK_LCD, 0);
    gpio_set_value(CE, 0);		// cho phep LCD hoat dong
    udelay(2);
    gpio_set_value(RST, 0);
    udelay(2);
    gpio_set_value(RST, 1); // reset LCD
  udelay(2);
  LCD_gui1byte(0,0x21); // che do dieu khien LCD, gia tri nap vao 0x21 : tuong duong ghi du lieu theo chieu ngang va cho phep thuc hien cac lenh bo sung
  LCD_gui1byte(0,0xC0); // che do dieu khien LCD, dung dien ap 5V
  LCD_gui1byte(0,0x20); // che do dieu khien LCD, dung cac lenh co ban
  LCD_gui1byte(0,0x0C); //che do dieu khien LCD, che do hien thi thong thuong ||   hoac 0x0D neu muon nen den chu trang || 0x0C la hien binh thuong 
}

void LCD_guikitu(unsigned char kitu)
{
  unsigned char x,i;
  x=kitu-32;
  for(i=0;i<6;i++)
  {
    LCD_gui1byte(1,font6[x][i]);
  }
}

void LCD_guichuoi(unsigned char *chuoi)
{
 	while(*chuoi)
	{
	  LCD_guikitu(*chuoi);
	  chuoi++;
	}
}

void LCD_chonvitri(unsigned char x, unsigned char y)
{
   LCD_gui1byte(0,(0x40 | (y-1)));  // chon dong
   LCD_gui1byte(0,(0x80 | (x-1)));  // chon cot
}

void LCD_xoamanhinh()
{
 char x,y;
 for(x=0;x<6;x++)
 {
   for(y=0;y<84;y++)
   {
     LCD[x][y]=0;
   }
 }
}

void LCD_xuat()
{
  unsigned char x,y;
  for(y=0;y<6;y++)
	  {
	   for(x=0;x<84;x++)
		  {
		    LCD_gui1byte(1,LCD[y][x]);
		  }
  }
}

void LCD_sang1px(unsigned char x, unsigned char y)
{  
   LCD[y/8][x-1]|=setpx(y%8); 
}

void LCD_veduongngang(unsigned char x, unsigned char y,unsigned char dodai)
{
char dem;
  if(dodai>84)
  {
    dodai=84;
  }
  for(dem=0;dem<dodai;dem++)
  {
    LCD_sang1px(x+dem,y);
  }
} 

void LCD_veduongdoc(unsigned char x, unsigned char y,unsigned char dodai)
{
char dem;
  if(dodai>48)
  {
    dodai=48;
  }
  for(dem=0;dem<dodai;dem++)
  {
    LCD_sang1px(x,y+dem);
  }
}


void LCD_vehinhchunhat(unsigned char x, unsigned char y,unsigned char cao, unsigned char rong)
{
   unsigned char x1,y1;
   x1=x+rong-1;
   y1=y+cao-1;
   LCD_veduongdoc(x,y,cao); LCD_veduongdoc(x1,y,cao);     
   LCD_veduongngang(x,y,rong); LCD_veduongngang(x,y1,rong);  
}
void LCD_veduongthang(int x, int y,int x1, int y1)
{
 int dx,dy,err,ystep; 
 int steep=abs(y1-y) > abs(x1-x);   
 int xtd,ytd;
 if(x==x1)
 {
  ytd=y1-y;  
  ytd=abs(ytd);
  if(y1>y)
  { 
    LCD_veduongdoc(x,y,ytd+1);    
  }
  else
  {
    LCD_veduongdoc(x1,y1,ytd+1);
  }
  return;
 } 
 
  if(y==y1)
 {    
  xtd=x1-x;
  xtd=abs(xtd);
  if(x1>x)
  { 
    LCD_veduongngang(x,y,xtd+1);    
  }
  else
  {
    LCD_veduongngang(x1,y1,xtd+1);
  }
  return;
 }
 if (steep) 
  {
    swap(x, y);
    swap(x1, y1);
  }    
  
  if (x > x1) {
    swap(x, x1);
    swap(y, y1);
  }
 dx=x1-x;
 dy=abs(y1-y);
 err=dx/2;  
  
  if (y < y1)
   {
    ystep = 1;
   }      
  
   else {
    ystep = -1;} 
    
  for (; x<=x1; x++) 
  {
    if (steep) 
    {
      LCD_sang1px(y,x);

    } 
    
    else   
    
    {
      LCD_sang1px(x, y);
    }   
    
    err -= dy;     
    
    if (err < 0) {
      y += ystep;
      err += dx;
    }
  }
 
}