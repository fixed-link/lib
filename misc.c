
// lsb2msb or msb2lsb
typedef unsigned char SHIFTINT;

SHIFTINT shift_bit(SHIFTINT data)
{
	SHIFTINT ret = 0;
	unsigned ret_bits = 0;
	unsigned data_bits = sizeof(SHIFTINT) * 8;

	while (data_bits-- > 0)
		ret |= ((data >> data_bits) & 1) << ret_bits++;

	return ret;
}

// traverse all child device node
static void get_of_child(struct device_node *np)
{
	struct device_node *child;

	for_each_child_of_node(np, child)
	{
		if (child->name == "A")
		{
			// to do
			continue;
		}

		if (child->name == "B")
		{
			// to do
			continue;
		}
	}
}

// get time
#include <sys/time.h>
void get_time(void)
{

	struct timeval tv;
	gettimeofday(&tv, NULL);
	tv.tv_sec * 1000000 + tv.tv_usec;
}

// get gcc version
#include <gnu/libc-version.h>
char *gcc_version(void)
{
	return gnu_get_libc_version();
}
