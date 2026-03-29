#include "myos.h"

int main(void) {

	
	int hh, mm, ss;
	printf("<Real Time Clock>\n");
	for(int i=0; i<10; i++) {
		now(&hh, &mm, &ss);
		printf("\rCurrent Time: %2d:%2d:%2d", hh, mm, ss);
		sleep(1);
	}
	//TODO1：在代码里添加地址访问越界的操作，使用make bochs运行项目，观察内核捕获异常的情况
	// 访问超出用户段限长的地址，触发段保护异常
	*(volatile int *)0x20000 = 0x12345678;

	//TODO2: 在代码里添加特权指令，使用make run运行项目，观察内核捕获异常的情况。
	// 执行特权指令 cli，用户态下会触发保护异常
	asm volatile("cli");

        //注意：两个todo可以都一起完成，跟在现有代码后面，先用make bochs测试TODO1，再用make run测试TODO2（make run用的是qemu，TODO1不影响测试结果）
	return 0;
}
