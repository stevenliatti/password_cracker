#!/bin/bash
while read line
do
	./crack "$line" 43 8 >> result.txt
done < hash.txt
