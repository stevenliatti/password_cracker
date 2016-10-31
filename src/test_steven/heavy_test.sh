#!/bin/bash
i=1
for i in {1..100000}
do
	echo "$i"
	arguments=$(./random_hash)
	./crack $arguments >> result.csv
done

