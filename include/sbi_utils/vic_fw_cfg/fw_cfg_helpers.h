#ifndef FW_CFG_HELPERS_H
#define FW_CFG_HELPERS_H

#include <sbi/riscv_io.h>
#include <sbi/sbi_types.h>
#include <sbi/sbi_byteorder.h>

extern uint64_t fw_cfg_ptr;

struct __attribute__((__packed__)) qemu_cfg_dma_access {
	uint32_t control;
	uint32_t length;
	uint64_t address;
};

struct qemu_cfg_file {
	uint32_t size;
	uint16_t select;
	uint16_t reserved;
	char name[56];
};

struct __attribute__((__packed__)) qemu_cfg_ramfb_cfg {
	uint64_t addr;
	uint32_t fourcc;
	uint32_t flags;
	uint32_t width;
	uint32_t height;
	uint32_t stride;
};

#define DATA (void *)(fw_cfg_ptr)
#define SELECTOR (void *)(fw_cfg_ptr + 0x8)
#define ADDR (void *)(fw_cfg_ptr + 0x10)

#define QEMU_CFG_FILE_DIR 0x19

// DMA control bits
#define QEMU_CFG_DMA_CTL_ERROR 0x01
#define QEMU_CFG_DMA_CTL_READ 0x02
#define QEMU_CFG_DMA_CTL_SKIP 0x04
#define QEMU_CFG_DMA_CTL_SELECT 0x08
#define QEMU_CFG_DMA_CTL_WRITE 0x10

#define readq_be(addr) (BSWAP64(readq(addr)))
#define writeq_be(value, addr) writeq(BSWAP64(value), addr)

void qemu_cfg_dma_transfer(void *address, uint32_t length, uint32_t control);
void qemu_cfg_read(void *buf, int len);
void qemu_cfg_read_entry(void *buf, int e, int len);
void qemu_cfg_write_entry(void *buf, int e, int len);

#endif // FW_CFG_HELPERS_H
