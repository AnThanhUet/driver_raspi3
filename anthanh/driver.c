#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fd.h>
#include "lcd.h"

#define BCM2837_PERI_BASE   0x3F000000
#define GPIO_BASE           0x3F200000 /* GPIO controller */

#define PAGE_SIZE   (4*1024)
#define BLOCK_SIZE  (4*1024)


#define LCD_WIDTH   84
#define LCD_HEIGHT  48

#define LCD_C   0
#define LCD_D   1


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

    gpio_direction_output(CE, 0);
    gpio_direction_output(RST, 0);
    gpio_direction_output(DC, 0);
    gpio_direction_output(SDA_LCD, 0);
    gpio_direction_output(CLK_LCD, 0);
    
    LCD_khoitao();      

    LCD_chonvitri(10,2);
    LCD_guichuoi("Goc DIY.com");  
    LCD_chonvitri(1,3);     
    LCD_guichuoi("Chia se dam me");       
    LCD_chonvitri(20,4);     
    LCD_guichuoi("dien tu");      
    LCD_chonvitri(20,5);     
    LCD_guichuoi("tin hoc");
    
    printk(KERN_INFO "Module lcd5110 inserted\n");
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
    int pins[5] = {RES, SCE, DC, SDIN, SCLK};
        for(int g=0; g<6; g++)
        {
            gpio_free(pins[g]);
        }
    device_destroy(lcd_class, dev_num);
    class_destroy(lcd_class);
    cdev_del(&lcd_cdev);
    unregister_chrdev_region(dev_num, 1);
    printk(KERN_INFO "Module lcd5110 rmmoved\n");
    kfree(kbuf);
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
    char *kbuf = kcalloc((count + 1), sizeof(char), GFP_KERNEL);

    if (copy_to_user(kbuf, buf, count) != 0)
    {
        kfree(kbuf);
        return -EFAULT;
    }
    kbuf[count-1] = 0;

    clearLcdScreen();
    writeStringToLcd(kbuf);
    //kfree(kbuf);
    return count;
}


module_init(my_lcd_init);
module_exit(my_lcd_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("HuynhLD+AnNT");
MODULE_DESCRIPTION("Nokia Driver");
