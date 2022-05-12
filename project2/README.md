# Embeded System Software 과제2
## 20171659 유경연

### Description
Make new device driver
Device driver name : /dev/dev_driver
Device driver major number : 242

### Environment
- Host PC : Linux (Ubuntu 16.04) System on x64 architecture
- Target Board : Embeded Linux System on ARM architecture
- Compiler : arm-none-linux-gnueabi-gcc

### Usage
#### Host
1. cd /app 
2. make push
3. cd ../module
4. make push
5. then app and dev_driver.ko are transfered to device.

#### Device
1. If you want to see the logs, echo "7 6 1 7" > /proc/sys/kernel/printk
2. cd /data/local/tmp
3. insmod dev_driver.ko
4. mknod /dev/dev_driver c 242 0
5. ./app TIMER_INTERVAL [1-100] TIMER_ CNT [1-100] TIMER_ INIT [0001-8000]