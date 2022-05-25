# Embeded System Software 과제3
## 20171659 유경연

### Description
Make stopwatch using interrupt
Device driver name : /dev/stopwatch
Device driver major number : 242

### Environment
- Host PC : Linux (Ubuntu 16.04) System on x64 architecture
- Target Board : Embeded Linux System on ARM architecture (Linux 3.0.35)
- Compiler : arm-none-linux-gnueabi-gcc

### Usage
#### Host
1. kernel compile configure
2. cd /app 
3. make push
4. cd ../module
5. make push
6. then app and stopwatch.ko are transfered to device.

#### Device
1. If you want to see logs during running, echo "7 6 1 7" > /proc/sys/kernel/printk
2. cd /data/local/tmp
3. insmod stopwatch.ko
4. mknod /dev/stopwatch c 242 0
5. ./app
6. then press home button to start

#### Mannual
- Home button : Start stopwatch
- Back button : Pause stopwatch
- Vol+ button : Reset stopwatch
- Vol- button : If you press button for 3 seconds, you can terminate stopwatch 

