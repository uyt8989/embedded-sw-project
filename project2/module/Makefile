obj-m = dev_driver.o 

KDIR := /work/achroimx_kernel
PWD := $(shell pwd)

driver : 
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

.PHONY: clean
clean : 
	rm -rf *.o
	rm -rf *.ko
	rm -rf *.mod.c
	rm -rf *.order
	rm -rf *.symvers

.PHONY: push
push:
	make
	adb push dev_driver.ko /data/local/tmp