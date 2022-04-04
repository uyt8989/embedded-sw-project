CC=arm-none-linux-gnueabi-gcc
#CC=gcc

.SUFFIXES: .c .h .o

DEPS = mylib.h 
CFLAGS=

TARGET = app
OBJDIR = build/

_OBJS = input_process.o output_process.o main.o main_process.o sema.o
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