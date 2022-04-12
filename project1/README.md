# Embeded System Software 과제1 
## 20171659 유경연

### Description

아래의 네 가지 모드를 구현하였다. <br>
각 모드는 사용자의 입력을 받아 적절한 기능을 수행한다. <br>

1. Clock mode
2. Counter mode
3. Text Editor mode
4. Draw Board mode

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