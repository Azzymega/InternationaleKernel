.global NtFlushGdt
NtFlushGdt:
	mov $0x10,%ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss
	jmp $0x08,$FlushEnd
FlushEnd:
    ret