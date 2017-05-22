#!/bin/bash

PID=`ps ax | grep $1 | grep -v grep | grep -v debugcgi | awk '{ print $1 }'`
su -c "gdb -p $PID -symbols=$2"
