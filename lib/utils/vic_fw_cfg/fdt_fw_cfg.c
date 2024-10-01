#include "sbi/riscv_io.h"
#include "sbi/sbi_error.h"
#include "sbi_utils/fdt/fdt_helper.h"
#include <libfdt.h>
#include <sbi/sbi_types.h>
#include <sbi_utils/vic_fw_cfg/fw_cfg_helpers.h>

uint64_t fw_cfg_ptr;

#ifdef CONFIG_QEMU_RAMFB
extern void qemu_cfg_ramfb_init();
#endif

int fdt_fw_cfg_init(const void *fdt)
{
	int rc;
	u32 root_offset = fdt_path_offset(fdt, "/");

	int nodeoff = fdt_node_offset_by_compatible(fdt, root_offset,
						    "qemu,fw-cfg-mmio");
	if (nodeoff < 0) {
		return SBI_ENODEV;
	}

	uint64_t reg_size;
	rc = fdt_get_node_addr_size(fdt, nodeoff, 0, &fw_cfg_ptr, &reg_size);
	if (rc < 0 || !fw_cfg_ptr || !reg_size) {
		return SBI_ENODEV;
	}

	writew(0, SELECTOR);
	if (readq_be(ADDR) != 0x51454d5520434647) {
		return SBI_ENODEV;
	}

#ifdef CONFIG_QEMU_RAMFB
	qemu_cfg_ramfb_init();
#endif

	return 0;
}
