OBJECTS = main.o input_process.o main_process.o output_process.o sema.o
SRCS = main.c input_process.c main_process.c output_process.c sema.c
 
CC = arm-none-linux-gnueabi-gcc
CFLAGS = -W
 
TARGET = app

$(TARGET) : $(OBJECTS)
<<<<<<< HEAD
	$(CC) -o $(TARGET) $(OBJECTS)
=======
	$(CC) -static -o $(TARGET) $(OBJECTS)
>>>>>>> 0f24497209082e7607a0b41ec21942adfee8d796
 
clean : 
	rm -f $(OBJECTS) $(TARGET)
 
main.o : mylib.h main.c
input_process.o : mylib.h input_process.c
main_process.o : mylib.h main_process.c
output_process.o : mylib.h output_process.c
sema.o : mylib.h sema.c
