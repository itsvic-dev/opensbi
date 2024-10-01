libsbiutils-objs-$(CONFIG_QEMU_FW_CFG) += vic_fw_cfg/fdt_fw_cfg.o
libsbiutils-objs-$(CONFIG_QEMU_FW_CFG) += vic_fw_cfg/fw_cfg_helpers.o

libsbiutils-objs-$(CONFIG_QEMU_RAMFB) += vic_fw_cfg/ramfb.o

libsbiutils-objs-$(CONFIG_QEMU_RAMFB_CONSOLE) += vic_fw_cfg/ramfbcon.o
libsbiutils-objs-$(CONFIG_QEMU_RAMFB_CONSOLE) += vic_fw_cfg/tamsyn.o
