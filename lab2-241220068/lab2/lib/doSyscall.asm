[bits 32]

global syscall

; TODO: 实现 syscall 汇编函数
; - 参数：EAX=系统调用号，ECX/EDX/EBX/ESI/EDI=参数1-5
; - 使用栈帧传递参数
; - 保存寄存器
; - 触发软中断
; - 返回值在 EAX 中
; - 恢复寄存器并返回
syscall:
    ; TODO: 在此实现系统调用触发代码
