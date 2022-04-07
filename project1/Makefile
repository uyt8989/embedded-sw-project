CC=arm-none-linux-gnueabi-gcc
#CC=gcc

.SUFFIXES: .c .o

CFLAGS= -W

TARGET = app
OBJDIR = build/

_OBJS =  main.o input_process.o main_process.o output_process.o sema.o utils.o mode.o 
OBJS = $(addprefix $(OBJDIR),$(_OBJS))

$(OBJDIR)$(TARGET): $(OBJS)
	$(CC) -static -o $(OBJDIR)$(TARGET) $(OBJS)

$(OBJDIR)%.o: %.c $(OBJDIR)
	$(CC) -static -c -o $@ $<

$(OBJDIR):
	mkdir -p $(OBJDIR)

.PHONY: clean
clean:
	rm -rf $(OBJDIR)
	
.PHONY: push
push:
	make
	adb push $(OBJDIR)$(TARGET) /data/local/tmp