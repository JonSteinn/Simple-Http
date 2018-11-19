#!/bin/bash
make -C ./src distclean
make -C ./src
make -C ./src clean

if [ $1 ]; then
	./src/main.exe
fi