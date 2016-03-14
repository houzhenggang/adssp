sudo ifconfig p1p1 down
sudo ifconfig p1p2 down
sudo modprobe uio
sudo insmod ./build/kmod/igb_uio.ko
sudo ./tools/dpdk_nic_bind.py --bind=igb_uio p1p1 p1p2
