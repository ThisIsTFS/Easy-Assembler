#!/bin/bash

# Assembling, linking and execution in just one script.
# By: Mohammad Hossein Aghaee - TFS

if [ -z "$1" ]; then
    echo "Easy Assembler. Usage: asm <filename.s> or asm <filename.asm>"
    exit 1
fi

fileName="${1%.*}"
extension="${1##*.}"

if [[ "$extension" != "s" && "$extension" != "asm" ]]; then
    echo "Unsupported file extension. Please use .s or .asm"
    exit 1
fi

if [ ! -f "$1" ]; then
    echo "ERR: provided file not found: $1"
    exit 1
fi

arch=$(uname -m)
if [ "$arch" == "x86_64" ]; then
    nasm_format="elf64"
elif [ "$arch" == "i686" ] || [ "$arch" == "i386" ]; then
    nasm_format="elf32"
else
    echo "Unsupported system architecture: $arch"
    exit 1
fi

if [ "$nasm_format" == "elf32" ]; then
    if grep -q -E '\b(r[8-9]|[1-5])\b|\brax\b|\brbx\b|\brcx\b|\brdx\b|\brsi\b|\brdi\b|\bmov[[:space:]]+r' "$fileName"."$extension"; then
        echo "ERR: The provided assembly code is not supported by your system's architecture."
        exit 1
    fi
fi

nasm -f $nasm_format "$fileName"."$extension"
ld "${fileName}.o" -o "$fileName"

[ "$2" == "-g" ] && gdb -q "$fileName" || ./"$fileName"
