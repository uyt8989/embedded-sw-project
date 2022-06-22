#include "com_example_androidex_MainActivity.h"
#include "android/log.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>

//#define LOG_TAG "MyTag"
//#define LOGV(...)   __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define IOCTL_MOVE _IOW(242, 1, int)
#define IOCTL_CHECK _IO(242, 2)

JNIEXPORT jint JNICALL Java_com_example_androidex_MainActivity_driverOpen (JNIEnv *env, jobject thisObject) {
	int fd = open("/dev/maze", O_WRONLY);
	return fd;
}

/*
 * Class:     com_example_androidex_MainActivity
 * Method:    driverClose
 * Signature: (I)V
 */
JNIEXPORT jint JNICALL Java_com_example_androidex_MainActivity_driverClose
(JNIEnv * env, jobject thisObject, jint fd) {
	return (jint)close(fd);

}

JNIEXPORT void JNICALL Java_com_example_androidex_MainActivity_moveUp
(JNIEnv * env, jobject thisObject, jint fd) {
	//LOGV("up");
	int dir = 0, result;
	result = ioctl(fd, IOCTL_MOVE, &dir);
}

/*
 * Class:     com_example_androidex_MainActivity
 * Method:    moveDown
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_example_androidex_MainActivity_moveDown
(JNIEnv * env, jobject thisObject, jint fd) {
	//LOGV("down");
	int dir = 2;
	ioctl(fd, IOCTL_MOVE, &dir);
}

/*
 * Class:     com_example_androidex_MainActivity
 * Method:    moveLeft
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_example_androidex_MainActivity_moveLeft
(JNIEnv * env, jobject thisObject, jint fd) {
	int dir = 3;
	ioctl(fd, IOCTL_MOVE, &dir);
}
/*
 * Class:     com_example_androidex_MainActivity
 * Method:    moveRight
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_example_androidex_MainActivity_moveRight
(JNIEnv * env, jobject thisObject, jint fd) {
	int dir = 1;
	ioctl(fd, IOCTL_MOVE, &dir);
}

/*
 * Class:     com_example_androidex_MainActivity
 * Method:    checkEnding
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_example_androidex_MainActivity_checkEnding
  (JNIEnv *env, jobject thisObject, jint fd) {
	ioctl(fd, IOCTL_CHECK);
}
