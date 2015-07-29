#!/bin/bash

PID=`ps ax | grep $1 | grep -v grep | awk ' { print $1 }'`
gdb --pid $PID --symbols=$2
