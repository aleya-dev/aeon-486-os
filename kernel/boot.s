###############################################################################
# Constants
###############################################################################
.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map

.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

.set KERNEL_START_OFFSET, 0xC0000000

# Page table flags
.set PTF_PRESENT,             1<<0
.set PTF_READ_WRITE,          1<<1
.set PTF_CACHE_WRITE_THROUGH, 1<<3

# Page table OS specific flags
.set PTF_AEON_LOCKED,         1<<11 # The page should never be unpaged. It belongs to the kernel

# Page directory flags
.set PDF_PRESENT,             1<<0
.set PDF_READ_WRITE,          1<<1

###############################################################################

###############################################################################
# Multiboot header
###############################################################################
.section .rodata.multiboot, "a"
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

###############################################################################

# The kernel entry point.
.section .text.multiboot, "a"
.global _start
.type _start, @function
_start:
###############################################################################
# Setup 00000000 page table (identity map of the first 1mb)
###############################################################################

    movl $(g_page_table_00000000 - KERNEL_START_OFFSET), %edi # Page table address
    movl $0, %esi # Page table value

    movl %esi, %edx                                             # Copy the current address

    # Write the flags. Enable cache write-through since this table is mostly used for
    # memory mapped hardware (vga, etc.)
    orl $(PTF_PRESENT | PTF_READ_WRITE | PTF_CACHE_WRITE_THROUGH | PTF_AEON_LOCKED), %edx
    movl %edx, (%edi)                                           # Write address+flags in the page table

1:
    # if we're at the end of the page table, stop; otherwise write the next entry
    cmpl $(g_page_table_00000000 - KERNEL_START_OFFSET + 4096), %edi
    jge 2f

    movl %esi, %edx
    orl $0x003, %edx
    movl %edx, (%edi)
    addl $4096, %esi
    addl $4, %edi
    loop 1b

2:

###############################################################################
# Setup C0000000 page table (kernel map)
###############################################################################

    movl $(g_page_table_C0000000 - KERNEL_START_OFFSET), %edi
    movl $0, %esi

1:
    # Only map the kernel.
    cmpl $_kernel_start, %esi
    jl 2f
    cmpl $(_kernel_end - KERNEL_START_OFFSET), %esi
    jge 3f

    # Map physical address as "present, writable". Note that this maps
    # .text and .rodata as writable. Mind security and map them as non-writable.
    movl %esi, %edx
    orl $(PTF_PRESENT | PTF_READ_WRITE | PTF_AEON_LOCKED), %edx
    movl %edx, (%edi)

2:
    # Size of page is 4096 bytes.
    addl $4096, %esi
    # Size of entries in boot_page_table1 is 4 bytes.
    addl $4, %edi
    # Loop to the next entry if we haven't finished.
    loop 1b

3:
    movl $(0x000B8000 | 0x003), g_page_table_C0000000 - KERNEL_START_OFFSET + 1023 * 4

    # Temporarily identity map the kernel to 00000000 until we can actually long jump into the upper half
    movl $(g_page_table_C0000000 - KERNEL_START_OFFSET + PDF_PRESENT + PDF_READ_WRITE), g_page_directory - KERNEL_START_OFFSET
    movl $(g_page_table_C0000000 - KERNEL_START_OFFSET + PDF_PRESENT + PDF_READ_WRITE), g_page_directory - KERNEL_START_OFFSET + 768 * 4
    movl $(g_page_table_C0400000 - KERNEL_START_OFFSET + PDF_PRESENT + PDF_READ_WRITE), g_page_directory - KERNEL_START_OFFSET + 769 * 4
    movl $(g_page_table_C0800000 - KERNEL_START_OFFSET + PDF_PRESENT + PDF_READ_WRITE), g_page_directory - KERNEL_START_OFFSET + 770 * 4

    # Set cr3 to the address of the boot_page_directory.
    movl $(g_page_directory - KERNEL_START_OFFSET), %ecx
    movl %ecx, %cr3

    # Enable paging and the write-protect bit.
    movl %cr0, %ecx
    orl $0x80010000, %ecx
    movl %ecx, %cr0

    # Jump to higher half with an absolute jump.
    lea 4f, %ecx
    jmp *%ecx

.section .text

4:
    # The longjump happened; we can now safely use the 1mb identity map instead since the kernel only needs
    # the C0000000 map from now on.
    movl $(g_page_table_00000000 - KERNEL_START_OFFSET + PDF_PRESENT + PDF_READ_WRITE), g_page_directory + 0

    # Reload crc3 to force a TLB flush so the changes to take effect.
    movl %cr3, %ecx
    movl %ecx, %cr3

    # Set up the stack.
    mov $stack_top, %esp

    # Enter the high-level kernel.
    pushl %ebx # Info struct ptr
    pushl %eax # Multiboot magic number
    call kernel_main

    # Infinite loop if the system has nothing more to do.
    cli
1:      hlt
    jmp 1b

###############################################################################
# Page tables
###############################################################################
.section .bss, "aw", @nobits
    .align 4096
.global g_page_directory
g_page_directory:
    .skip 4 * 1024
.global g_page_table_00000000
g_page_table_00000000:
    .skip 4 * 1024
.global g_page_table_C0000000
g_page_table_C0000000:
    .skip 4 * 1024
.global g_page_table_C0400000
g_page_table_C0400000:
    .skip 4 * 1024
.global g_page_table_C0800000
g_page_table_C0800000:
    .skip 4 * 1024

###############################################################################

###############################################################################
# Kernel stack
###############################################################################
.section .kernel_stack, "aw", @nobits
stack_bottom:
.skip 8 * 1024 # 8 KiB
stack_top:
###############################################################################
