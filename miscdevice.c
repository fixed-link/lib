#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>

static int o_count = 0;

int sample_open(struct inode *inode, struct file *file)
{
	if (o_count)
		return -EBUSY; //Only open one time
	else
	{
		o_count++;
		return 0;
	}
}

int sample_release(struct inode *inode, struct file *file)
{
	o_count--;
	return 0;
}

ssize_t sample_read(struct file *file, char __user *data, size_t len, loff_t *offset)
{
	//sys_read call to do
	return 0;
}

ssize_t sample_write(struct file *file, const char __user *data, size_t len, loff_t *offset)
{
	//sys_write call to do
	return 0;
}

long sample_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	switch (cmd)
	{
	//sys_ioctl call to do
	default:
		return -EIOCTLCMD;
		break;
	}
	return 0;
}

static struct file_operations sample_fops = {
	.owner = THIS_MODULE,
	.open = sample_open,
	.release = sample_release,
	.read = sample_read,
	.write = sample_write,
	.unlocked_ioctl = sample_ioctl,
};

static struct miscdevice sample_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "sample_dev",
	.fops = &sample_fops,
};

static int __init sample_init(void)
{
	int ret = -EBUSY;

	ret = misc_register(&sample_dev);
	if (ret != 0)
	{
		goto exit1;
	}

	return 0;

exit1:
	return ret;
}

static void __exit sample_exit(void)
{
	misc_deregister(&sample_dev);
}

module_init(sample_init);
module_exit(sample_exit);

MODULE_DESCRIPTION("Sample of Misc Device Driver");
MODULE_AUTHOR("Mr.Chowe");
MODULE_LICENSE("GPL v3");
