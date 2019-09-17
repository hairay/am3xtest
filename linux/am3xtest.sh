#!/bin/bash

uname -m
cd `dirname $0`

case `uname -m` in
  arm64|aarch64|aarch64_be|armv8b|armv8l)
    echo "arm64"
    sudo ./am3xtest-arm64
    ;;
  x86_64|amd64|AMD64)
    echo "x86_64"    
    sudo ./am3xtest-x64
    ;;
  MIPS64|mips64|mips)
    echo "mips64"
    sudo ./am3xtest-mips64
    ;;
  arm32|aarch32|arm)
    echo "arm32"
    sudo ./am3xtest-arm32
    ;;
  *)
    echo "Unknown architecture `uname -m`."
    exit 1
    ;;
esac
  
echo "done"
