#!/bin/bash
make rebuild
if [ ! -f crack ]; then
	echo "executable introuvable"
	exit 0
else
	./crack 1234 43 10
fi