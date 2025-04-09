#include <stdlib.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>

// 计算π的算法
void calculate_pi(void)
{
	long a = 10000, b = 0, c = 280000, d, f[280001], g;

	for (; b < c; b++)
		f[b] = a / 5;

	for (; g = c * 2; c -= 14)
	{
		for (d = 0, b = c; b > 0; b--)
		{
			d += f[b] * a;
			f[b] = d % --g;
			d /= g--;
			d *= b;
		}
	}
}

// 创建计算π的子进程
pid_t create_pi_process(void)
{
	pid_t pid = fork();
	if (pid == 0)
	{
		while (1)
		{
			calculate_pi();
		}
		// 理论上不会执行到这里
		exit(EXIT_FAILURE);
	}
	return pid;
}

int main(int argc, char *argv[])
{
	unsigned num_procs = get_nprocs();
	pid_t pids[num_procs];
	int sig;
	sigset_t set;

	sigemptyset(&set);
	sigaddset(&set, SIGINT);

	// 创建与CPU核心数相同的计算进程
	for (size_t i = 0; i < num_procs; i++)
	{
		pids[i] = create_pi_process();
		if (pids[i] < 0)
		{
			perror("fork failed");
			exit(EXIT_FAILURE);
		}
	}

	printf("Created %d processes. Press Ctrl+C to exit...\n", num_procs);
	sigwait(&set, &sig);

	// 清理所有子进程
	for (size_t i = 0; i < num_procs; i++)
	{
		if (kill(pids[i], SIGKILL) < 0)
		{
			perror("kill failed");
		}
	}

	return EXIT_SUCCESS;
}