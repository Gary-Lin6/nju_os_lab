#include "../kernel/include/common/stdarg.h"

// System Call Numbers
#define SYS_WRITE 0
#define SYS_NOW 2
#define SYS_ZERO_TIME_TICKS 3
#define SYS_GET_TIME_TICKS 4
#define MAX_BUFFER_SIZE 256

int syscall(int num, unsigned int a1, unsigned int a2, unsigned int a3, unsigned int a4, unsigned int a5);

// TODO 1: 实现 now() 函数
// - 参数为小时/分钟/秒的指针
void now(int *hh, int *mm, int *ss){
	// TODO: 在此实现 now 函数
}

void zeroTimeTicks() {
	syscall(SYS_ZERO_TIME_TICKS, 0, 0, 0, 0, 0);
}

int getTimeTicks() {
	return syscall(SYS_GET_TIME_TICKS, 0, 0, 0, 0, 0);
}

// TODO 2: 实现 sleep() 函数
void sleep(int second) {
	// TODO: 在此实现 sleep 函数
}

static int dec2Str(int n, char *buf, int count, int w) {
	char s[16];
	int i = 0, neg = n < 0;
	if (neg) n = -n;
	do { s[i++] = (n % 10) + '0'; n /= 10; } while (n > 0);
	if (neg) s[i++] = '-';
	while (i < w) s[i++] = '0';
	while (i > 0) {
		buf[count++] = s[--i];
		if (count == MAX_BUFFER_SIZE) {
			syscall(SYS_WRITE, (unsigned int)buf, (unsigned int)MAX_BUFFER_SIZE, 0, 0, 0);
			count = 0;
		}
	}
	return count;
}

// TODO 3: 实现 printf() 函数
void printf(const char *format,...){
	// TODO: 在此实现 printf 函数
}