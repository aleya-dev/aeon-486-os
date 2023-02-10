sudo losetup -P /dev/loop0 /mnt/d/Development/aeon_486_os/hdd.img
sudo mount /dev/loop0p1 /mnt/hdd
sudo cp /mnt/d/Development/aeon_486_os/build_i486_debug/kernel.bin /mnt/hdd/boot/kernel.bin
sudo sync
sudo umount /mnt/hdd
sudo losetup -d /dev/loop0
