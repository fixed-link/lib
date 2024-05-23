#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>

typedef struct
{
	short type;
	int size;
	int reserved;
	int off_bits;
} __attribute__((packed)) BITMAPFILEHEADER;

typedef struct
{
	int size;
	int width;
	int height;
	short planes;
	short bit_count;
	int compress;
	int size_image;
	int x_pixel_per_meter;
	int y_pixel_per_meter;
	int color_used;
	int color_important;
} __attribute__((packed)) BITMAPINFOHEADER;

typedef struct
{
	void *fbp;
	int width;
	int height;
	int mem_len;
} fb_desc;

#define COLOR_DEPTH 32
#define DISP_WIDTH 720
#define DISP_HEIGHT 1280
#define DEVICE_NAME "/dev/fb0"

#if (COLOR_DEPTH == 32)
#define RED 0xFFFF0000
#define GREEN 0xFF00FF00
#define BLUE 0xFF0000FF
#define YELLOW 0xFFFFFF00
#define WHITE 0xFFFFFFFF
#define BLACK 0xFF000000
#define ARGB_PREFIX 0xFF000000
#endif

void set_res(struct fb_var_screeninfo *vinfo)
{
	vinfo->xoffset = 0;
	vinfo->yoffset = 0;
	vinfo->xres = DISP_WIDTH;
	vinfo->yres = DISP_HEIGHT;
	vinfo->xres_virtual = DISP_WIDTH;
	vinfo->yres_virtual = DISP_HEIGHT * 2;
	vinfo->bits_per_pixel = COLOR_DEPTH;
}

void show_res(struct fb_var_screeninfo *vinfo)
{
	printf("xres = %d\n", vinfo->xres);
	printf("yres = %d\n", vinfo->yres);
	printf("xres_virtual = %d\n", vinfo->xres_virtual);
	printf("yres_virtual = %d\n", vinfo->yres_virtual);
	printf("bits_per_pixel = %d\n", vinfo->bits_per_pixel);
}

#if (COLOR_DEPTH == 32)
void fill_color(uint32_t *addr, uint32_t color, uint32_t size)
{
	for (uint32_t i = 0; i < size; i++)
		*(addr + i) = color;
}

void fill_bmp(uint32_t *addr, const uint8_t *src, uint32_t size)
{
	uint32_t color;

	for (uint32_t i = 0; i < size; i++, src += 3)
	{
		color = ARGB_PREFIX | *(src + 2) << 16 | *(src + 1) << 8 | *(src + 0);
		*(addr + i) = color;
	}
}
#endif

int load_bmp(const char *path, int width, int height, void **bmp_src)
{
	int sfd = -1;
	void *buf = NULL;
	int bpp;
	int min_height;
	BITMAPFILEHEADER filehead;
	BITMAPINFOHEADER infohead;

	if (access(path, F_OK) == F_OK)
		sfd = open(path, O_RDONLY);

	if (sfd > 0)
	{
		read(sfd, &filehead, sizeof(filehead));
		read(sfd, &infohead, sizeof(infohead));

		printf("infohead.width = %d\n", infohead.width);
		printf("infohead.height = %d\n", infohead.height);
		printf("infohead.bit_count = %d\n", infohead.bit_count);

		bpp = infohead.bit_count / 8;
		if (bpp != 3)
		{
			printf("Not support bmp file\n");
			close(sfd);
			return -1;
		}

		buf = malloc(width * height * bpp);
		if (buf == NULL)
		{
			close(sfd);
			return errno;
		}

		*bmp_src = buf;
		memset(buf, 0x00, width * height * bpp);

		min_height = infohead.height < height ? infohead.height : height;

		if (infohead.height > height)
			lseek(sfd, (infohead.height - height) * infohead.width * bpp, SEEK_CUR);

		if (infohead.width <= width)
		{
			for (size_t i = 1; i <= min_height; i++)
			{
				buf = *bmp_src + (min_height - i) * width * bpp;
				read(sfd, buf, infohead.width * bpp);
			}
		}
		else
		{
			for (size_t i = 1; i <= min_height; i++)
			{
				buf = *bmp_src + (min_height - i) * width * bpp;
				read(sfd, buf, width * bpp);
				lseek(sfd, (infohead.width - width) * bpp, SEEK_CUR);
			}
		}

		return 0;
	}
	else
		return errno;
}

int fb_init(const char *dev, fb_desc *fbd)
{
	int fd;
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;

	fd = open(dev, O_RDWR);
	if (fd < 0)
		return errno;

	if (ioctl(fd, FBIOGET_FSCREENINFO, &finfo))
	{
		perror("FBIOGET_FSCREENINFO failed");
		goto out;
	}

	if (ioctl(fd, FBIOGET_VSCREENINFO, &vinfo))
	{
		perror("FBIOGET_VSCREENINFO failed");
		goto out;
	}

	set_res(&vinfo);
	if (ioctl(fd, FBIOPUT_VSCREENINFO, &vinfo))
	{
		perror("FBIOPUT_VSCREENINFO failed");
		goto out;
	}

	if (ioctl(fd, FBIOGET_VSCREENINFO, &vinfo))
	{
		perror("FBIOGET_VSCREENINFO failed");
		goto out;
	}
	show_res(&vinfo);

	fbd->fbp = (void *)mmap(0, finfo.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (fbd->fbp == MAP_FAILED)
	{
		perror("failed to mmap framebuffer!");
		goto out;
	}

	fbd->width = vinfo.xres;
	fbd->height = vinfo.yres;
	fbd->mem_len = finfo.smem_len;
	memset(fbd->fbp, 0x00, finfo.smem_len);

	return 0;

out:
	close(fd);
	return errno;
}

int main(int argc, char *argv[])
{
	fb_desc fbd;
	int ret;
	int disp_size;
	void *bmp_src = NULL;

	ret = fb_init(DEVICE_NAME, &fbd);
	if (ret)
		return ret;

	disp_size = fbd.height * fbd.width;

	if (argv[1])
	{
		ret = load_bmp(argv[1], fbd.width, fbd.height, &bmp_src);
		if (!ret)
			fill_bmp(fbd.fbp, bmp_src, disp_size);

		getchar();
	}
	else
	{
		while (1)
		{
			fill_color(fbd.fbp, RED, disp_size);
			sleep(1);
			fill_color(fbd.fbp, GREEN, disp_size);
			sleep(1);
			fill_color(fbd.fbp, BLUE, disp_size);
			sleep(1);
		}
	}

	if (bmp_src)
		free(bmp_src);
	munmap(fbd.fbp, fbd.mem_len);
	return 0;
}