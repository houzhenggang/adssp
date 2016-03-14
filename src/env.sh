export RTE_SDK=`pwd`/dpdk-2.0.0
export RTE_TARGET=x86_64-native-linuxapp-gcc
export EXTRA_CFLAGS="-O0 -g"
make -C naga/pcre-m/
