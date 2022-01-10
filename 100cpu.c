#include <stdlib.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <sys/wait.h>

/* PI/2 = ∑ (n! / (2n+1)!!) */

int main()
{
	int proc;
	pid_t pid[1024];

	proc = get_nprocs();

	for (size_t i = 0; i < proc; i++)
	{
		pid[i] = fork();
		if (pid[i] == 0)
		{
			long a = 10000, b = 0, c = 280000, d, e = 0, f[280001], g;

			for (; b - c;)
				f[b++] = a / 5;

			for (; d = 0, g = c * 2; c -= 14 /*, printf("%.4d",e+d/a), e = d % a */ )
				for (b = c; d += f[b] * a, f[b] = d % --g, d /= g--, --b; d *= b)
					;

			exit(0);
		}
	}

	for (size_t i = 0; i < proc; i++)
		waitpid(pid[i], NULL, 0);

	return 0;
}
