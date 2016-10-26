import csv

fichier = open('result.csv', 'rb')
cr = csv.reader(fichier, delimiter=';')
i = 0

for row in cr:
	i = i + 1
	if float(row[2]) - float(row[1]) > 0.005:
		print "line " + str(i) + " : " + str(row )


fichier.close()
