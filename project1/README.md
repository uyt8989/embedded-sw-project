# Embeded System Software 과제1 
## 20171659 유경연

### Description

아래의 네 가지 모드를 구현하였다. <br>
각 모드는 사용자의 입력을 받아 적절한 기능을 수행한다. <br>

1. Clock mode
	- FND : Current time
	- sw1 : Change time
	- sw2 : Set to current time
	- sw3 : Add 1 hour
	- sw4 : Add 1 minute
2. Counter mode
	- FND : Current counter value 
	- sw1 : Change notations (10, 8, 4, 2)
	- sw2 : Add 1 to hundreds
	- sw3 : Add 1 to tens
	- sw4 : Add 1 to units
3. Text Editor mode
	- FND : Counts of inputs
	- LCD : Text edior
	- Dot Matrix : Current input mode
	- sw1~9 : Input alphbets and numbers
	- sw2&3 : Clear the LCD
	- sw5&6 : Change input mode
	- sw8&9 : Input space character
4. Draw Board mode
	- FND : Counts of inputs
	- Dot Matrix : Current drawing
	- sw2,4,6,8 : Arrow keys
	- sw5 : Select
	- sw1 : Reset
	- sw3 : On/Off cursor
	- sw7 : Clear drawing
	- sw9 : Reverse drawing
	
### Environment
- Host PC : Linux (Ubuntu 16.04) System on x64 architecture
- Target Board : Embeded Linux System on ARM architecture
- Compiler : arm-none-linux-gnueabi-gcc

### Prerequisite
Device Driver : 
1. /dev/input/event0
2. /dev/fpga_push_switch 
3. /dev/fpga_fnd
4. /dev/fpga_dot
5. /dev/fpga_text_lcd
6. /dev/mem

### Usage
1. make push (Host)
2. /data/local/tmp  ./app (Device)