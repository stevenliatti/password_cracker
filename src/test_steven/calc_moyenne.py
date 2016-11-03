import csv

fichier = open('test.csv', 'rb')
cr = csv.reader(fichier, delimiter=';')
i = 0
tab = [0,0,0,0,0,0,0,0,0]

for row in cr:
	i += 1;
	#print(int(row[0]))
	tab[int(row[0])] += float(row[1])
	#print(tab[int(row[0])])

i = i / 8
#print(i)

for j in range(len(tab)):
	tab[j] = tab[j] / i
	print(tab[j])

fichier.close()
