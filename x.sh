#!/bin/bash
MAX=100
for ((count=1; $count<MAX; count++))
do
	./r.out > input
	./a.out < input > out
	./b.out	< input > xout 
	let "mod = $count % 10"
	[ $mod -eq 0 ] && echo $count
	diff xout oout
	[ $? -ne 0 ] && exit 0
done
	echo 'Yeah'
