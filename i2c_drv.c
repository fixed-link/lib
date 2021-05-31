
#include <uapi/linux/i2c.h>
#include <linux/gpio.h>
#include <linux/delay.h>

#define SDA_GPIO 1
#define SCL_GPIO 2

#define TIMING udelay(1)

#define SCL_HIGH gpio_set_value(SCL_GPIO, 1)
#define SCL_LOW gpio_set_value(SCL_GPIO, 0)

#define SDA_SET(val) gpio_set_value(SDA_GPIO, val)
#define SDA_HIGH SDA_SET(1)
#define SDA_LOW SDA_SET(0)

#define SDA_READ gpio_get_value(SDA_GPIO)

#define SDA_IN gpio_direction_input(SDA_GPIO)
#define SDA_OUT gpio_direction_output(SDA_GPIO, 0)

static int _init(unsigned sda, unsigned scl)
{
	if ((!gpio_is_valid(sda)) || (!gpio_is_valid(scl)))
		return -EINVAL;

	if (!gpio_request(sda, "gpio2sda"))
		return -EINVAL;
	if (!gpio_request(scl, "gpio2scl"))
	{
		gpio_free(sda);
		return -EINVAL;
	}

	gpio_direction_output(sda, 1);
	gpio_direction_output(scl, 1);
}

static void _deinit(unsigned sda, unsigned scl)
{
	gpio_free(sda);
	gpio_free(scl);
}

static inline void _start(void)
{
	SDA_LOW;
	TIMING;
	SCL_LOW;
	TIMING;
}

static inline void _stop(void)
{
	SDA_LOW;
	TIMING;
	SCL_HIGH;
	TIMING;
	SDA_HIGH;
	TIMING;
}

static inline int _getack(void)
{
	int ret;

	SDA_HIGH;
	SCL_HIGH;
	TIMING;

	SDA_IN;
	ret = SDA_READ;
	SDA_OUT;

	SCL_LOW;
	TIMING;

	return ret;
}

static inline void _sendack(void)
{
	SDA_LOW;
	TIMING;
	SCL_HIGH;
	TIMING;
	SCL_LOW;
	TIMING;
}

static int _send(unsigned char data)
{
	int val;

	for (size_t i = 7; i >= 0; i--)
	{
		val = (data >> i) & 1; // msb
		SDA_SET(val);
		TIMING;
		SCL_HIGH;
		TIMING;

		SCL_LOW;
		TIMING;
	}

	return _getack();
}

static unsigned char _recv(void)
{
	unsigned char val = 0;

	SDA_IN;
	for (size_t i = 0; i < 8; i++)
	{
		SCL_HIGH;
		TIMING;

		val <<= 1;
		val |= SDA_READ;

		SCL_LOW;
		TIMING;
	}
	SDA_OUT;

	_sendack(); // ack?

	return val;
}

int i2c_write(unsigned char *buf, int n)
{
	int i;

	for (i = 0; i < n; i++)
	{
		if (!_send(buf[i]))
		{
			i = -EIO;
			break;
		}
	}

	return i;
}

int i2c_read(unsigned char *buf, int n)
{
	int i;

	for (i = 0; i < n; i++)
	{
		buf[i] = _recv();
	}

	return i;
}

int send_addr(struct i2c_msg *msg)
{
	unsigned char addr;
	int ret;

	addr = (msg->addr << 1) | (msg->flags & I2C_M_RD);

	ret = i2c_write(&addr, 1);

	return ret;
}

int i2c_xfer(struct i2c_msg msgs[], int num)
{
	struct i2c_msg *pmsg;

	_start();
	for (size_t i = 0; i < num; i++)
	{
		pmsg = &msgs[i];
		if (send_addr(pmsg) < 0)
			goto _exit;

		if (pmsg->flags & I2C_M_RD)
		{
			i2c_read(pmsg->buf, pmsg->len);
		}
		else
		{
			if (i2c_write(pmsg->buf, pmsg->len) < 0)
				goto _exit;
		}
	}
	_stop();
	return 0;

_exit:
	_stop();
	return -EIO;
}
