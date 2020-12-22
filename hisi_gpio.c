#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/kernel.h>
#include <linux/string.h>

#define HISI_GPIO_REG 0x12150000
#define GPIO_BASE_ADDR(x) (HISI_GPIO_REG + x * 0x10000)
#define GPIO_DIR_ADDR 0x400
#define GPIO_DATA_ADDR(x) (1 << (x + 2))
#define GPIO_DATA_OUT(x, val) ((val >> x) & 0x01)
#define GPIO_DATA_IN(x, val) (val << x)
#define GPIO_REG_LENGTH 0x10000
#define GPIO_NUM_PER_GROUP 8

int hisi_gpio_direction_input(unsigned gpio);
int hisi_gpio_direction_output(unsigned gpio, int value);
int hisi_gpio_set_value(unsigned gpio, int value);
int hisi_gpio_get_value(unsigned gpio);

static inline void *hisi_gpio_ioremap(int addr)
{
	return ioremap(GPIO_BASE_ADDR(addr), GPIO_REG_LENGTH);
}

static inline void hisi_gpio_iounmap(void *addr)
{
	iounmap(addr);
}

int hisi_gpio_direction_input(unsigned gpio)
{
	void *addr;
	unsigned int val;
	unsigned num = gpio % GPIO_NUM_PER_GROUP;

	addr = hisi_gpio_ioremap(gpio / GPIO_NUM_PER_GROUP);
	if (!addr)
		return -EINVAL;

	val = readl(addr + GPIO_DIR_ADDR);
	val = val & (~(1 << num));
	writel(val, addr + GPIO_DIR_ADDR);

	hisi_gpio_iounmap(addr);
	return 0;
}
EXPORT_SYMBOL(hisi_gpio_direction_input);

int hisi_gpio_direction_output(unsigned gpio, int value)
{
	void *addr;
	unsigned int val;
	unsigned num = gpio % GPIO_NUM_PER_GROUP;

	addr = hisi_gpio_ioremap(gpio / GPIO_NUM_PER_GROUP);
	if (!addr)
		return -EINVAL;

	val = readl(addr + GPIO_DIR_ADDR);
	val = val | (1 << num);
	writel(val, addr + GPIO_DIR_ADDR);

	writel(GPIO_DATA_IN(num, value), addr + GPIO_DATA_ADDR(num));

	hisi_gpio_iounmap(addr);
	return 0;
}
EXPORT_SYMBOL(hisi_gpio_direction_output);

int hisi_gpio_set_value(unsigned gpio, int value)
{
	void *addr;
	unsigned num = gpio % GPIO_NUM_PER_GROUP;

	addr = hisi_gpio_ioremap(gpio / GPIO_NUM_PER_GROUP);
	if (!addr)
		return -EINVAL;

	writel(GPIO_DATA_IN(num, value), addr + GPIO_DATA_ADDR(num));

	hisi_gpio_iounmap(addr);
	return 0;
}
EXPORT_SYMBOL(hisi_gpio_set_value);

int hisi_gpio_get_value(unsigned gpio)
{
	void *addr;
	unsigned int val;
	unsigned num = gpio % GPIO_NUM_PER_GROUP;

	addr = hisi_gpio_ioremap(gpio / GPIO_NUM_PER_GROUP);
	if (!addr)
		return -EINVAL;

	val = readl(addr + GPIO_DATA_ADDR(num));

	hisi_gpio_iounmap(addr);
	return GPIO_DATA_OUT(num, val);
}
EXPORT_SYMBOL(hisi_gpio_get_value);

static ssize_t hisi_gpio_read(struct file *flip, char __user *data, size_t size, loff_t *offset)
{
	return 0;
}

/* usage: echo "gpionum input/output 0/1" > /dev/hisi_gpio */
static ssize_t hisi_gpio_write(struct file *flip, const char __user *data, size_t size, loff_t *offset)
{
	char buf[128];
	int ret;
	int i = 0;
	char gpio[4];
	char op[4];
	char *p;
	unsigned num;
	int value;

	// copy data from user
	ret = copy_from_user(buf, data, size);

	// parse gpio num
	p = gpio;
	while (buf[i] != ' ' && buf[i] != '\0' && buf[i] != '\n')
	{
		*p = buf[i++];
		p++;
	}
	*p = '\0';

	ret |= kstrtouint(gpio, 10, &num);
	if (num < 0 || num >= 200)
		return -1;
	// printk("%d\n", num);

	// parse direction
	while (buf[i++] == ' ')
		;
	p = op;
	i--;
	while (buf[i] != ' ' && buf[i] != '\0' && buf[i] != '\n')
	{
		*p = buf[i++];
		p++;
	}
	*p = '\0';

	// if direction == output, get value
	if (strncmp(op, "out", 3) == 0)
	{
		while (buf[i++] == ' ')
			;
		p = op;
		i--;
		while (buf[i] != ' ' && buf[i] != '\0' && buf[i] != '\n')
		{
			*p = buf[i++];
			p++;
		}
		*p = '\0';
		ret |= kstrtouint(op, 10, &value);
		value &= 0x01;
		// printk("%d\n", value);
		hisi_gpio_direction_output(num, value);
	}
	// if direction == input
	else if (strncmp(op, "in", 2) == 0)
	{
		hisi_gpio_direction_input(num);
		printk("gpio %d = %d\n", num, hisi_gpio_get_value(num));
	}

	return ret ? ret : size;
}

static long hisi_gpio_ioctl(struct file *flip, unsigned int cmd, unsigned long arg)
{
	return 0;
}

static int hisi_gpio_open(struct inode *inode, struct file *flip)
{
	return 0;
}

static int hisi_gpio_release(struct inode *inode, struct file *flip)
{
	return 0;
}

static struct file_operations hisi_gpio_ops = {
	.owner = THIS_MODULE,
	.read = hisi_gpio_read,
	.write = hisi_gpio_write,
	.unlocked_ioctl = hisi_gpio_ioctl,
	.open = hisi_gpio_open,
	.release = hisi_gpio_release,
};

static struct miscdevice hisi_gpio_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.fops = &hisi_gpio_ops,
	.name = "hisi_gpio",
};

int __init hisi_gpio_init(void)
{
	return misc_register(&hisi_gpio_dev);
}

void __exit hisi_gpio_exit(void)
{
	misc_deregister(&hisi_gpio_dev);
}

module_init(hisi_gpio_init);
module_exit(hisi_gpio_exit);

MODULE_AUTHOR("Chowe");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("the hisi gpio driver");
