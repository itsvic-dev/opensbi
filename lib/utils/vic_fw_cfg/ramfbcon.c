#include "sbi/sbi_console.h"
#include "sbi/sbi_string.h"
#include <sbi/sbi_types.h>

static uint32_t *fb;
static int x = 0;
static int y = 0;

extern unsigned char Tamsyn8x16r_psf[];

struct __attribute__((__packed__)) psf2_header {
	uint32_t magic;		/* magic bytes to identify PSF */
	uint32_t version;	/* zero */
	uint32_t headersize;	/* offset of bitmaps in file, 32 */
	uint32_t flags;		/* 0 if there's no unicode table */
	uint32_t numglyph;	/* number of glyphs */
	uint32_t bytesperglyph; /* size of each glyph */
	uint32_t height;	/* height in pixels */
	uint32_t width;		/* width in pixels */
};

struct psf2_header *font;

#define BIT_SET(x, b) (((x) >> (7 - (b))) & 1)

#define WIDTH CONFIG_QEMU_RAMFB_WIDTH
#define HEIGHT CONFIG_QEMU_RAMFB_HEIGHT

#define FB_XY(x, y) (fb[(y) * WIDTH + (x)])

static void ramfbcon_putc(char ch)
{
	if (ch == '\r')
		return;

	if (ch == '\n') {
		x = 0;
		y += 15;
		goto y_bounds_check;
	}

	uint8_t *glyph =
		(uint8_t *)font + font->headersize + ch * font->bytesperglyph;

	for (int j = 0; j < 16; j++) {
		for (int i = 0; i < 8; i++) {
			if (BIT_SET(glyph[j], i)) {
				FB_XY(x + i, y + j) = 0xffffff;
			}
		}
	}

	x += 8;
	if (x >= WIDTH) {
		y += 15;
		x = 0;
	}

y_bounds_check:
	if ((y + 15) >= HEIGHT) {
		uint64_t offset = WIDTH * 15;
		uint64_t count	= WIDTH * (HEIGHT - 15) * 4;
		sbi_memmove(fb, fb + offset, count);
		sbi_memset(&FB_XY(0, HEIGHT - 15), 0, WIDTH * 15 * 4);

		y -= 15;
	}
}

static struct sbi_console_device ramfb_console = {
	.name	      = "QEMU ramfb",
	.console_putc = ramfbcon_putc,
};

void ramfb_console_init(uint32_t *framebuffer)
{
	fb = framebuffer;

	font = (struct psf2_header *)&Tamsyn8x16r_psf;
	if (font->magic != 0x864ab572) {
		sbi_printf("fucked up psf\n");
	}

	sbi_printf("switching to ramfb console\n");

	sbi_console_set_device(&ramfb_console);
}
