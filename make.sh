output_dir=build
output_image=floppy.img
#grub_prefix=$HOME/cross
#grub_modules="biosdisk part_msdos multiboot sh"

# Build bootloader
nasm boot/boot.asm -f bin -o ${output_dir}/boot.bin

#grub-mkimage --prefix=/boot --compression=none --format=i386-pc --output=${output_dir}/grub.bin multiboot fat
#size=$(ls --block-size=512 -s "${output_dir}/grub.bin" | sed 's/\s.*$//')

# Generate the floppy image
dd if=/dev/zero of=${output_dir}/floppy.img bs=512 count=2880

# Replace the bootsector with the one from grub
#dd if=${grub_prefix}/lib/grub/i386-pc/boot.img of=${output_dir}/floppy.img conv=notrunc
#dd if=${output_dir}/boot.bin of=${output_dir}/floppy.img conv=notrunc


#grub_bin_size=$(stat --printf="%s" ${output_dir}/grub.bin)
#size=$(ls --block-size=512 -s "${output_dir}/grub.bin" | sed 's/\s.*$//')

#echo "second img ${size}"

# Add the generated grub image
#dd if=${output_dir}/grub.bin of=${output_dir}/floppy.img conv=notrunc seek=1

#echo "Mformat blocks $(($size + 2))"
#mformat -i "${output_dir}/floppy.img" -kR 3

#dd if=${output_dir}/boot.bin of=${output_dir}/floppy.img conv=notrunc

mformat -i "${output_dir}/floppy.img" -B ${output_dir}/boot.bin -kR 3

#echo "mmd"
#mmd -i "${output_dir}/floppy.img" grub

#mkfs.msdos -F 12 ${output_dir}/floppy.img


#grub-install --verbose --allow-floppy --compress=no --target=i386-pc ${output_dir}/floppy.img

# Copy grub
#grub_bin_size=$(stat --printf="%s" ${output_dir}/grub.bin)

#dd conv=notrunc if=${output_dir}/grub.bin of=${output_dir}/floppy.img bs=${grub_bin_size} count=1

# Copy test.bin right after that
#dd conv=notrunc if=${output_dir}/test.bin of=${output_dir}/floppy.img seek=512 bs=1 count=23

#dd if=boot.bin of=/dev/sda
