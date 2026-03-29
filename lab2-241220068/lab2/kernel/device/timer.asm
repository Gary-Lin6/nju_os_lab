[bits 32]
global initTimer

; TODO: 配置8253定时器，产生周期性中断
; - 向端口0x43写入控制字（模式3，波特率发生器）
; - 向端口0x40写入分频值
; - 计算：输入频率1193182Hz / 目标频率100Hz = 11932
; - 实现10ms产生一次时钟中断（用于sleep精度）
initTimer:
    mov dx, 0x43
    mov al, 0x36                ; 00：通道0|11：先低8位再高8位|011：模式3|0：二进制计数
    out dx, al                  ; 定时器配置

    mov dx, 0x40
    mov ax, 11932               ; 1193182 / 100Hz
    out dx, al                  ; 设置低8位
    mov al, ah
    out dx, al                  ; 设置高8位
    ret
