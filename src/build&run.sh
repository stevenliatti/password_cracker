#!/bin/bash
make
if [ ! -f crack ]; then
	echo "executable introuvable"
	exit 0
else
	./crack 431pugYzZc5QM 43 2
fi 


