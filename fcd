#!/bin/bash

if [ -n "$1" ]
then
	echo " " > temp_result
	chmod ugo+r temp_result
	./fcd_c $1
	TEMP_RESULT=$(cat temp_result)
	rm temp_result
	cd $TEMP_RESULT
##	echo $TEMP_RESULT
	exec bash
else
	./fcd_c
fi
