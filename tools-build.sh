#!/bin/sh
set -eu

kernel_bin="kernel.bin"
boot_asm="boot/boot.asm"
boot_bin="boot.bin"
max_sectors=17

size=$(stat -c %s "$kernel_bin")
sectors=$(( (size + 511) / 512 ))

if [ "$sectors" -lt 1 ] || [ "$sectors" -gt "$max_sectors" ]; then
    echo "Error: kernel needs $sectors sectors; allowed range is 1-$max_sectors"
    exit 1
fi

echo "Kernel requires $sectors sector(s)"
nasm -f bin -D "KERNEL_SECTORS=$sectors" "$boot_asm" -o "$boot_bin"
