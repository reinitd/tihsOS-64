section .multiboot_header
header_start:
    ;; magic number that multiboot 2 will look for
    dd 0xe85250d6
    ;; Architecture (Protected mode - i386)
    dd 0
    ;; Length of header
    dd header_end - header_start
    ;; Checksum
    dd 0x100000000  - (0xe85250d6 + 0 + (header_end - header_start))

    ;; End tag
    dw 0
    dw 0
    dd 0

header_end: