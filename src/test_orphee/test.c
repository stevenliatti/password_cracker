#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* alphabet = "abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ!*~";
const int alphabet_size = 65;
const int passwd_size = 8;

int main() {
  int pos;
  int reste;
  int quotient;
  char affichage[passwd_size],temp[passwd_size],convert[2];


  for (int i = 0; i < 4225; i++) {
    pos = i;
    quotient=1;
    convert[1]='\0';
    affichage[0]='\0';

    while (quotient != 0) {
      quotient = pos / alphabet_size;
      reste = pos % alphabet_size;
      pos = quotient;
      convert[0] = alphabet[reste];

      strcpy(temp,convert);
      strcat(temp,affichage);
      strcpy(affichage,temp);
    }
    printf("%d en base %d = %s\n",i,alphabet_size,affichage);
  }
}
