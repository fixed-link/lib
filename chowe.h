
#undef NDEBUG
#define NDEBUG

#ifdef __cplusplus
#include <stdio>
#include <stdlib>
#else
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <signal.h>
#endif

#define dbg_info(args)                                     \
	do                                                \
	{                                                 \
		printf("[%s] in [%d]: ", __func__, __LINE__); \
		perror(args);                                 \
	} while (0)

#ifdef __cplusplus
extern "C"
{
#endif

	static int exit_flag = 0;
	static void process_exit(int signal)
	{
		printf("Catch INT signal, cleaning & exiting...\n");
		exit_flag = 1;
	}

#ifdef __cplusplus
}
#endif

	//signal(SIGINT, process_exit); //Make sure this expression excute before loop block