#!/bin/sh

## A small wrapper script to execute command on Linux and keep the console running
## until ENTER is pressed
## Process any arguments that were supplied
until [ -z "$1" ]  # Until all parameters used up
do
	## each argument of the form name=value
 	## are handled as environment variable and exported
	## by this script to the target application
	var=$1
	variable_name=`echo $var | cut -s -d= -f1`
	variable_value=`echo $var | cut -s -d= -f2`
	if [ "$variable_value" != "" ]; then
		export $variable_name=$variable_value
	else
		break;
	fi
	shift;
done

$@
echo "Press ENTER to continue..."
read key

