output_dir=build_i486_release
output_image=${output_dir}/hdd.img
grub_prefix=$HOME/cross
disk_img_size_mb=20
grub_modules="biosdisk part_msdos multiboot sh"

#grub-mkimage --prefix=/boot --compression=none --format=i386-pc --output=${output_dir}/grub.bin multiboot fat

dd if=/dev/zero of=${output_image} count=${disk_img_size_mb} bs=1048576

parted --script ${output_image} mklabel msdos mkpart p ext2 1 20 set 1 boot on

looppart=`kpartx -l ${output_image} | awk -e '{ print $1; exit }'`
loopdev=`kpartx -l ${output_image} | awk -e '{ print $5; exit }'`
kpartx -a ${output_image}

sleep 3

mkfs.ext2 /dev/mapper/$looppart
mkdir -p ${output_dir}/tmp/p1
mount /dev/mapper/$looppart ${output_dir}/tmp/p1

cp ${output_dir}/kernel.bin build/tmp/p1

echo "(hd0) $loopdev" > ${output_dir}/tmp/device.map

grub2-install --no-floppy                                                      \
              --grub-mkdevicemap=${output_dir}/tmp/device.map                  \
              --modules="biosdisk part_msdos ext2 configfile normal multiboot" \
              --root-directory=${output_dir}/tmp/p1                            \
              --boot-directory=${output_dir}/tmp/p1/boot                       \
              $loopdev

umount ${output_dir}/tmp/p1

kpartx -d ${output_image}


# Replace the bootsector with the one from grub
#dd if=${grub_prefix}/lib/grub/i386-pc/boot.img of=${output_dir}/${output_image} conv=notrunc
#
## Add the generated grub image
#dd if=${output_dir}/grub.bin of=${output_dir}/${output_image} conv=notrunc seek=1
#
#grub_bin_size=$(stat --printf="%s" ${output_dir}/grub.bin)
#size=$(ls --block-size=512 -s "${output_dir}/grub.bin" | sed 's/\s.*$//')
#
#echo "mformat..."
#mformat -i "${output_dir}/${output_image}" -kR $(($size + 2))
#
#echo "mkdir..."
#mmd -i "${output_dir}/${output_image}" grub


