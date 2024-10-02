
.global NtLoadVm
NtLoadVm:
    push %ebp
    mov %esp, %ebp
    mov 8(%esp), %eax
    mov %eax, %cr3
    mov %ebp, %esp
    pop %ebp
    ret

.global NtEnableVm
NtEnableVm:
    push %ebp
    mov %esp, %ebp
    mov %cr0, %eax
    or $0x80000000, %eax
    mov %eax, %cr0
    mov %ebp, %esp
    pop %ebp
    ret

.global NtDisableVm
NtDisableVm:
    mov %cr0, %eax
    and $0x7fffffff, %eax
    mov %eax, %cr0

    jmp $0x08,$NtFlush

NtFlush:
    ret

.global NtFlushVm
NtFlushVm:
    mov %cr3, %eax
    mov %eax, %cr3
    ret

.global NtGetTable
NtGetTable:
    mov %cr3, %eax
    ret