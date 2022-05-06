obj-m = dev_driver.o 

OBJDIR = build/
KDIR := /work/achroimx_kernel
PWD := $(shell pwd)

driver : 
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD)$(OBJDIR) modules

.PHONY: clean
clean : 
	rm -rf $(OBJDIR)

.PHONY: push
push:
	make
	adb push $(OBJDIR)dev_driver.ko /data/local/tmp