#!/bin/bash
make rebuild
if [ ! -f crack ]; then
	echo "executable introuvable"
	exit 0
else
	./crack 43m1HIBvfMB7k 43 8
fi
