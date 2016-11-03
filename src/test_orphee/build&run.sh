#!/bin/bash
make
for (( i=1; i<=$4; i++ ))
do
	if [ ! -f crack ]; then
		echo "executable introuvable"
		exit 0
	else
		for (( j=$1; j<=$2; j++ ))
		do
			./crack 43p0Ej8w7jMqs 43 "$j" >> "$3"
		done
	fi
done
