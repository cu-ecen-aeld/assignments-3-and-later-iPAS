#!/bin/bash
# Script outline to install and build kernel.
# Author: Siddhant Jajoo.

set -e
set -u

OUTDIR=/tmp/aeld
KERNEL_REPO=git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
KERNEL_VERSION=v5.1.10
BUSYBOX_VERSION=1_33_1
FINDER_APP_DIR=$(realpath $(dirname $0))
ARCH=arm64
CROSS_COMPILE=aarch64-none-linux-gnu-

if [ $# -lt 1 ]
then
	echo "Using default directory ${OUTDIR} for output"
else
	OUTDIR=$1
	echo "Using passed directory ${OUTDIR} for output"
fi

mkdir -p ${OUTDIR}

cd "$OUTDIR"
if [ ! -d "${OUTDIR}/linux-stable" ]; then
    #Clone only if the repository does not exist.
	echo "CLONING GIT LINUX STABLE VERSION ${KERNEL_VERSION} IN ${OUTDIR}"
	git clone ${KERNEL_REPO} --depth 1 --single-branch --branch ${KERNEL_VERSION}
fi
if [ ! -e ${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image ]; then
    cd linux-stable
    echo "Checking out version ${KERNEL_VERSION}"
    git checkout ${KERNEL_VERSION}

    # TODO: Add your kernel build steps here
    # aarch64-none-linux-gnu-gcc --version
    make HOSTCC=gcc-9 ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} mrproper
    make HOSTCC=gcc-9 ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} clean
    make HOSTCC=gcc-9 ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} defconfig
    make -j4 HOSTCC=gcc-9 ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} all
    make -j4 HOSTCC=gcc-9 ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} modules
    make HOSTCC=gcc-9 ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} dtbs
fi

echo "Adding the Image in outdir"

echo "Creating the staging directory for the root filesystem"
cd "$OUTDIR"
if [ -d "${OUTDIR}/rootfs" ]
then
	echo "Deleting rootfs directory at ${OUTDIR}/rootfs and starting over"
    sudo rm  -rf ${OUTDIR}/rootfs
fi

# TODO: Create necessary base directories
# File Hierarchy Standdard (FHS)
# /bin              programs to all user, used at booting
# /sbin             programs for the admin, used at boot
# /usr/bin
# /usr/sbin         additional programs, lib, util which are NOT the part of booting
# /dev              device nodes and other files
# /etc              system configuration files
# /lib              shared libraries
# /proc, /sys       proc and sysfs file-system
# /var              files modified at runtime (e.g. /var/log), needed to be retained after booting
# /tmp              temporary files that can be delated on booting
mkdir rootfs
cd rootfs
mkdir bin sbin lib home etc
mkdir dev proc sys tmp
mkdir -p usr/bin usr/sbin usr/lib var/log
# tree
# sudo chown -R root:root *
cd "${OUTDIR}/linux-stable"
make HOSTCC=gcc-9 ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} INSTALL_MOD_PATH="${OUTDIR}/rootfs" modules_install
cp ./arch/arm64/boot/Image "${OUTDIR}"
cd "${OUTDIR}/rootfs"

cd "$OUTDIR"
if [ ! -d "${OUTDIR}/busybox" ]
then
git clone git://busybox.net/busybox.git
    cd busybox
    git checkout ${BUSYBOX_VERSION}
    # TODO:  Configure busybox
    make distclean
    make defconfig
else
    cd busybox
fi

# TODO: Make and install busybox
make HOSTCC=gcc-9 ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} CONFIG_PREFIX="${OUTDIR}/rootfs" install
cd "${OUTDIR}/rootfs"

echo "Library dependencies"
${CROSS_COMPILE}readelf -a bin/busybox | grep "program interpreter"
${CROSS_COMPILE}readelf -a bin/busybox | grep "Shared library"

# TODO: Add library dependencies to rootfs
SYSROOT=`${CROSS_COMPILE}gcc --print-sysroot`
cp -a "${SYSROOT}"/lib64/* lib

# TODO: Make device nodes
sudo mknod -m 666 dev/null c 1 3
sudo mknod -m 666 dev/console c 5 1

# TODO: Clean and build the writer utility

# TODO: Copy the finder related scripts and executables to the /home directory
# on the target rootfs
exit 0
# TODO: Chown the root directory
sudo chown -R root:root *

# TODO: Create initramfs.cpio.gz
