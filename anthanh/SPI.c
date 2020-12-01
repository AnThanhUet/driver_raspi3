#include <linux/gpio.h>
#include "SPI.h"

void spi_init(void)
{
    gpio_direction_output(MOSI, 0);
    gpio_direction_output(SCK, 0);
    gpio_direction_output(SS, 0);
    gpio_direction_output(RST, 0);
    gpio_direction_output(DC, 0);

    gpio_set_value(RST, 1);

}