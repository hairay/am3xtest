#!/bin/sh

uname -m
cd `dirname $0`

case `uname -m` in
  arm64|aarch64|aarch64_be|armv8b|armv8l)
    echo "arm64"
    APP='./am3xtest-arm64'
    ;;
  x86_64|amd64|AMD64)
    echo "x86_64"    
    APP='./am3xtest-x64'
    ;;
  MIPS64|mips64|mips)
    echo "mips64"
    APP='./am3xtest-mips64'
    ;;
  arm32|aarch32|armv7l)
    echo "arm32"
    APP='./am3xtest-arm32'
    ;;
  *)
    echo "Unknown architecture `uname -m`."
    exit 1
    ;;
esac

which sudo
if [ $? -ne 0 ]; then
    ${APP}  
else
	sudo ${APP}
fi

echo "done"
