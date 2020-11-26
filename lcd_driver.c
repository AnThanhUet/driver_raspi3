#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <linux/delay.h>

/* Raspberry pi 3 B+, address base */
#define GPIO_BASE_ADDR		0x3F200000
#define GPSET0			7
#define GPCLR0			10
#define GPLVL0			13

/****************************************/
struct GpioRegisters
{
    uint32_t GPFSEL[6];
    uint32_t Reserved1;
    uint32_t GPSET[2];
    uint32_t Reserved2;
    uint32_t GPCLR[2];
};

struct GpioRegisters *s_pGpioRegisters;
/********************************************/
/*set gpio from gpio9 to gpio19 output
*GPIO from 9 to 16 spend for data transfer
*/



int gpio_pin[11] = {9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};

void gpio_set_direction(unsigned int *base_addr, Direction direct, int pin)
{
	if (direct == INPUT)
		*(base_addr + (pin / 10)) = (*(base_addr + (pin /10)) & ~(7 << ((pin % 10)*3)) | (0 << ((pin % 10) * 3)));
	else
		*(base_addr + (pin / 10)) = (*(base_addr + (pin /10)) & ~(7 << ((pin % 10)*3)) | (1 << ((pin % 10) * 3)));
}

void gpio_set_output(unsigned int *base_addr)
{
	int i;

	for(i = 0; i < 11; i++) {
		gpio_set_direction(base_addr, OUTPUT, gpio_pin[i]);
	}
}

int get_pin_state(unsigned int *base_addr, int pin)
{
	gpio_set_direction(base_addr, INPUT, pin);
	return (*(base_addr + GPLVL0) & (1 << pin));
}

void gpio_init(unsigned int *base_addr)
{
	gpio_set_output(base_addr);
}

void gpio_set_value(unsigned int *base_addr, int pin, int value)
{
	if (value)
		*(base_addr + GPSET0) = 1 << pin;
	else
		*(base_addr + GPCLR0) = 1 << pin;
}
/************************************************************************************************************/
static void set_mode()
{
	/*
	 * set GPIO pin 11 (GPIO17) to output
	 * GPIO pin 11 -> GPFSEL1 [23 - 21]
	 */ 
	gpio_mode = (unsigned int *)(gpio_base + 1);
	*gpio_mode = ((*gpio_mode) & (~ (7 << 21))) | (1 << 21);
}

static void set0()
{
	gpio_low = (unsigned int *)(gpio_base + 10);
	*gpio_low = 1 << 17;
}

static void clr0()
{
	/*
	 * set GPIO pin 11 (GPIO17) to output
	 * GPIO pin 11 -> GPFSEL1 [23 - 21]
	 */ 
	gpio_mode = (unsigned int *)(gpio_base + 1);
	*gpio_mode = ((*gpio_mode) & (~ (7 << 21))) | (1 << 21);
}

/************************************************************************************************************/
/*Define for LCD*/



















static int dev_open(struct inode *, struct file *);
static int dev_close(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

static long dev_ioctl(struct file *, unsigned int, unsigned long);

static struct file_operations fops = {
	.owner          = THIS_MODULE,
	.open           = dev_open,
    .release        = dev_close,

    .read           = dev_read,
	.write          = dev_write,
	.unlocked_ioctl = dev_ioctl,
};

static struct miscdevice my_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "lcd_driver",
	.fops = &fops,
};

static int dev_open(struct inode *inodep, struct file *filep)
{
    pr_info("open file\n");
	return 0;
}

static int dev_close(struct inode *inodep, struct file *filep)
{
    pr_info("file is closed\n");
	return 0;
}

static ssize_t dev_write(struct file *filep, const char __user *user_buf, size_t len,
                        loff_t *offset)
{
	int ret;
	Draw_String_t *str = NULL;
	str = kmalloc(sizeof(Draw_String_t), GFP_KERNEL );
    /* bufer user -> str */
	ret = copy_from_user(str, user_buf, sizeof(Draw_String_t));
	if (ret) {
		printk(KERN_WARNING "Can not copy from user!\n");
	}

	/* Hien thi LCD */

    /* ON - OFF LED */

	return len;
}

static int __init my_driver_init(void)
{
    int ret;
	gpio_addr = (unsigned int *)ioremap(GPIO_BASE_ADDR, 0x100);
	//setup_lcd(gpio_addr);
	//write_string(gpio_addr, "hello");
	ret = misc_register(&my_dev);
	if (ret) {
		pr_alert("can not register misc device\n");
		return ret;
	}
	pr_info("register device successfully with minor number is: %d\n", my_dev.minor);
	return ret;
}
static void __exit my_driver_exit(void)
{
	misc_deregister(&my_dev);
	clear_screen(gpio_addr);
	iounmap(gpio_addr);
	pr_info("goodbye\n");

}


static long my_ioctl(struct file *f, unsigned int cmd, unsigned long arg){
    set_device d;
    printk("The %s function was invoked\n", __FUNCTION__);

    switch (cmd)
    {
    case GET_INFO:
  
        if (copy_to_user((set_device *)arg, &d, sizeof(set_device))){
            return -EACCES;
        }
        break;
    case CLEAR_INFO:
   
        break;
    case SET_INFO:
        if (copy_from_user(&d, (set_device *)arg, sizeof(set_device))){
            return -EACCES;
        }
        
        break;
    default:
        return -EINVAL;
    }
    return 0;
}


module_init(my_driver_init);
module_exit(my_driver_exit);

MODULE_LICENSE("GPL"); 
MODULE_AUTHOR(DRIVER_AUTHOR); 
MODULE_DESCRIPTION(DRIVER_DESC); 
MODULE_VERSION(DRIVER_VERSION); 
MODULE_SUPPORTED_DEVICE("testdevice"); 