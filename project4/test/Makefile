CC := arm-none-linux-gnueabi-gcc

app:
	arm-none-linux-gnueabi-gcc -static -o test test.c

.PHONY: clean
clean : 
	rm -rf test

.PHONY: push
push:
	make
	adb push test /data/local/tmp