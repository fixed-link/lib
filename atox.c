
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int xtoi(const char *s)
{
	int len;
	int x = 0;

	len = strlen(s);
	for (size_t i = 0; i < len; i++)
	{
		x <<= 4;
		if ((*(s + i) > 47) && (*(s + i) < 58))
		{
			x = x + (*(s + i)) - 48;
			continue;
		}

		if ((*(s + i) > 64) && (*(s + i) < 71))
		{
			x = x + (*(s + i)) - 55;
			continue;
		}

		if ((*(s + i) > 96) && (*(s + i) < 103))
		{
			x = x + (*(s + i)) - 87;
			continue;
		}

		x = 0;
		break;
	}

	return x;
}

int otoi(const char *s)
{
	int len;
	int i = 0;
	int o = 0;

	len = strlen(s);

	while ((len > 0) && (*(s + i) > 47) && (*(s + i) < 56))
	{
		o <<= 3;
		o = o + (*(s + i)) - 48;
		i++;
		len--;
	}

	if (len == 0)
		return o;
	else
		return 0;
}

int btoi(const char *s)
{
	int len;
	int i = 0;
	int b = 0;

	len = strlen(s);
	while ((len > 0) && (*(s + i) > 47) && (*(s + i) < 50))
	{
		b <<= 1;
		b = b + (*(s + i)) - 48;
		i++;
		len--;
	}

	if (len == 0)
		return b;
	else
		return 0;
}

int str_to_int(const char *s)
{
	int len;

	len = strlen(s);
	switch (len)
	{
	case 0:
		return 0;
		break;

	case 1:
		return atoi(s);
		break;

	default:
		break;
	}

	switch (*s)
	{
	case '0':
		break;

	default:
		return atoi(s);
		break;
	}

	switch (*(s + 1))
	{
	case 'x':
	case 'X':
		return xtoi(s + 2);
		break;

	case 'b':
	case 'B':
		return btoi(s + 2);
		break;

	default:
		return otoi(s + 1);
		break;
	}
}

int main(int argc, char *argv[])
{
	return printf("%d\n", str_to_int(argv[1]));
}
