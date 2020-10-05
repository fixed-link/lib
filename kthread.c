#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/jiffies.h>
#include <linux/delay.h>

static struct task_struct *task = NULL;

static int kthread_sample(void *data)
{
	do
	{
		printk("kthread sample running... \n");
		set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout(msecs_to_jiffies(1000));
	} while (!kthread_should_stop());

	return 0;
}

static int __init kthread_sample_init(void)
{
	task = kthread_run(kthread_sample, NULL, "kthread_sample");
	if (IS_ERR(task))
		return -EBUSY;

	return 0;
}

static void __exit kthread_sample_exit(void)
{
	if (task != NULL && (!IS_ERR(task)))
	{
		kthread_stop(task);
	}
}

module_init(kthread_sample_init);
module_exit(kthread_sample_exit);

MODULE_DESCRIPTION("kthread Sample Driver");
MODULE_AUTHOR("Mr.Chowe");
MODULE_LICENSE("GPL v3");
