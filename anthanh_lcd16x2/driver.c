#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fd.h>

//#include "lcd.h"
/***************************************************************************/
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

/*================================================================================*/
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
    mdelay(1);// 1milis
    //En = 0 // ket thuc khung truyen
    gpio_set_value(EN, 0);
    mdelay(3);
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
    mdelay(1);// 1milis
    //En = 0 // ket thuc khung truyen
    gpio_set_value(EN, 0);
    mdelay(3);
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
    mdelay(20);

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
/**********************************************************************************/

static dev_t dev_num = 0;
static struct class *lcd_class;
static struct cdev lcd_cdev;

static int __init my_lcd_init(void);
static void __exit my_lcd_exit(void);

/* ---------- D R I V E R   S P E C I F I C ------------------*/

static int lcd5110_open(struct inode *inode, struct file *filp);
static int lcd5110_release(struct inode *inode, struct file *filp);
static ssize_t lcd5110_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
static ssize_t lcd5110_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);

/* -----------------------------------------------------------*/
static struct file_operations lcd5110_fops = 
{
    .owner  =   THIS_MODULE,
    .open   =   lcd5110_open, 
    .read   =   lcd5110_read,
    .write  =   lcd5110_write,
    .release =  lcd5110_release, 
};


static int __init my_lcd_init(void)
{
    if ((alloc_chrdev_region(&dev_num, 0, 1, "lcd5110")) < 0)
    {
        pr_err("Cannot allocate major number\n");
        goto r_unreg;
    }
    /* create cdev structure */
    cdev_init(&lcd_cdev, &lcd5110_fops);
    /* add character device to the system */
    if ((cdev_add(&lcd_cdev, dev_num, 1)) < 0)
    {
        pr_err("Cannot add the device to the system\n");
        goto r_del;
    }
    /* Create struct class */
    if ((lcd_class = class_create(THIS_MODULE, "lcd5110_class")) == NULL)
    {
        pr_err("Cannot create the struct class\n");
        goto r_class;
    }
    /* Create device */
    if ((device_create(lcd_class, NULL, dev_num, NULL, "lcd5110_device")) == NULL)
    {
        pr_err("Cannot create the device\n");
        goto r_device;
    }

    LCD_init(void);
    LCD_clear(void);
    LCD_string_xy(0, 0, "anthanh");

    printk(KERN_INFO "Module lcd16x2 inserted\n");
    return 0;

    r_device:
        device_destroy(lcd_class, dev_num);
    r_class:
        class_destroy(lcd_class);
    r_del:
        cdev_del(&lcd_cdev);
    r_unreg:
        unregister_chrdev_region(dev_num, 1);
    
    return -1;
}

static void __exit my_lcd_exit(void)
{
    device_destroy(lcd_class, dev_num);
    class_destroy(lcd_class);
    cdev_del(&lcd_cdev);
    unregister_chrdev_region(dev_num, 1);
    printk(KERN_INFO "Module lcd 16x2 rmmoved\n");
    //kfree(kbuf);
}

static int lcd5110_open(struct inode *inode, struct file *filp)
{
    printk(KERN_INFO "Opened\n");
    return 0;
}

static int lcd5110_release(struct inode *inode, struct file *filp)
{
    printk(KERN_INFO "Closed\n");
    return 0;
}

static ssize_t lcd5110_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    return 0;
}

static ssize_t lcd5110_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
    /* Buffer writing to the device */
    //char *kbuf = kcalloc((count + 1), sizeof(char), GFP_KERNEL);

    //if (copy_to_user(kbuf, buf, count) != 0)
    //{
    //    kfree(kbuf);
    //    return -EFAULT;
    //}
    //kbuf[count-1] = 0;

    //clearLcdScreen();
    //writeStringToLcd(kbuf);
    //kfree(kbuf);
   // return count;
   return 0;
}

module_init(my_lcd_init);
module_exit(my_lcd_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("AnNT");
MODULE_DESCRIPTION("LCD 16x2 Driver");
