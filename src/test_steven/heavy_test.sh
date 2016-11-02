#!/bin/bash
i=1
for i in {1..1000}
do
	#echo "$i"
	#arguments=$(./random_hash)
	./crack 43p0Ej8w7jMqs 43 "$i" >> result2.csv
done

