/*
1. 列出网卡列表ls
2. 检查网卡状态
3. 如果up->down，删除route
4. 如果down->up，增加网关（可选调用dhcp）
5. 回到第2步
*/

#define _DEFAULT_SOURCE
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <ctype.h>

#define DEBUG 0

#define NET_PATH "/sys/class/net/"
#define NET_STATUS "/operstate"

#define NET_UP "up"
#define NET_DOWN "down"

#define NET_CONNECT 1
#define NET_DISCONNECT 0
#define NET_UNKNOWN -1

#define IP_NONE "0.0.0.0"
#define IP_CONFIG "/etc/ip"

struct net_status
{
	char name[16];
	char ip[16];
	int status;
};

struct net_status eth[16];
int eth_count = 0;

int dhcp = 0;

int check_ip(const char *ip)
{
	int dots = 0;
	int setions = 0;

	if (NULL == ip || *ip == '.')
	{
		return -1;
	}

	while ((*ip != '\n') && (*ip != '\0'))
	{
		if (*ip == '.')
		{
			dots++;
			if (setions >= 0 && setions <= 255)
			{
				setions = 0;
				ip++;
				continue;
			}
			return -1;
		}
		else if (*ip >= '0' && *ip <= '9')
		{
			setions = setions * 10 + (*ip - '0');
		}
		else
			return -1;
		ip++;
	}

	if (setions >= 0 && setions <= 255)
	{
		if (dots == 3)
		{
			return 0;
		}
	}

	return -1;
}

void get_default_ip(char *ip)
{
	FILE *fp;
	fp = fopen(IP_CONFIG, "r");

	if (!fp)
	{
		// perror("fopen");
		strcpy(ip, IP_NONE);
		return;
	}

	fgets(ip, 16, fp);
	fclose(fp);
	// puts(ip);
	if (check_ip(ip) < 0)
	{
		strcpy(ip, IP_NONE);
	}
}

void get_ip(struct net_status *ns)
{
	int sd;
	struct sockaddr_in sin;
	struct ifreq ifr;

	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sd < 0)
	{
		perror("socket");
		return;
	}

	strcpy(ifr.ifr_name, ns->name);

	if (ioctl(sd, SIOCGIFADDR, &ifr) < 0)
	{
		// perror("socket");
		strcpy(ns->ip, IP_NONE);
	}
	else
	{
		strcpy(ns->ip, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
	}

	close(sd);
}

void set_ip(const char *ifname, const char *ip)
{
	char buf[64];
	sprintf(buf, "ifconfig %s %s", ifname, ip);
	system(buf);
}

void net_debug()
{
	int i = eth_count;
	while (i-- > 0)
	{
		puts(eth[i].name);
		printf("status = %d\n", eth[i].status);
	}
}

void net_change(struct net_status *ns, int status)
{
	char buf[32];
	ns->status = status;
	switch (status)
	{
	case NET_CONNECT:
		if (dhcp)
		{
			system("killall udhcpc");
			sprintf(buf, "udhcpc -i %s &", ns->name);
			system(buf);
		}
		else
		{
			set_ip(ns->name, ns->ip);
		}
		fprintf(stderr, "%s: going to online", ns->name);
		break;

	case NET_DISCONNECT:
		get_ip(ns);
		set_ip(ns->name, IP_NONE);
		fprintf(stderr, "%s: going to offline", ns->name);
		break;

	default:
		fprintf(stderr, "%s: going to unkown", ns->name);
		break;
	}
#if DEBUG
	puts(ns->name);
	printf("status = %d\n", status);
#endif
}

void net_check(const char *ifname, int status)
{
	int i = eth_count;
	while (i-- > 0)
	{
		if (strcmp(eth[i].name, ifname) == 0)
			break;
	}
	if (i >= 0)
	{
		if (eth[i].status != status)
			net_change(&eth[i], status);
	}
}

void net_status(const char *path, const char *ifname)
{
	FILE *fp = NULL;
	char buf[16];
	int status = -1;

	fp = fopen(path, "r");
	if (!fp)
	{
		perror("fopen");
		puts(__func__);
		return;
	}

	fgets(buf, 16, fp);
#if DEBUG
	puts(buf);
#endif

	if (strncmp(buf, NET_UP, 2) == 0)
	{
		// puts("UP");
		status = NET_CONNECT;
	}
	else if (strncmp(buf, NET_DOWN, 4) == 0)
	{
		// puts("DOWN");
		status = NET_DISCONNECT;
	}
	else
	{
		// puts("failed");
		status = NET_UNKNOWN;
	}

	net_check(ifname, status);

	fclose(fp);
}

void net_ls(void)
{
	struct dirent **namelist;
	int n = eth_count;
	char buf[32];
	while (n-- > 0)
	{
		sprintf(buf, NET_PATH "%s" NET_STATUS, eth[n].name);
		net_status(buf, eth[n].name);
	}
}

void net_init(void)
{
	struct dirent **namelist;
	int n;
	int i = 0;
	char buf[32];
	FILE *fp = NULL;

	n = scandir(NET_PATH, &namelist, NULL, alphasort);
	if (n == -1)
	{
		perror("scandir");
		exit(EXIT_FAILURE);
	}

	while (n-- > 2)
	{
		if (strcmp(namelist[n]->d_name, "lo") == 0)
			continue;

		sprintf(buf, NET_PATH "%s" NET_STATUS, namelist[n]->d_name);
#if DEBUG
		printf("%s\n", buf);
#endif

		strcpy(eth[i].name, namelist[n]->d_name);
		free(namelist[n]);

		get_default_ip(eth[i].ip);
		// puts(eth[i].ip);

		fp = fopen(buf, "r");
		if (!fp)
		{
			perror("fopen");
			puts(__func__);
			continue;
		}

		fgets(buf, 16, fp);

		if (strncmp(buf, NET_UP, 2) == 0)
		{
			eth[i].status = NET_CONNECT;
		}
		else if (strncmp(buf, NET_DOWN, 4) == 0)
		{
			eth[i].status = NET_DISCONNECT;
		}
		else
		{
			eth[i].status = NET_UNKNOWN;
		}
		fclose(fp);
		i++;
	}
	free(namelist);
	eth_count = i;
}

int main(int argc, char *argv[])
{
	net_init();
	puts("init done");
#if DEBUG
	net_debug();
#endif
	while (1)
	{
		sleep(1);
		net_ls();
	}

	return 0;
}