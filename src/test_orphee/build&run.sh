#!/bin/bash
make rebuild
if [ ! -f crack ]; then
	echo "executable introuvable"
	exit 0
else
	./crack oooo 43 64
fi
