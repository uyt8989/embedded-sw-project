OBJECTS = main.o input_process.o main_process.o output_process.o sema.o
SRCS = main.c input_process.c main_process.c output_process.c sema.c
 
CC = arm-none-linux-gnueabi-gcc
CFLAGS = -static -c -W
 
TARGET = app

$(TARGET) : $(OBJECTS)
$(CC) : -o $(TARGET) $(OBJECTS)
 
clean : 
	rm -f $(OBJECTS) $(TARGET)
 
main.o : mylib.h main.c
input_process.o : mylib.h input_process.c
main_process.o : mylib.h main_process.c
output_process.o : mylib.h output_process.c
sema.o : mylib.h sema.c