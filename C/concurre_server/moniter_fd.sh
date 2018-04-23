#!/bin/sh

PID=`pgrep $1`
echo $PID
watch ls /proc/$PID/fd	

