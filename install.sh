#!/bin/bash

sudo cp syslibs/* /usr/lib/

echo "export KLEE_ROOT=$(pwd)/" >> ~/.bashrc
echo "export KLEE_LIBMPI=$(pwd)/libs/Azequialib/lib/" >> ~/.bashrc
echo "export KLEE_UCLIBC_ROOT=$(pwd)/libs/libc/" >> ~/.bashrc
echo "export KLEEROOT=$(pwd)" >> ~/.bashrc
echo "export AZQROOT=$(pwd)/libs/Azequialib" >> ~/.bashrc
echo "export PATH=$(pwd):$PATH" >> ~/.bashrc

echo "Installation succeeds."
