bits 32

global displayStr
global clearStr
global readRTC

; Display string at row/col
displayStr:
	push ebx
	push edi
	mov edx, [esp+12]
	mov eax, [esp+16]
	imul edx, edx, 80
	add edx, eax
	shl edx, 1
	mov edi, edx
	mov ebx, [esp+20]
	mov ecx, [esp+24]
	mov ah, 0x14
nextChar:
	mov al, [ebx]
	mov [gs:edi], ax
	add edi, 2
	inc ebx
	loop nextChar
	pop edi
	pop ebx
	ret

; Clear string at row/col
clearStr:
	push edi
	mov edx, [esp+8]
	mov eax, [esp+12]
	imul edx, edx, 80
	add edx, eax
	shl edx, 1
	mov edi, edx
	mov ecx, [esp+16]
	mov ax, 0x1f20
clearLoop:
	mov [gs:edi], ax
	add edi, 2
	loop clearLoop
	pop edi
	ret

; Read from RTC register
readRTC:
	; TODO: 实现readRTC，函数定义见main.h

; 等待 RTC 不在更新中（寄存器 0x0A 的 bit7 变 0）
wait_uip:
    mov al, 0x8a
    out 0x70, al         ; 选寄存器 0x0A
    in  al, 0x71
    test al, 0x80
    jnz wait_uip

	;禁用NMI 
	mov al, [esp+4]
    or  al, 0x80
	out 0x70,al

	;读值
	in al,0x71
	mov bl,al

	;重新启用nmi
	mov  al, 0x00
    out  0x70, al

	mov al,bl
	ret
