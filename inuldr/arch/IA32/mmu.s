.global NTFlushGDT
NTFlushGDT:
#	mov 4(%esp), %eax
#	lgdt (%eax)
	mov $0x10,%ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss
    pop %eax
    push $0x08
    push %eax
    lret

.global NTLoadMMU
NTLoadMMU:
    push %ebp
    mov %esp, %ebp
    mov 8(%esp), %eax
    mov %eax, %cr3
    mov %ebp, %esp
    pop %ebp
    ret

.global NTEnableMMU
NTEnableMMU:
    push %ebp
    mov %esp, %ebp
    mov %cr0, %eax
    or $0x80000000, %eax
    mov %eax, %cr0
    mov %ebp, %esp
    pop %ebp
    ret

.global NTDisableMMU
NTDisableMMU:
    mov %cr0, %eax
    and $0x7fffffff, %eax
    mov %eax, %cr0
    pop %eax
    push $0x08
    push %eax
    lret

.global NTEnablePSE
NTEnablePSE:
    mov %cr4, %ecx
    btsl $4, %ecx
    btsl $7, %ecx
    mov %ecx, %cr4
    ret