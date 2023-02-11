qemu-system-i386 \
    -no-reboot \
    -d cpu_reset \
    -m size=8,maxmem=0x800000 \
    -s \
    -hda hdd.img
