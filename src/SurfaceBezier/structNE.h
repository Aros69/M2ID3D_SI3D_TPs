#ifndef STRUCTNE_H
#define STRUCTNE_H

#include <stdlib.h> // pour le malloc en C++

#define ALLOUER(X,NB) (X)= (typeof (X)) malloc(NB*sizeof(*X))
// j'ai du rajouter le typeof pour la compil en C++


// pas de typedef en C++
typedef struct Triplet
{
	double x,y,z;
} Triplet;

typedef struct Table_triplet
{
	int nb;
	Triplet *table; 
} Table_triplet;

typedef struct Grille_triplet
{
	int nb_lignes;
	int nb_colonnes;
	Triplet **grille; 
} Grille_triplet;

typedef struct Quadruplet
{
	double x,y,z,h;
} Quadruplet;

typedef struct Table_quadruplet
{
	int nb;
	Quadruplet *table;   
} Table_quadruplet;

typedef struct Grille_quadruplet
{
	int nb_lignes;
	int nb_colonnes;
	Quadruplet **grille; 
} Grille_quadruplet;

#endif
