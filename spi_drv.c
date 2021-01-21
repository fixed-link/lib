#include <linux/init.h>
#include <linux/module.h>
#include <linux/spi/spi.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/gpio/consumer.h>
#include <asm/uaccess.h>
#include <linux/kernel.h>

#define HIGH 1
#define LOW 0

// #define TIMING udelay(1)
#define TIMING \
	do         \
	{          \
	} while (0) // do as fast as possible

#define DATA_ENTRY_MODE(mode) \
	if (mode)                 \
	{                         \
		x = 0;                \
		y = 8;                \
		z = 1;                \
	}                         \
	else                      \
	{                         \
		x = 7;                \
		y = -1;               \
		z = -1;               \
	}

typedef struct
{
	int sclk;
	int mosi;
	int miso;
	int cs;
} spi_pin;

struct spi_data
{
	unsigned char *tx_buf;
	unsigned char *rx_buf;
	ssize_t n;
	int mode;
};

static spi_pin pin;

static int gpio_init(void)
{
	int ret;

	ret = gpio_request(pin.sclk, "spi sclk");
	if (ret < 0)
	{
		pr_err("gpio_request %d fail! %d\n", pin.sclk, ret);
		goto out;
	}

	ret = gpio_request(pin.mosi, "spi mosi");
	if (ret < 0)
	{
		pr_err("gpio_request %d fail! %d\n", pin.mosi, ret);
		goto out1;
	}

	ret = gpio_request(pin.miso, "spi miso");
	if (ret < 0)
	{
		pr_err("gpio_request %d fail! %d\n", pin.miso, ret);
		goto out2;
	}

	ret = gpio_request(pin.cs, "spi cs");
	if (ret < 0)
	{
		pr_err("gpio_request %d fail! %d\n", pin.cs, ret);
		goto out3;
	}

	return 0;

out3:
	gpio_free(pin.miso);
out2:
	gpio_free(pin.mosi);
out1:
	gpio_free(pin.sclk);
out:
	return ret;
}

int set_spi_pin(int sclk, int mosi, int miso, int cs)
{
	pin.sclk = sclk;
	pin.mosi = mosi;
	pin.miso = miso;
	pin.cs = cs;

	return gpio_init();
}
EXPORT_SYMBOL_GPL(set_spi_pin);

void spi_pin_free(void)
{
	gpio_free(pin.miso);
	gpio_free(pin.mosi);
	gpio_free(pin.sclk);
	gpio_free(pin.cs);
}
EXPORT_SYMBOL_GPL(spi_pin_free);

void set_spi_mode(int mode)
{

	switch (mode)
	{
	case SPI_MODE_0:
	case SPI_MODE_1:
		gpio_direction_output(pin.sclk, LOW);
		break;

	case SPI_MODE_2:
	case SPI_MODE_3:
		gpio_direction_output(pin.sclk, HIGH);
		break;

	default:
		pr_err("Not support spi mode %d\n", mode);
		gpio_direction_output(pin.sclk, LOW);
		break;
	}

	gpio_direction_input(pin.miso);
	gpio_direction_output(pin.mosi, LOW);
	gpio_direction_output(pin.cs, HIGH);
}
EXPORT_SYMBOL_GPL(set_spi_mode);

static inline void cs_high(void)
{
	gpio_set_value(pin.cs, HIGH);
}

static inline void cs_low(void)
{
	gpio_set_value(pin.cs, LOW);
}

static inline void clk_falling(void)
{
	gpio_set_value(pin.sclk, LOW);
}

static inline void clk_rising(void)
{
	gpio_set_value(pin.sclk, HIGH);
}

static inline void data_set(int val)
{
	gpio_set_value(pin.mosi, val);
}

static inline int data_get(void)
{
	return gpio_get_value(pin.miso);
}

static void spi_transfer_rise_edge(struct spi_data *data)
{
	int val;
	size_t i = 0;
	unsigned char *tx_buf = data->tx_buf;
	unsigned char *rx_buf = data->rx_buf;
	ssize_t n = data->n;
	unsigned char tx_tmp, rx_tmp = 0;
	/* data begin, data end, data step */
	int x, y, z;

	DATA_ENTRY_MODE(data->mode & SPI_LSB_FIRST);

	// 0&3
	// tx
	while (n > 0)
	{
		tx_tmp = *tx_buf;
		tx_buf++;
		n--;

		// for (i = 7; i >= 0; i--)
		for (i = x; i != y; i += z)
		{
			val = (tx_tmp >> i) & 0x1;
			data_set(val);
			TIMING;
			clk_rising();
			TIMING;
			rx_tmp |= (data_get() << i);
			clk_falling();
		}

		*rx_buf = rx_tmp;
		rx_tmp = 0;
		rx_buf++;
	}
}

static void spi_transfer_fall_edge(struct spi_data *data)
{
	int val;
	size_t i = 0;
	unsigned char *tx_buf = data->tx_buf;
	unsigned char *rx_buf = data->rx_buf;
	ssize_t n = data->n;
	unsigned char tx_tmp, rx_tmp = 0;
	/* data begin, data end, data step */
	int x, y, z;

	DATA_ENTRY_MODE(data->mode & SPI_LSB_FIRST);

	// 1&2
	// tx
	while (n > 0)
	{
		tx_tmp = *tx_buf;
		tx_buf++;
		n--;

		// for (i = 7; i >= 0; i--)
		for (i = x; i != y; i += z)
		{
			val = (tx_tmp >> i) & 0x1;
			data_set(val);
			TIMING;
			clk_falling();
			TIMING;
			rx_tmp |= (data_get() << i);
			clk_rising();
		}

		*rx_buf = rx_tmp;
		rx_tmp = 0;
		rx_buf++;
	}
}

void spi_data_swap(struct spi_data *data)
{
	cs_low();

	switch (data->mode & SPI_MODE_3)
	{
	case SPI_MODE_0:
		gpio_set_value(pin.sclk, LOW);
		spi_transfer_rise_edge(data);
		gpio_set_value(pin.sclk, LOW);
		break;

	case SPI_MODE_1:
		gpio_set_value(pin.sclk, HIGH);
		spi_transfer_fall_edge(data);
		gpio_set_value(pin.sclk, LOW);
		break;

	case SPI_MODE_2:
		gpio_set_value(pin.sclk, HIGH);
		spi_transfer_fall_edge(data);
		gpio_set_value(pin.sclk, HIGH);
		break;

	case SPI_MODE_3:
		gpio_set_value(pin.sclk, LOW);
		spi_transfer_rise_edge(data);
		gpio_set_value(pin.sclk, HIGH);
		break;

	default:
		pr_err("Not support spi mode %x\n", mode);
		break;
	}

	cs_high();
}
EXPORT_SYMBOL_GPL(spi_data_swap);

MODULE_AUTHOR("Chowe");
MODULE_DESCRIPTION("GPIO to SPI");
MODULE_LICENSE("GPL");
