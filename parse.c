#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define MAX_SIZE 1024

int parse(const char *filename, int (*action)(const char *name, const char *value));

int parse(const char *filename, int (*action)(const char *name, const char *value))
{
	char buf[MAX_SIZE];
	char *tmp;
	char name[128];
	char val[384];
	FILE *fp;
	size_t i = 0;

	fp = fopen(filename, "r");
	if (fp == NULL)
	{
		return -EBADF;
	}

	while (fgets(buf, MAX_SIZE, fp) != NULL)
	{

		for (tmp = buf; *tmp == ' ' || *tmp == '\t'; tmp++)
			;

		if ((*tmp == '#') || (*tmp == '=') || (*tmp == '\0') || (*tmp == '\n'))
			continue;

		for (i = 0; *tmp != '='; tmp++, i++)
		{
			name[i] = *tmp;
		}
		while (name[i - 1] == ' ' || name[i - 1] == '\t')
			i--;
		name[i] = '\0';

		for (tmp++, i = 0; (*tmp != '\0') && (*tmp != '\n'); tmp++, i++)
		{
			if (*tmp == ' ' || *tmp == '\t')
			{
				i--;
				continue;
			}
			val[i] = *tmp;
		}
		while (val[i - 1] == ' ' || val[i - 1] == '\t')
			i--;
		val[i] = '\0';

		action(name, val);
	}

	return 0;
}

int msg(const char *name, const char *value)
{
	// printf("%s=%s\n", name, value);
	return 0;
}

int main(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;

	parse(argv[1], msg);
	return 0;
}
