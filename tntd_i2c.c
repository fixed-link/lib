/*
	the nothing todo driver with I2C interface
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

#include <linux/i2c.h>
// #include <linux/spi/spi.h>

static int foo_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	u8 buf[2] = {0};

	dev_dbg(&client->dev, "%s\n", __func__);

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C))
		return -ENODEV;

	i2c_set_clientdata(client, NULL);

	i2c_master_send(client, buf, sizeof(buf));
	i2c_master_recv(client, buf, sizeof(buf));

	return 0;
}

static int foo_remove(struct i2c_client *client)
{
	devm_kfree(&client->dev, i2c_get_clientdata(client));
	return 0;
}

static const struct i2c_device_id foo_id[] = {
	{"foo_0", 0},
	{"foo_1", 0},
	{},
};
MODULE_DEVICE_TABLE(i2c, foo_id);

static const struct of_device_id foo_of_match_table[] = {
	{
		.compatible = "foo,foo", // manufacturer,model
	},
	{},
};
MODULE_DEVICE_TABLE(of, foo_of_match_table);

static struct i2c_driver foo_drv = {
	.probe = foo_probe,
	.remove = foo_remove,
	.id_table = foo_id,
	.driver = {
		.name = "foo",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(foo_of_match_table),
	},
};

module_i2c_driver(foo_drv);

MODULE_AUTHOR("Chowe");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("the nothing todo driver with I2C interface");

/* dts
&i2c0 {
        foo@39 {
            compatible = "foo,foo";
            reg = <0x39>;
            devname = "foo";
            rst-gpio = <&gpio_porta 1 GPIO_ACTIVE_HIGH>;
            irq-gpio = <&gpio_porta 2 GPIO_ACTIVE_HIGH>;
        };
};
*/
