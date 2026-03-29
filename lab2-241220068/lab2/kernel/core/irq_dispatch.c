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
	// TODO: 在此实现系统调用分发代码
}

// TODO 2: 实现 syscallWrite 写屏功能
static void syscallWrite(struct TrapFrame *tf) {
	// TODO: 在此实现写屏功能
}

// TODO 3: 实现 syscallNow 获取实时时钟
static void syscallNow(struct TrapFrame *tf){
	// TODO: 在此实现获取RTC时间代码
}

static int ticks = 0;
static int tickCount = 0;

// TODO 4: 实现 timerHandle 时钟中断处理
static void timerHandle(struct TrapFrame *tf) {
	// TODO: 在此实现时钟中断处理代码
}

static void syscallZeroTimeTicks(struct TrapFrame *tf) {
	// kprintf("Syscall: ZeroTimeTicks\n");
    ticks = 0;
}

static void syscallGetTimeTicks(struct TrapFrame *tf) {
    tf->eax = ticks;
}
