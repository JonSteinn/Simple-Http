#!/bin/bash
make -C ./src distclean
make -C ./src
make -C ./src clean

mkdir -p log

# Prepend following if glib gives trouble beyond still reachable
# G_DEBUG=gc-friendly G_SLICE=always-malloc
valgrind \
    --leak-check=full \
    --show-leak-kinds=all \
    --track-origins=yes \
    --verbose \
    --show-reachable=no \
    --leak-resolution=high \
    --log-file=log/valgrind-report.txt \
    ./src/main.exe "$@"