#include "common.h"
#include "x86.h"
#include "device.h"

static void GProtectFaultHandle(struct TrapFrame *tf);
static void syscallHandle(struct TrapFrame *tf);
static void syscallWrite(struct TrapFrame *tf);
static void syscallNow(struct TrapFrame *tf);
static void timerHandle(struct TrapFrame *tf);
static void syscallZeroTimeTicks(struct TrapFrame *tf);
static void syscallGetTimeTicks(struct TrapFrame *tf);

void irqHandle(struct TrapFrame *tf) {
	switch(tf->irq) {
		case -1:
			if (tf->err_code != 0) {
				GProtectFaultHandle(tf);
			}
			break;
		case 0xd:
			GProtectFaultHandle(tf);
			break;
		case 0x20:
			timerHandle(tf);
			break;
		case 0x80:
			syscallHandle(tf);
			break;
		default:
			kprintf("Unhandled IRQ %x, ERR %x\n", tf->irq, tf->err_code);
			assert(0);
	}
}

static void GProtectFaultHandle(struct TrapFrame *tf){
	kprintf("\n--- General Protection Fault Caught by Kernel ---\n");
	kprintf("Application tried to access restricted memory or performed invalid operation.\n");
	kprintf("Trap Frame Info:\n");
	kprintf("  IRQ: %x, Error Code: %x\n", tf->irq, tf->err_code);
	kprintf("  EIP: %x, CS: %x, CPL: %d\n", tf->eip, tf->cs, tf->cs & 3);
	kprintf("  DS: %x, ES: %x, SS: %x\n", tf->ds, tf->es, tf->ss_user);
	
	if ((tf->cs & 3) == 3) {
		kprintf("Confirmed: Exception occurred in USER MODE (CPL 3).\n");
		kprintf("Segmentation protection is ACTIVE.\n");
	}

	kprintf("Halting system.\n");
	while(1);
}

// TODO 1: 实现 syscallHandle 系统调用分发函数
static void syscallHandle(struct TrapFrame *tf) {
	//lib/doSyscall.asm 中已经将系统调用号放在 tf->eax 中
	switch (tf->eax) {
		//各case情况对应lib/syscall.c的定义
		case 0:
			syscallWrite(tf);
			break;
		case 2:
			syscallNow(tf);
			break;
		case 3:
			syscallZeroTimeTicks(tf);
			break;
		case 4:
			syscallGetTimeTicks(tf);
			break;
		default:
			kprintf("Unhandled syscall: %d\n", tf->eax);
			break;
	}
}

// TODO 2: 实现 syscallWrite 写屏功能
static void syscallWrite(struct TrapFrame *tf) {
	//取参数，由lib/syscall.c/dec2Str可得参数1为字符串地址，参数2为字符串长度
	int sel = tf->ds;	//取用户段选择子
	int base = getSegBase(sel);		//根据选择子获取段基址
	char *str = (char *)(base + tf->ecx);	//根据段基址和偏移获取字符串地址
	int size = tf->edx;		//写屏字符串长度

	//VGA 文本模式显存基址
	volatile uint16_t *video = (volatile uint16_t *)0xb8000;

	for (int i = 0; i < size; i++) {
		char ch = str[i];
		//换行 \n 分支新起一行。
		if (ch == '\n') {
			displayRow++;
			displayCol = 0;
		} 
		//回车 \r 分支将列重置为0，但不换行。
		else if (ch == '\r') {
			displayCol = 0;
		} 
		//制表符 \t 分支移动到下一个4列制表位。
		else if (ch == '\t') {
			int nextCol = (displayCol + 4) & ~3;
			while (displayCol < nextCol) {
				video[displayRow * 80 + displayCol] = (uint16_t)' ' | (0x0c << 8);
				displayCol++;
				if (displayCol >= 80) {
					displayCol = 0;
					displayRow++;
					break;
				}
			}
		}
		//普通字符写到当前位置
		else {
			video[displayRow * 80 + displayCol] = (uint16_t)ch | (0x0c << 8); //高八位：属性字节，0x0c 表示红色前景。低8位：字符
			displayCol++;//列加1
			//列超过80则换行
			if (displayCol >= 80) {
				displayCol = 0;
				displayRow++;
			}
		}

		//滚屏：行数超过25则滚屏，行数重置为24
		if (displayRow >= 25) {
			scrollScreen();
			displayRow = 24;
		}
	}

	//更新光标位置
	updateCursor(displayRow, displayCol);
}

// TODO 3: 实现 syscallNow 获取实时时钟
static void syscallNow(struct TrapFrame *tf){
	//取参数 
	int base = getSegBase(tf->ds);
	int *hh = (int *)(base + tf->ecx);
	int *mm = (int *)(base + tf->edx);
	int *ss = (int *)(base + tf->ebx);

	//同lab1，读取RTC寄存器获取当前时间
	unsigned char sec = readRTC(0x00);
	unsigned char min = readRTC(0x02);
	unsigned char hour = readRTC(0x04);

	//若 RTC 处于 BCD 模式则转换为十进制编码，//若不处于 BCD 模式（二进制模式）则直接赋值给参数
	unsigned char regB = readRTC(0x0B);
	if ((regB & 0x04) == 0)//处于BCD 模式
	{
		sec = (unsigned char)(((sec >> 4) * 10) + (sec & 0x0f));
		min = (unsigned char)(((min >> 4) * 10) + (min & 0x0f));
		hour = (unsigned char)(((hour >> 4) * 10) + (hour & 0x0f));
	}
	*hh = (int)hour;
	*mm = (int)min;
	*ss = (int)sec;
}

static int ticks = 0;
static int tickCount = 0;

// TODO 4: 实现 timerHandle 时钟中断处理
static void timerHandle(struct TrapFrame *tf) {
	(void)tf;	//避免未使用参数告警。
	//在device/timer.asm里设置了每10ms发送一次时钟中断,每次中断经irqHandle调用到timerHandle
	//要想计算1s，即调用了100次timerHandle.
	//因此在timerHandle里维护一个tickCount，每次调用加1，当tickCount达到100时，ticks加1，tickCount重置为0。
	tickCount++;
	if (tickCount >= 100) {
		ticks++;
		tickCount = 0;
	}
}

static void syscallZeroTimeTicks(struct TrapFrame *tf) {
	// kprintf("Syscall: ZeroTimeTicks\n");
    ticks = 0;
}

static void syscallGetTimeTicks(struct TrapFrame *tf) {
    tf->eax = ticks;
}
