output_dir=build
output_image=floppy.img
#grub_prefix=$HOME/cross
#grub_modules="biosdisk part_msdos multiboot sh"

# Build bootloader
nasm boot/boot.asm -f bin -o ${output_dir}/boot.bin

# Build kernel
nasm kernel/entry.asm -f elf -o ${output_dir}/entry.o
i486-elf-gcc -o2 -ffreestanding -c kernel/main.c -o ${output_dir}/main.o
i486-elf-ld -o ${output_dir}/kernel.bin -Ttext 0x1000 ${output_dir}/entry.o ${output_dir}/main.o --oformat binary

# Generate the floppy image
dd if=/dev/zero of=${output_dir}/floppy.img bs=512 count=2880

# Write bootsector
dd if=${output_dir}/boot.bin of=${output_dir}/floppy.img conv=notrunc

# Check kernel size
kernel_size=$(stat --printf="%s" ${output_dir}/kernel.bin)
echo "Kernel size: ${kernel_size} bytes"

# Copy kernel right after that
dd conv=notrunc if=${output_dir}/kernel.bin of=${output_dir}/floppy.img seek=512 bs=1 count=${kernel_size}
