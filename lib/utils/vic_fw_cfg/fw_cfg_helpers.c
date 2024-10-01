#include "sbi/sbi_byteorder.h"
#include <sbi/sbi_types.h>
#include <sbi_utils/vic_fw_cfg/fw_cfg_helpers.h>

void qemu_cfg_dma_transfer(void *address, uint32_t length, uint32_t control)
{
	struct qemu_cfg_dma_access access = {
		.address = BSWAP64((uint64_t)address),
		.length	 = BSWAP32(length),
		.control = BSWAP32(control),
	};

	if (length == 0)
		return;

	writeq_be((uint64_t)&access, ADDR);
	while (BSWAP32(access.control) & ~QEMU_CFG_DMA_CTL_ERROR) {
	}
}

void qemu_cfg_read(void *buf, int len)
{
	qemu_cfg_dma_transfer(buf, len, QEMU_CFG_DMA_CTL_READ);
}

void qemu_cfg_read_entry(void *buf, int e, int len)
{
	uint32_t control =
		(e << 16) | QEMU_CFG_DMA_CTL_SELECT | QEMU_CFG_DMA_CTL_READ;
	qemu_cfg_dma_transfer(buf, len, control);
}

void qemu_cfg_write_entry(void *buf, int e, int len)
{
	uint32_t control =
		(e << 16) | QEMU_CFG_DMA_CTL_SELECT | QEMU_CFG_DMA_CTL_WRITE;
	qemu_cfg_dma_transfer(buf, len, control);
}
