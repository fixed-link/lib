#undef NDEBUG
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>
#include <sys/ioctl.h>
#include <getopt.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>

#define I2C_16BIT_REG 0x0709  /* 16BIT REG WIDTH */
#define I2C_16BIT_DATA 0x070a /* 16BIT DATA WIDTH */

#define I2C_DEV "/dev/i2c-0"
#define SLAVE_ADDR 0x98

int open_i2c(void)
{
	int fd;
	int ret;

	fd = open(I2C_DEV, O_RDWR);
	assert(fd > 0);

	ret = ioctl(fd, I2C_TENBIT, 0);
	assert(ret >= 0);

	ret = ioctl(fd, I2C_SLAVE_FORCE, SLAVE_ADDR);
	assert(ret >= 0);

#ifdef buf_16BIT
	ret = ioctl(fd, I2C_16BIT_DATA, 1);
#else
	ret = ioctl(fd, I2C_16BIT_DATA, 0);
#endif
	assert(ret >= 0);

#ifdef REG_16BIT
	ret = ioctl(fd, I2C_16BIT_REG, 1);
#else
	ret = ioctl(fd, I2C_16BIT_REG, 0);
#endif
	assert(ret >= 0);

	return fd;
}

int main(int argc, char *argv[])
{
	int fd;
	int ret;
	unsigned char buf[128];

	fd = open_i2c();
	memset(buf, 0x0, 128);

	// read
	buf[0] = 0x00;			// reg
	ret = read(fd, buf, 1); // read respond
	assert(ret > 0);
	printf("read data respond = 0x%X\n", buf[0]);

	// write
	buf[0] = 0x00; //reg
	buf[1] = 0x00; //data
	ret = write(fd, buf, 2);
	assert(ret > 0);

	return 0;
}
