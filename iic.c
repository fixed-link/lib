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
#include <time.h>
#include <sys/select.h>
#include <sys/time.h>

#define I2C_DEV "/dev/i2c-3"

void usleep(int us)
{
	struct timeval delay;
	delay.tv_sec = 0;
	delay.tv_usec = us;
	select(0, NULL, NULL, NULL, &delay);
}

void msleep(int ms)
{
	usleep(us * 1000);
}

int open_i2c(void)
{
	int fd;

	fd = open(I2C_DEV, O_RDWR);
	assert(fd > 0);

	return fd;
}

int main(int argc, char *argv[])
{
	int fd;
	int ret;
	unsigned char send[128];
	unsigned char recv[128];
	struct i2c_rdwr_ioctl_data rdwr;
	struct i2c_msg msgs[2];

	fd = open_i2c();

	memset(send, 0x0, 128);
	memset(recv, 0x0, 128);

	send[0] = 0x00; // usually reg
	send[1] = 0x01;
	send[2] = 0x02;
	send[3] = 0x03; // send data

	rdwr.msgs = msgs;
	rdwr.nmsgs = 1;

	msgs[0].addr = 0; // slave addr
	msgs[0].flags = 0; // write
	msgs[0].buf = send;
	msgs[0].len = 4;

	ioctl(fd, I2C_RDWR, &rdwr);
	msleep(10);

	// require sending reg before read?
	// there is no required sample
	msgs[0].addr = 0;
	msgs[0].flags = I2C_M_RD; // read
	msgs[0].buf = recv;
	msgs[0].len = 0x4;

	ioctl(fd, I2C_RDWR, &rdwr);

	for (size_t i = 0; i < 0xe; i++)
	{
		printf("%02X\t", recv[i]);
	}
	printf("\n");

	return 0;
}