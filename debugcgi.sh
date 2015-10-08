#!/bin/bash

PID=`ps ax | grep $1 | awk '{ print $1 }'`
su -c "gdb --pid $PID --symbols=$2"
