
ssize_t rw_in_kernel(const char *path)
{
	struct file *fd = NULL;
	mm_segment_t fs;
	loff_t pos = 0;
	char *swap;
	int ret;

	swap = (char *)kzalloc(PAGE_SIZE, GFP_KERNEL);
	if (IS_ERR(swap))
		return -ENOMEM;

	fd = filp_open(path, O_RDWR | O_CREAT, 0644);
	if (IS_ERR(fd))
	{
		printk("open %s FAILED. %d\n", path, fd);
		return -1;
	}

	fs = get_fs();
	set_fs(get_ds());

	do
	{
		ret = vfs_read(fd, swap, PAGE_SIZE, &pos);
		// ret = vfs_write(fd, swap, PAGE_SIZE, &pos);
	} while (ret > 0);

	filp_close(fd, NULL);
	set_fs(fs);
	kfree(swap);

	return ret;
}
