/*
	the nothing todo driver with SPI interface
	powered by Chowe.
*/

#include <linux/init.h>

#include <asm/io.h>

#include <linux/io.h>
#include <linux/of.h>
#include <linux/fs.h>

#include <linux/gfp.h>

#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/poll.h>

#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include <linux/mutex.h>
#include <linux/errno.h>
#include <asm/uaccess.h>

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/string.h>

#include <linux/of_gpio.h>
#include <linux/proc_fs.h>
#include <linux/kthread.h>
#include <linux/jiffies.h>

#include <linux/firmware.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/miscdevice.h>
#include <linux/moduleparam.h>
#include <linux/timekeeping.h>
#include <linux/gpio/consumer.h>
#include <linux/platform_device.h>

// #include <linux/i2c.h>
#include <linux/spi/spi.h>

static int foo_probe(struct spi_device *spi)
{
	int ret;
	u8 buf[2];

	spi->bits_per_word = 8;
	spi->mode = SPI_MODE_0;
	ret = spi_setup(spi);
	if (ret < 0)
		return ret;

	spi_read(spi, buf, sizeof(buf));
	spi_write(spi, buf, sizeof(buf));

	return 0;
}

static int foo_remove(struct spi_device *spi)
{
	return 0;
}

static const struct spi_device_id foo_id_table[] = {
	{"foo_0", 0},
	{},
};
MODULE_DEVICE_TABLE(spi, foo_id_table);

static const struct of_device_id foo_of_match_table[] = {
	{
		.compatible = "foo,foo", // manufacturer,model
	},
	{},
};
MODULE_DEVICE_TABLE(of, foo_of_match_table);

static struct spi_driver foo_drv = {
	.probe = foo_probe,
	.remove = foo_remove,
	.id_table = foo_id_table,
	.driver = {
		.name = "foo",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(foo_of_match_table),
	},
};

module_spi_driver(foo_drv);

MODULE_AUTHOR("Chowe");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("the nothing todo driver with SPI interface");

/* dts
&spi0 {
        foo@1 {
            compatible = "foo,foo";
            reg = <1>;
            devname = "foo";
            spi-max-frequency = <20000000>;
            rst-gpio = <&gpio_porta 1 GPIO_ACTIVE_HIGH>;
            irq-gpio = <&gpio_porta 2 GPIO_ACTIVE_HIGH>;
        };
};
*/
