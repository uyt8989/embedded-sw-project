# Embeded System Software 과제4
## 20171659 유경연

### Description
Make mini maze escape game
Device driver name : /dev/maze
Device driver major number : 242

### Environment
- Host PC : Linux (Ubuntu 16.04) System on x64 architecture
- Target Board : Embeded Linux System on ARM architecture (Linux 3.0.35)
- Compiler : arm-none-linux-gnueabi-gcc

### Test
#### Host(Terminal)
1. kernel compile configure
4. cd /module
5. make push
6. then maze.ko are transfered to device.

#### Device
1. If you want to see logs during running, echo "7 6 1 7" > /proc/sys/kernel/printk
2. cd /data/local/tmp
3. insmod maze.ko
4. mknod /dev/maze c 242 0
5. chmod 777 /dev/maze

#### Mannual
