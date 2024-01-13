#!/bin/sh
progdir=`dirname "$0"`
case "$1" in
	1)
		rm -f $progdir/../DinguxCommander.sh
		cp -f $progdir/文件管理器.sh $progdir/../
		sync
		;;
	2)
		rm -f $progdir/../文件管理器.sh
		cp -f $progdir/DinguxCommander.sh $progdir/../
		sync
		;;
esac
