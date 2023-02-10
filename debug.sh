qemu-system-i386 \
    -no-reboot \
    -d cpu_reset \
    -s -S \
    -hda hdd.img \
    --kernel ./build_i486_debug/kernel.bin
