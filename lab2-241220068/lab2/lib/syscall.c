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
	syscall(SYS_NOW, (unsigned int)hh, (unsigned int)mm, (unsigned int)ss, 0, 0);
}

void zeroTimeTicks() {
	syscall(SYS_ZERO_TIME_TICKS, 0, 0, 0, 0, 0);
}

int getTimeTicks() {
	return syscall(SYS_GET_TIME_TICKS, 0, 0, 0, 0, 0);
}

// TODO 2: 实现 sleep() 函数
void sleep(int second) {
	zeroTimeTicks();
	while (getTimeTicks() < second) {
		;
	}
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

static int hex2Str(unsigned int n, char *buf, int count, int w) {
	char s[16];
	int i = 0;
	//将无符号整数转换为十六进制字符串
	do {
		unsigned int d = n & 0xf;
		s[i++] = (d < 10) ? ('0' + d) : ('a' + d - 10);
		n >>= 4;
	} while (n != 0);
	//补齐宽度，用0填充不足部分
	while (i < w) {
		s[i++] = '0';
	}
	//将字符串倒序输出到buf
	while (i > 0) {
		buf[count++] = s[--i];
		if (count == MAX_BUFFER_SIZE) {
			syscall(SYS_WRITE, (unsigned int)buf, (unsigned int)MAX_BUFFER_SIZE, 0, 0, 0);
			count = 0;
		}
	}
	return count;
}

static int str2Buf(char *str, char *buf, int count) {
	if (str == 0) {
		str = "(null)";
	}
	while (*str != '\0') {
		buf[count++] = *str++;
		if (count == MAX_BUFFER_SIZE) {
			syscall(SYS_WRITE, (unsigned int)buf, (unsigned int)count, 0, 0, 0);
			count = 0;
		}
	}
	return count;
}

static int char2Buf(char ch, char *buf, int count) {
	buf[count++] = ch;
	if (count == MAX_BUFFER_SIZE) {
		syscall(SYS_WRITE, (unsigned int)buf, (unsigned int)count, 0, 0, 0);
		count = 0;
	}
	return count;
}

// TODO 3: 实现 printf() 函数
void printf(const char *format,...){
	char buf[MAX_BUFFER_SIZE];
	int count = 0;
	//在kernel/include/common/stdarg.h里定义了va_list和相关宏
	//使用这些宏来处理可变参数列表
	va_list ap;
	va_start(ap, format);

	for (const char *p = format; *p != '\0'; p++) {
		//处理普通字符
		if (*p != '%') {
			buf[count++] = *p;
			//缓存满了调用系统调用写入文件描述符0（即屏幕），并重置计数器
			if (count == MAX_BUFFER_SIZE) {
				//在kernel/core/irq_dispatch.c/syscallWrite包括了\t\n\r的处理，因此这里直接调用系统调用写屏即可
				syscall(SYS_WRITE, (unsigned int)buf, (unsigned int)count, 0, 0, 0);
				count = 0;
			}
			continue;
		}

		//处理特殊字符
		p++;
		//防止出现%\0的情况
		if (*p == '\0') {
			break;
		}

		//读取宽度
		int width = 0;
		while (*p >= '0' && *p <= '9') {
			width = width * 10 + (*p - '0');
			p++;
		}

		//处理整数
		if (*p == 'd') {
			int val = va_arg(ap, int);	//获取整数参数
			count = dec2Str(val, buf, count, width);//调用dec2Str将整数转换为字符串并存入buf，更新count
		} 
		//处理十六进制整数
		else if (*p == 'x') {
			unsigned int val = va_arg(ap, unsigned int);	//获取无符号整数参数
			count = hex2Str(val, buf, count, width);		//调用hex2Str将无符号整数转换为十六进制字符串并存入buf，更新count
		} 
		else if (*p == 's') {
			char *str = va_arg(ap, char *);	//获取字符串参数
			count = str2Buf(str, buf, count);	//调用str2Buf将字符串存入buf，更新count
		} else if (*p == 'c') {
			char ch = (char)va_arg(ap, int);	//获取字符参数
			count = char2Buf(ch, buf, count);	//调用char2Buf将字符存入buf，更新count
		} 
		//其他特殊情况
		//处理%%,表示输出一个%字符
		else if (*p == '%') {
			buf[count++] = '%';
			if (count == MAX_BUFFER_SIZE) {
				syscall(SYS_WRITE, (unsigned int)buf, (unsigned int)count, 0, 0, 0);
				count = 0;
			}
		} 
		//处理%+未知参数，这里我们选择输出%和未知参数字符
		else {
			buf[count++] = '%';
			if (count == MAX_BUFFER_SIZE) {
				syscall(SYS_WRITE, (unsigned int)buf, (unsigned int)count, 0, 0, 0);
				count = 0;
			}
			buf[count++] = *p;
			if (count == MAX_BUFFER_SIZE) {
				syscall(SYS_WRITE, (unsigned int)buf, (unsigned int)count, 0, 0, 0);
				count = 0;
			}
		}
	}

	va_end(ap);
	//最后将buf中剩余的内容写入屏幕
	if (count > 0) {
		syscall(SYS_WRITE, (unsigned int)buf, (unsigned int)count, 0, 0, 0);
	}
}