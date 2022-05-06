CC := arm-none-linux-gnueabi-gcc
OBJDIR = build/

app:
	arm-none-linux-gnueabi-gcc -static -o $(OBJDIR)app app.c

.PHONY: clean
clean : 
	rm -rf $(OBJDIR)

.PHONY: push
push:
	make
	adb push $(OBJDIR)app /data/local/tmp