# Journal de bord

### Discussion Ludo-Steven du 13.10
On a déterminé l'idée générale : on filerait à chaque thread un "pack" de 65 itérations 
sur une séquence de caractères à tester (exemple pour 2 char : a* au premier thread, b* 
au 2ème, etc.), si le mot de passe s'y trouve, on arrête tout (cancel et tutti quanti) 
et on affiche le password. Sinon, le thread récupère un nouveau jeu de 65 mots de passe 
(genre pour 3 char : aa* ou pour 2 char : c*) et on continue.

**plein de petites boucles (65 iter) VS peu de grandes boucles (65^n iter) ???**

**1 boucle -> 1 thread**

Pseudo-code :

~~~~~~~ { .c .numberLines startFrom="10" }
// Fonction appelée par pthread_create
func(hash_password, nbr_char_à_tester, tab_char_fixe) {
	for() {
		// Tester les valeurs
	}
	// Si pas trouvé, appel récursif ??? --> POSSIBLE OU PAS AVEC LES THREADS ???
	func(hash_password, nbr_char_à_tester_différent, tab_char_fixe);
}
~~~~~~~

On réfléchissait aussi à comment stocker l'alphabet :
- Simple tableau de char à une dimension ?
- Tableau à *Nbr_de_threads* dimensions ? Plus opti, ou sers à rien ?
