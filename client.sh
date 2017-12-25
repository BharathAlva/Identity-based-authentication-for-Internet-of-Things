#!/bin/sh
dir=`pwd`
if [ $1 = 'CLIENT1' ]
then
	if [ $2 = 'SERVER' ]
	then	
	$dir/client localhost 3001 0
	elif [ $2 = 'CLIENT' ]
	then	
	$dir/client localhost 3001 1
	else
	echo "Provide proper info"
	fi
elif [ $1 = 'CLIENT2' ]
then
	
	 $dir/client2 localhost 3001 0
else
	echo "Provide proper info"
fi
