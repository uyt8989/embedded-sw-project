CC := arm-none-linux-gnueabi-gcc

app:
	arm-none-linux-gnueabi-gcc -static -o app app.c

.PHONY: clean
clean : 
	rm -rf app

.PHONY: push
push:
	make
	adb push app /data/local/tmp