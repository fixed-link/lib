#include <stdlib.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>

void process_exit(int signal)
{
    // do nothing, just let it go away.
}

/* PI/2 = ∑ (n! / (2n+1)!!) */
void pi(void)
{
    long a = 10000, b = 0, c = 280000, d, /* e = 0, */ f[280001], g;

    for (; b - c;)
        f[b++] = a / 5;

    for (; d = 0, g = c * 2; c -= 14 /*, printf("%.4d",e+d/a), e = d % a */)
        for (b = c; d += f[b] * a, f[b] = d % --g, d /= g--, --b; d *= b)
            ;
}

pid_t fork_pi(void)
{
    pid_t pid;

    pid = fork();
    if (pid == 0)
    {
        while (1)
        {
            pi();
        }

        exit(0);
        return 0; // never reach
    }
    else
        return pid;
}

int main(int argc, char *argv[])
{
    unsigned proc;
    size_t i = 0;
    pid_t pid[1024];
    int sig;
    sigset_t set;

    proc = get_nprocs();

    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    signal(SIGINT, process_exit);

    for (; i < proc; i++)
    {
        pid[i] = fork_pi();
    }

    sigwait(&set, &sig);
    printf("catch %d\n", sig);

    while (i-- > 0)
    {
        kill(pid[i], SIGKILL);
    }

    return 0;
}
