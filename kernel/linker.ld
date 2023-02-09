ENTRY (_start)

kernel_physical_offset_address = 0x00100000;
kernel_paged_offset_address = 0xC0000000;

SECTIONS
{
    . = kernel_physical_offset_address;

    _kernel_start = .;

    .data.multiboot :
    {
        *(.data.multiboot)
    }

    .text.multiboot :
    {
        *(.text.multiboot)
    }

    . += kernel_paged_offset_address;

    .text ALIGN (4K) : AT (ADDR (.text) - kernel_paged_offset_address)
    {
        *(.text)
    }

    .rodata ALIGN (4K) : AT (ADDR (.rodata) - kernel_paged_offset_address)
    {
        *(.rodata)
    }

    .data ALIGN (4K) : AT (ADDR (.data) - kernel_paged_offset_address)
    {
        *(.data)
    }

    .bss ALIGN (4K) : AT (ADDR (.bss) - kernel_paged_offset_address)
    {
        *(COMMON)
        *(.bss)
        *(.kernel_stack)
    }

    _kernel_end = .;
}
