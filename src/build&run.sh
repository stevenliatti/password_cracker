#!/bin/bash
make rebuild
if [ ! -f crack ]; then
	echo "executable introuvable"
	exit 0
else
	./crack 43WVh8ipueU4Q 43 2
fi 


