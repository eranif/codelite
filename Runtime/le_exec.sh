#!/bin/sh

## A small wrapper script to execute command on Linux and keep the console running
## until ENTER is pressed
$@
echo "Press ENTER to continue..."
read key
