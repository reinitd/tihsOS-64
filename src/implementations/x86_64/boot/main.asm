global start
extern long_mode_start

;; STACK NOTES
;;
;; The stack is a region of memory that stores 
;; function call data, variables, memory
;; address that it should return to upon
;; completion, etc.
;;
;; Each segment is referred to as a "stack frame"
;;
;;
;;
;; OTHER NOTES
;;
;; In order to enable 64 bit, the CPU must go
;; into "long mode".
;;
;; For that we need to make virtual memory.
;;
;;
;; PAGING AND VIRTUAL MEMORY NOTES
;;
;;
;; A single page is 4kb.
;; Each table can hold 512 enteries.
;; There is so much about paging, Google it to learn more.
;;
;; Benefits of paging and virtual addresses:
;;      - Reduced memory fragmentation, we can shuffle memory around with affecting a program's memory space.
;;      - We can map virtual addresses to data that is not currently present in RAM, allowing for hard drive memory swapping.
;;      - Copy-on-Write data
;;      - etc.
;;

section .text
bits 32
start:
    mov esp, stack_top  ;; `esp` is used to determine the memory
                        ;; address of the current stack frame.
                        ;; A/K/A, the stack pointer.

    ;; Checklist to see if 64bit is available.

    call check_multiboot
    call check_cpuid
    call check_long_mode

    call setup_page_tables
    call enable_paging

    lgdt [gdt64.pointer] ;; Load the GDT by providing a dereferenced (brackets) pointer.
    jmp gdt64.code_segment:long_mode_start ;; Load the code segment into the code selector, or something. IDK what this really does.

    hlt ;; halt the cpu

check_multiboot:
    cmp eax, 0x36d76289 ;; Most popular bootloaders store this value in the `eax` register.
    jne .no_multiboot   ;; If the ceratin value isn't in the register, it goes to `no_multiboot`.
                        ;; `no_multiboot` returns the error "B". Meaning that multiboot wasn't used.
    ret
.no_multiboot:
    mov al, "B"
    jmp error

check_cpuid:
                        ;; Attempts to flip the ID bit of the flags register.
                        ;; If it's flipped, it means cpuid is available.
                        ;;
    pushfd              ;; Push the flags register to the top of the stack.
    pop eax             ;; Pop the stack into the `eax` register.
    mov ecx, eax        ;; Makes a copy in the `ecx` register, to compare if
                        ;; the bit was actually flipped.
                        ;;
    xor eax, 1 << 21    ;; Flip the ID bit (bit 21) in the `eax` register,
                        ;; `eax` currently has the stack in it.
                        ;;
    push eax            ;; Copy it back to the flag register by pushing it
                        ;; back to the stack.
                        ;;
    popfd               ;; Popping it in to the flags register. (I have no idea what is happening)
    pushfd              ;;
    pop eax             ;; `pushfd` and `pop eax` copies the stack with the
                        ;; supposed flipped ID bit into the stack.
                        ;;
    push ecx            ;; Pushing and popfd makes the flag register back to
    popfd               ;; normal, before any of this code was ran.
                        ;;
    cmp eax, ecx        ;; Compare `eax` and `ecx` to see if the ID bit was
                        ;; flipped.
                        ;;
    je .no_cpuid        ;; If they are equal to each other, that means the
                        ;; ID bit wasn't flipped. This jumps to `.no_cpuid`
                        ;; which throws an error "C", meaning no cpuid.
    ret
.no_cpuid:
    mov al, "C"
    jmp error

check_long_mode:
                        ;; First check if Extended Processor Info is supported.
    mov eax, 0x80000000 ;; 
    cpuid               ;; `cpuid` takes in the `eax` register as an argument.
    cmp eax, 0x80000001 ;;
    jb .no_long_mode    ;; If `eax` is less than `0x80000001`, that means that
                        ;; EPI is not supported. Meaning no long mode.
                        ;;
                        ;;
                        ;;
                        ;; Now we check if long mode is supported by using EPI.
                        ;;
    mov eax, 0x80000001 ;;
    cpuid               ;; `cpuid` will store a value into the `edx` register.
    test edx, 1 << 29   ;; If the LM (Long Mode) bit (bit 29) is set, that
                        ;; means long mode is supported.
                        ;;
    jz .no_long_mode    ;; If the LM bit is not set, it jumps to `.no_long_mode`,
                        ;; returning error "L", meaning long mode is not supported. 
    ret
.no_long_mode:
    mov al, "L"
    jmp error

setup_page_tables:                      ;; This does indentity mapping, with maps a physical address
                                        ;; to the exact same virtual address.
                                        ;; I have no idea what is happening here.
    mov eax, page_table_l3              ;; 
    or eax, 0b11                        ;; present and writable flags
    mov [page_table_l4], eax            ;;
                                        ;;
    mov eax, page_table_l2              ;;
    or eax, 0b11                        ;; present and writable flags
    mov [page_table_l3], eax            ;;
                                        ;;
    mov ecx, 0                          ;; counter
.loop:                                  ;;
                                        ;;
    mov eax, 0x200000                   ;; For each iteration it maps a 2mb page.
    mul ecx                             ;;
    or eax, 0b10000011                  ;; present and writable huge page, allows us to point directly
                                        ;; to physical memory and allocate a huge page that is 2mb.
                                        ;; This also means we dont have to worry about the L1 table.
                                        ;;
    mov [page_table_l2 + ecx * 8], eax  ;; I have no idea what is happening...
                                        ;;
    inc ecx                             ;; increment the counter
    cmp ecx, 512                        ;; check if whole table is mapped
    jne .loop                           ;; if not, continue

    ret

enable_paging:
    ;; pass the page table location to CPU
    mov eax, page_table_l4
    mov cr3, eax        ;; The CPU looks for the page table
                        ;; location in the `cr3` register.

    ;; Enable PAE (Physical Address Extension)
    ;; This is necessary for x64 paging.
    mov eax, cr4        ;; Copy the value of `cr4` to `eax`.
    or eax, 1 << 5      ;; Enable the PAE flag in the `cr4`
                        ;; register (via `eax`) by flipping
                        ;; the PAE bit (bit 5).
                        ;;
    mov cr4, eax        ;; Move the `eax`, the modified
                        ;; `cr4` register back to `cr4`.
                        ;; i.e. Saving the changes.

    ;; Enable long mode
    ;; by working with "Model Specific Registers" (MSR)
    mov ecx, 0xC0000080 ;; The magic value that does magic.
    rdmsr               ;; Read the MSR, this loads the EFER
                        ;; value into the `eax` register.
                        ;;
    or eax, 1 << 8      ;; Enable the long mode flag by flipping
                        ;; the long mode bit (bit 8).
                        ;;
    wrmsr               ;; Write the MSR.

    ;; Enable paging
    mov eax, cr0        ;; Copy `cr0` into `eax` so it can be edited.
    or eax, 1 << 31     ;; Enable the paging flag by flipping the 
                        ;; paging bit (bit 31).
                        ;;
    mov cr0, eax        ;; Save the changes by moving `eax` back
                        ;; into `cr0`.
    ret

error:
    ;; We print these to video memory, which starts at
    ;; `0xb8000`. The brackets around dereference it.
    mov dword [0xb8000], 0x4f524f45
    mov dword [0xb8004], 0x4f3a4f52
    mov dword [0xb8008], 0x4f204f20
    mov byte  [0xb800a], al ;; This is the error code, which is located at the `al` register.
    hlt

section .bss      ;; statically allocated variables - will be reserved when the boot loader loads the kernel.
                  ;;
                  ;; Reserving memory for the page tables
align 4096        ;; Align all the tables to 4kb.
page_table_l4:    ;; 
    resb 4096     ;; 4kb
page_table_l3:
    resb 4096     ;; 4kb
page_table_l2:
    resb 4096     ;; 4kb
stack_bottom:
    resb 4096 * 4 ;; reserve 16kb of memory, the stack space
stack_top:

section .rodata   ;; read-only data section.
gdt64:            ;; GDT = Global Descriptor Table, this switches
                  ;; us from a x32-compatible mode to actual x64.
                  ;;
    dq 0          ;; zero entry
.code_segment: equ $ - gdt64 ;; `equ $ - gdt64` is the offset.
    dq (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53)
    ;; Enable      Set         Enable      Enable
    ;; Executable  Descriptor  Present     64 bit
    ;; Flag        Type        Flag        Flag      WooHoo, all done!
    ;;             (Code n Data)
.pointer:         ;; pointer to the global descriptor table
    dw $ - gdt64 - 1
    dq gdt64
