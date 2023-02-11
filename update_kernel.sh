script=$(realpath "$0")
script_path=$(dirname "$script")
mount_point=/mnt/hdd
loop_device=/dev/loop0
config=$1

if [ -z "$config" ]; then
    config="debug"
fi

if [ "$config" != "debug" ] && [ "$config" != "release" ]; then
    echo "Configuration must be debug or release."
    exit 1
fi

echo Using config $config

sudo losetup -P ${loop_device} ${script_path}/hdd.img
sudo mount ${loop_device}p1 ${mount_point}
sudo cp ${script_path}/build_i486_${config}/kernel.bin ${mount_point}/boot/kernel.bin
sudo sync
sudo umount ${mount_point}
sudo losetup -d ${loop_device}
