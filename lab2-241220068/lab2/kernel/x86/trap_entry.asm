[bits 32]

global irqEmpty
global irqErrorCode
global irqDoubleFault
global irqInvalidTSS
global irqSegNotPresent
global irqStackSegFault
global irqGProtectFault
global irqPageFault
global irqAlignCheck
global irqSecException
global irqSyscall
global irqTimer
global asmDoIrq

extern irqHandle

irqEmpty:
    push 0
    push -1
    jmp asmDoIrq

irqTimer:
    push 0
    push 0x20
    jmp asmDoIrq

irqErrorCode:
    push -1
    jmp asmDoIrq

irqDoubleFault:
    push -1
    jmp asmDoIrq

irqInvalidTSS:
    push -1
    jmp asmDoIrq

irqSegNotPresent:
    push -1
    jmp asmDoIrq

irqStackSegFault:
    push -1
    jmp asmDoIrq

irqGProtectFault:
    push 0xd
    jmp asmDoIrq

irqPageFault:
    push -1
    jmp asmDoIrq

irqAlignCheck:
    push -1
    jmp asmDoIrq

irqSecException:
    push -1
    jmp asmDoIrq

; TODO 1: 实现 irqSyscall 系统调用入口
irqSyscall:
    push 0          ;errorcode
    push 0x80       ;irq
    jmp asmDoIrq

; TODO 2: 实现 asmDoIrq 公共中断处理入口
asmDoIrq:
;保存现场
    ;保存段寄存器
    push ds
    push es
    push fs
    push gs
    ;保存通用寄存器
    pushad

;切换到内核数据栈
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

;调用irqHandle
    push esp
    call irqHandle
    add esp, 4

;恢复现场
    popad
    pop gs
    pop fs
    pop es
    pop ds

;清理中断号与错误码并返回
    add esp, 8
    iret
    