#include <GL/gl.h>
#include <math.h>
#include "o_objet.h"
#include "u_table.h"
#include "t_geometrie.h"
#include "figure.h"

struct surface_bezier
{
  Grille_quadruplet grille_controle;
  Grille_quadruplet grille_derive_u;     //  Derivee sur la direction u
  Grille_quadruplet grille_derive_v;     //  Derivee sur la direction v
  Grille_quadruplet grille_normale;
  Grille_quadruplet grille_affichage;

  int nb_points;
  Booleen create_obj;
  Booleen export;
} surface_bezier;

// Indique la sous courbe de bezier que l'on souhaite
typedef enum {
    left_up = 0,
    right_up = 1,
    left_down = 2,
    right_down = 3,
} id_sous_courbe;


// ################################ BEZIER
Quadruplet calcul_un_point(Quadruplet *o, int nb_points_controles, float u) {
  Quadruplet table[nb_points_controles];
  for (unsigned int j = 0; j < nb_points_controles; j++) {
     table[j].x = o[j].x * o[j].h;
     table[j].y = o[j].y * o[j].h;
     table[j].z = o[j].z * o[j].h;
     table[j].h = o[j].h;
  }

  for (unsigned int i = 1; i < nb_points_controles; i++) {
    for (unsigned int j = 0; j < nb_points_controles - i; j++) {
      table[j].x = (1 - u) * table[j].x + u * table[j + 1].x;
      table[j].y = (1 - u) * table[j].y + u * table[j + 1].y;
      table[j].z = (1 - u) * table[j].z + u * table[j + 1].z;
      table[j].h = (1 - u) * table[j].h + u * table[j + 1].h;
    }
  }

  table[0].x = table[0].x / table[0].h;
  table[0].y = table[0].y / table[0].h;
  table[0].z = table[0].z / table[0].h;
  //printf("Return point %f %f %f\n", table[0].x, table[0].y, table[0].z);
  return table[0];
}

Table_quadruplet calcul_bezier(Quadruplet * points_controle, int nb_points_controles, int nb_points) {

  printf("NOK\n");
  Table_quadruplet res;
  ALLOUER(res.table, nb_points);
  res.nb = nb_points;

  printf("Ok\n");
  for (unsigned int i = 0; i < nb_points; i++) {
    printf("Compute %d with %d\n", i, nb_points_controles);
    res.table[i] = calcul_un_point(points_controle, nb_points_controles, 1.0f / (nb_points - 1) * i);
  }

  return res;
}

// ################################ END BEZIER

Grille_quadruplet calcul_surface_bezier(Grille_quadruplet * grille_controle, int nb_points) {
  Grille_quadruplet intermediaire;
  Grille_quadruplet resultat;

  // init grille de resultat
  ALLOUER(resultat.grille, nb_points);
  for (int i = 0; i < nb_points; i++) {
    ALLOUER(resultat.grille[i], nb_points);
  }
  resultat.nb_lignes = nb_points;
  resultat.nb_colonnes = nb_points;

  // init grille intermediaire
  ALLOUER(intermediaire.grille, nb_points);
  for (int i = 0; i < nb_points; i++) {
    ALLOUER(intermediaire.grille[i], nb_points);
  }
  intermediaire.nb_lignes = nb_points;
  intermediaire.nb_colonnes = grille_controle->nb_lignes;

  // first pass, calcul bezier sur chaque ligne et stocke dans chaque colonne de intermediaire
  printf("Compute %d lignes of %d points\n", grille_controle->nb_lignes, nb_points);
  for (int i = 0; i < grille_controle->nb_lignes; i++) {
    Table_quadruplet tab = calcul_bezier(grille_controle->grille[i], grille_controle->nb_colonnes, nb_points);

    // stocke le resultat en colonne au lieu de ligne
    for (int j = 0; j < nb_points; j++) {
      intermediaire.grille[j][i] = tab.table[j];
    }
  }

  //return intermediaire;

  // second pass, calcul bezier sur chaque colonne avec les points precedemment calcules
  for (int i = 0; i < nb_points; i++) {
    printf("Compute %d lignes of %d points\n", nb_points, nb_points);
    Table_quadruplet tab = calcul_bezier(intermediaire.grille[i], intermediaire.nb_colonnes, nb_points);

    // stocke le resultat en colonne au lieu de ligne
    for (int j = 0; j < nb_points; j++) {
      resultat.grille[j][i] = tab.table[j];
    }
  }

  return resultat;
}

// #################################### DERIVEE

/**
 * Produit vectoriel
*/
Quadruplet cross(Quadruplet v1,Quadruplet v2)
{
  Quadruplet crossed;
  crossed.x = (v1.y * v2.z) - (v1.z * v2.y);
  crossed.y = (v1.z * v2.x) - (v1.x * v2.z);
  crossed.z = (v1.x * v2.y) - (v1.y * v2.x);
  return crossed;
}

/**
 * Normalisation 
*/ 
Quadruplet normalize(Quadruplet v)
{
  Quadruplet normalized;
  float norm = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
  normalized.x = v.x / norm;
  normalized.y = v.y / norm;
  normalized.z = v.z / norm;
  return normalized;
}

/**
 * Renvoie la derivee a partir de deux points
*/
Quadruplet derive(Quadruplet p1, Quadruplet p2)
{
    Quadruplet drv;
    drv.x = p2.x - p1.x;
    drv.y = p2.y - p1.y;
    drv.z = p2.z - p1.z;
    return drv;
}

/**
 * Initialisation des derivee
*/
void initDerivatives(struct surface_bezier *s)
{
    // assert(s->grille_controle.nb_lignes > 0 );
    //Allouer la taille des tableaux de derivees
    //Tableau U
    ALLOUER(s->grille_derive_u.grille,s->grille_controle.nb_lignes);

    s->grille_derive_u.nb_lignes = s->grille_controle.nb_lignes;
    s->grille_derive_u.nb_colonnes = s->grille_controle.nb_colonnes;

    for(int i = 0 ; i < s->grille_derive_u.nb_lignes ; i++)
    {
        ALLOUER(s->grille_derive_u.grille[i],s->grille_derive_u.nb_colonnes);
    }

    //Tableau V
    ALLOUER(s->grille_derive_v.grille,s->grille_controle.nb_lignes);

    s->grille_derive_v.nb_lignes = s->grille_controle.nb_lignes;
    s->grille_derive_v.nb_colonnes = s->grille_controle.nb_colonnes;

    for(int i = 0 ; i < s->grille_derive_v.nb_lignes ; i++)
    {
        ALLOUER(s->grille_derive_v.grille[i],s->grille_derive_v.nb_colonnes);
    }
    // printf("Taille de la grille de base [%d,%d]\n Taille grille_derivee_u [%d,%d], Taille grille_derivee_v [%d,%d]\n",
    //   s->grille_controle.nb_lignes,);
}

/**
 * Calcul les derivee dans les deux directions et les sauvegardes dans grille_derive_u et grille_derive_v
*/
void computeDerivatives(struct surface_bezier *s)
{
    // Calculer la grille de derivee sur U
    for(int i = 0 ; i < s->grille_controle.nb_lignes ; i++) //Pour chaque ligne de la grille
    {
        for(int j = 0 ; j < s->grille_controle.nb_colonnes - 1 ; j++)
        {
            s->grille_derive_u.grille[i][j] = derive(s->grille_controle.grille[i][j],s->grille_controle.grille[i][j+1]);
        }
        //On affecte la derivee de la derniere colonne
        s->grille_derive_u.grille[i][s->grille_controle.nb_colonnes - 1] = s->grille_derive_u.grille[i][s->grille_controle.nb_colonnes - 2];
    }
    // Calculer la grille de derivee sur V
    for(int i = 0 ; i < s->grille_controle.nb_colonnes ; i++) //Pour chaque colonne de la grille
    {
        for(int j = 0 ; j < s->grille_controle.nb_lignes - 1 ; j++)
        {
            s->grille_derive_v.grille[j][i] = derive(s->grille_controle.grille[j][i],s->grille_controle.grille[j+1][i]);
        }
        //On affecte la derivee de la derniere ligne
        s->grille_derive_v.grille[s->grille_controle.nb_lignes - 1][i] = s->grille_derive_v.grille[s->grille_controle.nb_lignes - 2][i]; 
    }
}

void computeNormal(struct surface_bezier *s)
{
  //Allouer la grille de normal
  ALLOUER(s->grille_normale.grille, s->grille_controle.nb_lignes );

    s->grille_normale.nb_lignes = s->grille_controle.nb_lignes;
    s->grille_normale.nb_colonnes = s->grille_controle.nb_colonnes;

    for(int i = 0 ; i < s->grille_normale.nb_lignes ; i++)
    {
        ALLOUER(s->grille_normale.grille[i],s->grille_normale.nb_colonnes);
    }

  // Pour chaque points sur la grille sauf les derniers (Pas de derivee)
  for (int i = 0 ; i < s->grille_controle.nb_lignes; i++)
  {
    for (int j = 0 ; j < s->grille_controle.nb_colonnes; j++)
    {
      //Calculer la normal
      s->grille_normale.grille[i][j] = normalize(cross(s->grille_derive_v.grille[i][j],s->grille_derive_u.grille[i][j]));
    }
  }
}

Quadruplet calcul_point_sur_surface(Grille_quadruplet ptControle, float u, float v) {
  // Calculer une courbe de bezier sur u
  Quadruplet curve[ptControle.nb_lignes];

  // printf("Affichage courbe bezier contenant le point en u = %f et v = %f\n",u,v);
  for(int i = 0; i < ptControle.nb_lignes ; i++) {
    curve[i] = calcul_un_point(ptControle.grille[i], ptControle.nb_colonnes, u);
    //  printf("Point %d : [%f,%f,%f]\n",i,curve[i].x,curve[i].y,curve[i].z);
  }
  // Calculer le point v sur cette courbe
  Quadruplet tmp = calcul_un_point(curve, ptControle.nb_lignes, v);
  // printf("Point trouve ! [%f,%f,%f]\n",tmp.x,tmp.y,tmp.z);
  return tmp;
}

Quadruplet getNormal(struct surface_bezier *s, float u, float v )
{
  // Calculer un vecteur de derivee u et un vecteur de derivee v
  // et retourne le resultat de leurs cross product
  Quadruplet deriveeU = calcul_point_sur_surface(s->grille_derive_u,u,v);
  Quadruplet deriveeV = calcul_point_sur_surface(s->grille_derive_v,u,v);
  return normalize(cross(deriveeV,deriveeU));
}

// #################################################### FIN DERIVEE


// #################################################### Création du fichier obj
void create_obj_file(struct surface_bezier* o, char* name)
{
  fprintf(stderr, "creating obj file : nb_lignes = %d, nb_colonnes = %d\n",
    o->grille_affichage.nb_lignes, o->grille_affichage.nb_colonnes );
  FILE * file;
  char* extension = ".obj";
  char* filename = (char *) malloc(1 + strlen(name)+ strlen(extension) );
  strcpy(filename, name);
  strcat(filename, extension);
  file = fopen (filename,"w");
  fprintf (file, "g %s\n", name);
  // Vertices positions
  for (int i = 0; i < o->grille_affichage.nb_lignes; i++)
  {
    for (int j = 0; j < o->grille_affichage.nb_colonnes; j++)
    {
      //fprintf(stderr, "adding a point\n" );
      double x = o->grille_affichage.grille[i][j].x;
      double y = o->grille_affichage.grille[i][j].y;
      double z = o->grille_affichage.grille[i][j].z;
      fprintf (file, "v %f %f %f\n",x, y, z);
    }
  }
  fprintf (file, "\n");
  // Textures
  fprintf (file, "vt 0 0 0\n");
  fprintf (file, "\n");
  // Vertices normals
  for (int i = 0; i < o->grille_normale.nb_lignes; i++)
  {
    for (int j = 0; j < o->grille_normale.nb_colonnes; j++)
    {
      //fprintf(stderr, "adding a point\n" );
      double x = o->grille_normale.grille[i][j].x;
      double y = o->grille_normale.grille[i][j].y;
      double z = o->grille_normale.grille[i][j].z;
      fprintf (file, "vn %f %f %f\n",x, y, z);
    }
  }
  fprintf (file, "\n");
  // Faces
  // une seule texture, toujours 1 en deuxième position
  for (int j = 0; j < o->grille_affichage.nb_lignes - 1; j++)
  {
    for (int i = 1; i < o->grille_affichage.nb_colonnes; i++)
    {
      int current_index = i + j*o->grille_affichage.nb_colonnes;
      int line_under = current_index + o->grille_affichage.nb_colonnes;
      fprintf(file, "f %d/1/%d %d/1/%d %d/1/%d\n", current_index, current_index,
        line_under, line_under, current_index+1, current_index+1);
      fprintf(file, "f %d/1/%d %d/1/%d %d/1/%d\n", current_index+1,
        current_index+1, line_under, line_under, line_under+1, line_under+1);
    }
  }
  fclose (file);
}

void export_grille(Grille_quadruplet * grille, char* name)
{
  fprintf(stderr, "creating grille controle file : nb_lignes = %d, nb_colonnes = %d\n",
    grille->nb_lignes, grille->nb_colonnes );
  FILE * file;
  char* extension = ".grille";
  char* filename = (char *) malloc(1 + strlen(name)+ strlen(extension) );
  strcpy(filename, name);
  strcat(filename, extension);
  file = fopen (filename,"w");
  //fprintf (file, "g %s\n", name);

  fprintf (file, "l %d\n", grille->nb_lignes);
  fprintf (file, "c %d\n", grille->nb_colonnes);

  // Vertices positions
  for (int i = 0; i < grille->nb_lignes; i++)
  {
    for (int j = 0; j < grille->nb_colonnes; j++)
    {
      double x = grille->grille[i][j].x;
      double y = grille->grille[i][j].y;
      double z = grille->grille[i][j].z;
      fprintf (file, "v %f %f %f\n",x, y, z);
    }
  }
  fprintf (file, "\n");

  fclose (file);
}

// #################################################### Fin Création du fichier obj

// #################################################### Englober la surface

//Dessiner boite
void drawBoxWireframe(Quadruplet *pmin, Quadruplet *pmax) {
  glColor3f(1.0, 1.0, 1.0);
  
  glBegin(GL_LINE_STRIP);
  Quadruplet t = *pmin;
  glVertex3f(t.x, t.y, t.z);
  t.z = pmax->z;
  glVertex3f(t.x, t.y, t.z);
  t.x = pmax->x;
  glVertex3f(t.x, t.y, t.z);
  t.z = pmin->z;
  glVertex3f(t.x, t.y, t.z);
  t.x = pmin->x;
  glVertex3f(t.x, t.y, t.z);
  t.y = pmax->y;
  glVertex3f(t.x, t.y, t.z);
  t.x = pmax->x;
  glVertex3f(t.x, t.y, t.z);
  t.y = pmin->y;
  glVertex3f(t.x, t.y, t.z);
  glEnd();

  glBegin(GL_LINE_STRIP);
  t = *pmin;
  t.z = pmax->z;
  glVertex3f(t.x, t.y, t.z);
  t.y = pmax->y;
  glVertex3f(t.x, t.y, t.z);
  t.x = pmax->x;
  glVertex3f(t.x, t.y, t.z);
  t.y = pmin->y;
  glVertex3f(t.x, t.y, t.z);
  glEnd();

  glBegin(GL_LINES);
  t = *pmax;
  glVertex3f(t.x, t.y, t.z);
  t.z = pmin->z;
  glVertex3f(t.x, t.y, t.z);
  t.x = pmin->x;
  glVertex3f(t.x, t.y, t.z);
  t.z = pmax->z;
  glVertex3f(t.x, t.y, t.z);
  glEnd();
}


//Calcule la boite min,max et stocke le résultat dans pmin, pmax (points déjà alloués)
void compute_boite_min_max(Grille_quadruplet * g, Quadruplet *pmin, Quadruplet *pmax) {

  *pmin = g->grille[0][0];
  *pmax = g->grille[0][0];
  for (int i=0; i<g->nb_lignes; ++i) {
    for (int j=0; j<g->nb_colonnes; ++j) {
      
      if (g->grille[i][j].x < pmin->x)
        pmin->x = g->grille[i][j].x;
      if (g->grille[i][j].y < pmin->y)
        pmin->y = g->grille[i][j].y;
      if (g->grille[i][j].z < pmin->z)
        pmin->z = g->grille[i][j].z;

      if (g->grille[i][j].x > pmax->x)
        pmax->x = g->grille[i][j].x;
      if (g->grille[i][j].y > pmax->y)
        pmax->y = g->grille[i][j].y;
      if (g->grille[i][j].z > pmax->z)
        pmax->z = g->grille[i][j].z;
    }
  }
}

// #################################################### Fin englober surface


// #################################################### Debut decoupe
void decoupe_surface(Grille_quadruplet * points_controle, Grille_quadruplet * out_so,
                                                          Grille_quadruplet * out_se,
                                                          Grille_quadruplet * out_no,
                                                          Grille_quadruplet * out_ne) {
    int n = points_controle->nb_colonnes;
	float u = 1./2.;

	//grilles intermediaires contenant les parties gauche et droite de la surface

	Grille_quadruplet left;
	Grille_quadruplet right;
	ALLOUER(left.grille, points_controle->nb_lignes);
	ALLOUER(right.grille, points_controle->nb_lignes);
    for (int i = 0; i < points_controle->nb_lignes; i++) {
        ALLOUER(left.grille[i], (int)ceil(n / 2.) + 1);
        ALLOUER(right.grille[i], (int)ceil(n /2.) + 1);
    }
    left.nb_lignes = points_controle->nb_lignes;
    left.nb_colonnes = (int)ceil(n / 2.) + 1;
	right.nb_lignes = points_controle->nb_lignes;
    right.nb_colonnes = (int)ceil(n / 2.) + 1;
	
	//initialisation des sous grilles resultantes
	//(=> penser à free dans la fonction principale apres les tests d'intersection)
	out_so->nb_lignes = (int)ceil(points_controle->nb_lignes / 2.) + 1;
	out_se->nb_lignes = out_so->nb_lignes;
	out_no->nb_lignes = out_so->nb_lignes;
	out_ne->nb_lignes = out_so->nb_lignes; 

	out_so->nb_colonnes = (int)ceil(n / 2.) + 1;
	out_se->nb_colonnes = out_so->nb_colonnes;
	out_no->nb_colonnes = out_so->nb_colonnes;
	out_ne->nb_colonnes = out_so->nb_colonnes;

	ALLOUER(out_so->grille, out_so->nb_lignes);
	ALLOUER(out_se->grille, out_se->nb_lignes);
	ALLOUER(out_no->grille, out_no->nb_lignes);
	ALLOUER(out_ne->grille, out_ne->nb_lignes);
    for (int i = 0; i < out_so->nb_lignes; i++) {
        ALLOUER(out_so->grille[i], out_so->nb_colonnes);
        ALLOUER(out_se->grille[i], out_se->nb_colonnes);
        ALLOUER(out_no->grille[i], out_no->nb_colonnes);
        ALLOUER(out_ne->grille[i], out_ne->nb_colonnes);
    }

	//afin de ne pas dénaturer les sous-surfaces par rapport celle d'origine, on cherche à créer de nouveaux points de controles.

	//declaration des nouveaux points
	Quadruplet a; 
	Quadruplet b;
	Quadruplet c; 
	Quadruplet d; 
	Quadruplet new_p1;
	Quadruplet new_p2;
	Quadruplet new_p3;

	//decoupage des lignes
	for (int i = 0; i < points_controle->nb_lignes; i++) {
        //on determine les segments à decouper (auquels on applique de casteljau)
        if (n % 2) {
            //impair
            int mid = floor(n / 2.);
            a = points_controle->grille[i][mid - 1];
            b = points_controle->grille[i][mid];
            c = b;
            d = points_controle->grille[i][mid + 1];
        } else {
            //pair
            a = points_controle->grille[i][n / 2 - 2];
            b = points_controle->grille[i][n / 2 - 1];
            c = points_controle->grille[i][n / 2];
            d = points_controle->grille[i][n / 2 + 1];
        }

        new_p1.x = u * a.x + u * b.x;
   		new_p1.y = u * a.y + u * b.y;
   		new_p1.z = u * a.z + u * b.z;
  		new_p1.h = u * a.h + u * b.h;

        new_p2.x = u * c.x + u * d.x;
        new_p2.y = u * c.y + u * d.y;
    	new_p2.z = u * c.z + u * d.z;
    	new_p2.h = u * c.h + u * d.h;
     	
        new_p3.x = u * new_p1.x + u * new_p2.x;
    	new_p3.y = u * new_p1.y + u * new_p2.y;
    	new_p3.z = u * new_p1.z + u * new_p2.z;
    	new_p3.h = u * new_p1.h + u * new_p2.h;
        
        //on remplit les grilles intermediaires avec les points de la surface d'origine
        for (int j = 0; j < left.nb_colonnes - 2; j++) {
            left.grille[i][j] = points_controle->grille[i][j];
      		right.grille[i][left.nb_colonnes - j - 1] = points_controle->grille[i][points_controle->nb_colonnes - j - 1];
        }
        
        //on complete avec les nouveaux points
        left.grille[i][left.nb_colonnes - 2] = new_p1;
        left.grille[i][left.nb_colonnes - 1] = new_p3;
        right.grille[i][0] = new_p3;
        right.grille[i][1] = new_p2;
    }

	Quadruplet ar; 
	Quadruplet br;
	Quadruplet cr; 
	Quadruplet dr; 

	Quadruplet new_p1r;
	Quadruplet new_p2r;
	Quadruplet new_p3r;

	//decoupage des colonnes (on decoupe en même temps les deux grilles intermediaires left et right)
	n = left.nb_lignes;
	for (int i = 0; i < left.nb_colonnes; i++) {
        if (n % 2) {
            //impair
            int mid = floor(n / 2.);
            a = left.grille[mid - 1][i];
            b = left.grille[mid][i];
            c = b;
            d = left.grille[mid + 1][i];
            ar = right.grille[mid - 1][i];
            br = right.grille[mid][i];
            cr = br;
            dr = right.grille[mid + 1][i];
        } else {
            //pair
            a = left.grille[n / 2 - 2][i];
            b = left.grille[n / 2 - 1][i];
            c = left.grille[n / 2][i];
            d = left.grille[n / 2 + 1][i];
            ar = right.grille[n / 2 - 2][i];
            br = right.grille[n / 2 - 1][i];
            cr = right.grille[n / 2][i];
            dr = right.grille[n / 2 + 1][i];
        }

        new_p1.x = u * a.x + u * b.x;
        new_p1.y = u * a.y + u * b.y;
        new_p1.z = u * a.z + u * b.z;
        new_p1.h = u * a.h + u * b.h;

        new_p2.x = u * c.x + u * d.x;
        new_p2.y = u * c.y + u * d.y;
        new_p2.z = u * c.z + u * d.z;
        new_p2.h = u * c.h + u * d.h;
     	
        new_p3.x = u * new_p1.x + u * new_p2.x;
        new_p3.y = u * new_p1.y + u * new_p2.y;
        new_p3.z = u * new_p1.z + u * new_p2.z;
        new_p3.h = u * new_p1.h + u * new_p2.h;
        
        new_p1r.x = u * ar.x + u * br.x;
   		new_p1r.y = u * ar.y + u * br.y;
   		new_p1r.z = u * ar.z + u * br.z;
  		new_p1r.h = u * ar.h + u * br.h;

        new_p2r.x = u * cr.x + u * dr.x;
        new_p2r.y = u * cr.y + u * dr.y;
        new_p2r.z = u * cr.z + u * dr.z;
        new_p2r.h = u * cr.h + u * dr.h;
     	
        new_p3r.x = u * new_p1r.x + u * new_p2r.x;
        new_p3r.y = u * new_p1r.y + u * new_p2r.y;
        new_p3r.z = u * new_p1r.z + u * new_p2r.z;
        new_p3r.h = u * new_p1r.h + u * new_p2r.h;

        //on remplit les grilles resultantes avec les points des frilles interm
        for (int j = 0; j < out_so->nb_lignes - 2; j++) {
            out_no->grille[j][i] = left.grille[j][i];
            out_ne->grille[j][i] = right.grille[j][i];
            out_so->grille[out_so->nb_lignes - j - 1][i] = left.grille[left.nb_lignes - j - 1][i];
            out_se->grille[out_se->nb_lignes - j - 1][i] = right.grille[right.nb_lignes - j - 1][i];
        }
        //puis ajout des nouveaux points
        out_no->grille[out_no->nb_lignes - 2][i] = new_p1;
        out_no->grille[out_no->nb_lignes - 1][i] = new_p3;
        
        out_ne->grille[out_ne->nb_lignes - 2][i] = new_p1r;
        out_ne->grille[out_ne->nb_lignes - 1][i] = new_p3r;
        
        out_so->grille[0][i] = new_p3;
        out_so->grille[1][i] = new_p2;
        
        out_se->grille[0][i] = new_p3r;
        out_se->grille[1][i] = new_p2r;
    
    }
    //libération des grilles intermediaires

 	for (int i = 0; i < left.nb_lignes; ++i) {
        free(left.grille[i]);
        free(right.grille[i]);
    }
    free(left.grille);
    free(right.grille);
}

// #################################################### Fin decoupe


struct Ray {
  Quadruplet o;
  Quadruplet e;
};

Quadruplet plusQQ(Quadruplet q, Quadruplet t){
  q.x = q.x + t.x;
  q.y = q.y + t.y;
  q.z = q.z + t.z;
  q.h = q.h + t.h;
  return q;
}

Quadruplet minusQQ(Quadruplet q, Quadruplet t){
  q.x = q.x - t.x;
  q.y = q.y - t.y;
  q.z = q.z - t.z;
  q.h = q.h - t.h;
  return q;
}

Quadruplet divQD(Quadruplet q, double d){
  q.x/=d;
  q.y/=d;
  q.z/=d;
  q.h/=d;
  return q;
}

double min(double a, double b){
  return (a<b) ? a : b;
}

double max(double a, double b){
  return (a>=b) ? a : b;
}

double normQ(Quadruplet t){
  return sqrt(t.x*t.x+t.y*t.y+t.z*t.z+t.h*t.h);
}

Booleen intersect_boite(struct Ray * ray, Quadruplet * pMin, Quadruplet * pMax){
  Quadruplet vectorRay, inverseVectorRay;
  vectorRay = minusQQ(ray->e, ray->o);
  vectorRay = divQD(vectorRay, normQ(vectorRay));

  inverseVectorRay.x = 1.0 / vectorRay.x;
  inverseVectorRay.y = 1.0 / vectorRay.y;
  inverseVectorRay.z = 1.0 / vectorRay.z;

  double tmin, tmax, txmin, txmax, tymin, tymax, tzmin, tzmax;

  if (vectorRay.x >= 0) {
    txmin = (pMin->x - ray->o.x) * inverseVectorRay.x;
    txmax = (pMax->x - ray->o.x) * inverseVectorRay.x;
  }
  else {
    txmin = (pMax->x - ray->o.x) * inverseVectorRay.x;
    txmax = (pMin->x - ray->o.x) * inverseVectorRay.x;
  } 

  if (vectorRay.y >= 0) {
    tymin = (pMin->y - ray->o.y) * inverseVectorRay.y;
    tymax = (pMax->y - ray->o.y) * inverseVectorRay.y;
  }
  else {
    tymin = (pMax->y - ray->o.y) * inverseVectorRay.y;
    tymax = (pMin->y - ray->o.y) * inverseVectorRay.y;
  } 

  if ((txmin > tymax) || (tymin > txmax)){
    // return false;
    return 0;
  }

  tmin = max(txmin, tymin);
  tmax = min(txmax, tymax);

  if (vectorRay.z >= 0) {
    tzmin = (pMin->z - ray->o.z) * inverseVectorRay.z;
    tzmax = (pMax->z - ray->o.z) * inverseVectorRay.z;
  }
  else {
    tzmin = (pMax->z - ray->o.z) * inverseVectorRay.z;
    tzmax = (pMin->z - ray->o.z) * inverseVectorRay.z;
  } 

  if ((tmin > tzmax) || (tzmin > tmax)){
    // return false;
    return 0;
  }

  tmin = max(tmin, tzmin);
  tmax = min(tmax, tzmax);

  // renvois vrai si le rayon intersect la box
  return 1;
}


// #################################################### Debut trouve intersection

float epsilon = 0.001;

Booleen intersect_surface(Grille_quadruplet * grille, struct Ray * ray, 
      float u_min, float u_max, float v_min, float v_max,
      float * u, float * v) {
  if (u_max - u_min < epsilon && v_max - v_min < epsilon) {
    *u = (u_max + u_min) / 2.0f;
    *v = (v_max + v_min) / 2.0f;
    return 1;
  } 

  Quadruplet pmin, pmax;
  compute_boite_min_max(grille, &pmin, &pmax);

  if (intersect_boite(ray, &pmin, &pmax)) {
    Grille_quadruplet * so, * se, * no, * ne;
    decoupe_surface(grille, so, se, no, ne);

    float u_mid = u_min + u_max / 2.0f;
    float v_mid = v_min + v_max / 2.0f;

    /*
    if(intersect_surface(so, ray, u_min, u_mid, v_min, v_mid, u, v)) return true;
    if(intersect_surface(se, ray, u_mid, u_max, v_min, v_mid, u, v)) return true;
    if(intersect_surface(no, ray, u_min, u_mid, v_mid, v_max, u, v)) return true;
    if(intersect_surface(ne, ray, u_mid, u_max, v_mid, v_max, u, v)) return true;
    */

    return
      intersect_surface(so, ray, u_min, u_mid, v_min, v_mid, u, v) || 
      intersect_surface(se, ray, u_mid, u_max, v_min, v_mid, u, v) ||
      intersect_surface(no, ray, u_min, u_mid, v_mid, v_max, u, v) ||
      intersect_surface(ne, ray, u_mid, u_max, v_mid, v_max, u, v);
  }

  return 0;
}


// #################################################### Fin trouve intersection

static void changement(struct surface_bezier *o)
{
  printf("CHANGEMENT");
  if ( ! (UN_CHAMP_CHANGE(o)|| CREATION(o)) )
    return ;

  if (CHAMP_CHANGE(o, grille_controle) || CHAMP_CHANGE(o, nb_points))
  {

    if (o->nb_points < 10) o->nb_points = 10;

    // desallocation grille affichage
    for (int i = 0; i < o->grille_affichage.nb_lignes; i++) {
      free(o->grille_affichage.grille[i]);
    }
    free(o->grille_affichage.grille);

    export_grille(&o->grille_controle, "test");

    o->grille_affichage = calcul_surface_bezier(&o->grille_controle, o->nb_points);
    initDerivatives(o);
    computeDerivatives(o);
    computeNormal(o);
    printf("dans changement\n");
  }
  else if (CHAMP_CHANGE(o, create_obj))
  {
    create_obj_file(o, "bezier");
    o->create_obj = 0;
  }
  else if (CHAMP_CHANGE(o, export))
  {
    export_grille(&o->grille_controle, "bezier");
    o->export = 0;
  }
  else
    printf(" sans changement\n");
}

static void affiche_surface_bezier(struct surface_bezier *o)
{
  // Affiche grille de controle
  glColor3f(1, 1, 1);
  for (int i = 0; i < o->grille_controle.nb_lignes; i++) {
    glBegin(GL_LINE_STRIP);
    for (int j = 0; j < o->grille_controle.nb_colonnes; j++) {
      glVertex3f(o->grille_controle.grille[i][j].x, o->grille_controle.grille[i][j].y, o->grille_controle.grille[i][j].z);
    }
    glEnd();
  }

  for (int i = 0; i < o->grille_controle.nb_colonnes; i++) {
    glBegin(GL_LINE_STRIP);
    for (int j = 0; j < o->grille_controle.nb_lignes; j++) {
      glVertex3f(o->grille_controle.grille[j][i].x, o->grille_controle.grille[j][i].y, o->grille_controle.grille[j][i].z);
    }
    glEnd();
  }

  // Affiche surface de bezier
  glColor3f(0, 1, 1);
  for (int i = 0; i < o->grille_affichage.nb_lignes; i++) {
    glBegin(GL_LINE_STRIP);
    for (int j = 0; j < o->grille_affichage.nb_colonnes; j++) {
      glVertex3f(o->grille_affichage.grille[i][j].x, o->grille_affichage.grille[i][j].y, o->grille_affichage.grille[i][j].z);
    }
    glEnd();
  }

  for (int i = 0; i < o->grille_affichage.nb_colonnes; i++) {
    glBegin(GL_LINE_STRIP);
    for (int j = 0; j < o->grille_affichage.nb_lignes; j++) {
      glVertex3f(o->grille_affichage.grille[j][i].x, o->grille_affichage.grille[j][i].y, o->grille_affichage.grille[j][i].z);
    }
    glEnd();
  }

  // Affiche normale
  glColor3f(0, 1, 0);

  Quadruplet tmp;
  for (int i = 0; i < o->grille_normale.nb_lignes; i++) {

    glBegin(GL_LINES);

    for (int j = 0; j < o->grille_normale.nb_colonnes; j++) {
      tmp.x = o->grille_controle.grille[i][j].x + o->grille_normale.grille[i][j].x;
      tmp.y = o->grille_controle.grille[i][j].y + o->grille_normale.grille[i][j].y;
      tmp.z = o->grille_controle.grille[i][j].z + o->grille_normale.grille[i][j].z;
      tmp.h = 1;
      glVertex3f(o->grille_controle.grille[i][j].x, o->grille_controle.grille[i][j].y, o->grille_controle.grille[i][j].z);
      glVertex3f(tmp.x,tmp.y,tmp.z);

    }
    glEnd();
  }

  glColor3f(0, 1, 1);
  glBegin(GL_LINES);
  Quadruplet normal = getNormal(o,0.5,0.5); //TODO le créer dans changement, ne pas calculer dans affichage
  tmp.x = 0.5;
  tmp.y = 0.5;
  tmp.z = 2.;
  glVertex3f(tmp.x,tmp.y,tmp.z);
  glVertex3f(tmp.x + normal.x, tmp.y + normal.y, tmp.z + normal.z);
  // glVertex3f(5., 6., 10.);
  glEnd();


  //Affiche boite min/max de la grille de départ
  Quadruplet pmin,pmax;
  compute_boite_min_max(&o->grille_affichage, &pmin, &pmax);
  drawBoxWireframe(&pmin,&pmax);
  

}

CLASSE(surface_bezier, struct surface_bezier,
       CHAMP(grille_controle, L_grille_point P_grille_quadruplet Extrait Obligatoire)
       CHAMP(nb_points, LABEL("Nombre de points")  L_entier Affiche Edite Sauve DEFAUT("10") )

       CHANGEMENT(changement)
       CHAMP_VIRTUEL(L_affiche_gl(affiche_surface_bezier))

       CHAMP(create_obj,
         LABEL("Créer fichier obj")
         L_booleen
         Affiche
         Edite DEFAUT("0")
       )

       CHAMP(export,
         LABEL("Export grille")
         L_booleen
         Affiche
         Edite DEFAUT("0")
       )

       MENU("Exemples_MG/surface_bezier")
       EVENEMENT("Ctrl+PB")
       )
