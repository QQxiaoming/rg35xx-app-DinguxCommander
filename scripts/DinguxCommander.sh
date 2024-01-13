#!/bin/sh
cd $(dirname "$0")/DinguxCommander
HOME=$(readlink -f ../..)
LD_PRELOAD=./j2k.so ./DinguxCommander.elf
sync
