#include "sbi/sbi_byteorder.h"
#include "sbi/sbi_string.h"
#include "sbi_utils/vic_fw_cfg/fw_cfg_helpers.h"
#include <sbi/sbi_types.h>

#define fourcc_code(a, b, c, d)                                         \
	((uint32_t)(a) | ((uint32_t)(b) << 8) | ((uint32_t)(c) << 16) | \
	 ((uint32_t)(d) << 24))
#define DRM_FORMAT_XRGB8888 fourcc_code('X', 'R', '2', '4')

static int find_ramfb_file()
{
	uint32_t count, e, select;
	qemu_cfg_read_entry(&count, QEMU_CFG_FILE_DIR, sizeof(count));
	count = BSWAP32(count);

	for (select = 0, e = 0; e < count; e++) {
		struct qemu_cfg_file file;
		qemu_cfg_read(&file, sizeof(file));
		if (sbi_strncmp(file.name, "etc/ramfb", 10) == 0) {
			select = BSWAP16(file.select);
			break;
		}
	}

	return select;
}

uint32_t framebuffer[CONFIG_QEMU_RAMFB_WIDTH * CONFIG_QEMU_RAMFB_HEIGHT]
	__attribute__((__section__(".ramfb")));

#ifdef CONFIG_QEMU_RAMFB_CONSOLE
extern void ramfb_console_init(uint32_t *framebuffer);
#endif

void qemu_cfg_ramfb_init()
{
	uint32_t select = find_ramfb_file();

	if (select == 0) {
		return;
	}

	struct qemu_cfg_ramfb_cfg cfg = {
		.addr	= BSWAP64((uint64_t)framebuffer),
		.fourcc = BSWAP32(DRM_FORMAT_XRGB8888),
		.flags	= BSWAP32(0),
		.width	= BSWAP32(CONFIG_QEMU_RAMFB_WIDTH),
		.height = BSWAP32(CONFIG_QEMU_RAMFB_HEIGHT),
		.stride = BSWAP32(CONFIG_QEMU_RAMFB_WIDTH * 4),
	};

	qemu_cfg_write_entry(&cfg, select, sizeof(cfg));

#ifdef CONFIG_QEMU_RAMFB_CONSOLE
	ramfb_console_init(framebuffer);
#endif
}
