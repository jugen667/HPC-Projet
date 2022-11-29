// Algorithme SD - Optimisation SIMD 
// Authors : Julien GENTY - Youssef LAKHDHAR

#include "nrutil.h"
#include "nrio2.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "src/mipp.h"

#define N_SD 	2
#define V_MIN 	1
#define V_MAX 	254

void erosion(uint8** Array1, uint8** Array2, uint8** sortie, int* nrh, int* nch){
	for (short i = 1; i<*nrh-1; i++){
		for (short j = 1; j<*nch-1; j++){
			if (
			(Array1[i][j] && Array2[i-1][j-1]) &&
			(Array1[i][j] && Array2[i-1][j])	 &&
			(Array1[i][j] && Array2[i-1][j+1]) &&
			(Array1[i][j] && Array2[i][j+1])   &&
			(Array1[i][j] && Array2[i][j-1])   &&
			(Array1[i][j] && Array2[i+1][j-1]) &&
			(Array1[i][j] && Array2[i+1][j+1]) &&
			(Array1[i][j] && Array2[i+1][j])){
				sortie[i][j] = 255;
			}
			else{
				sortie[i][j] = 0;
			}
		}
	}
}

void dilatation(uint8** Array1, uint8** Array2, uint8** sortie, int* nrh, int* nch){
	for (short i = 1; i<*nrh-1; i++){
		for (short j = 1; j<*nch-1; j++){
			if (
			(Array1[i][j] || Array2[i-1][j-1]) ||
			(Array1[i][j] || Array2[i-1][j])   ||
			(Array1[i][j] || Array2[i-1][j+1]) ||
			(Array1[i][j] || Array2[i][j+1])   ||
			(Array1[i][j] || Array2[i][j-1])   ||
			(Array1[i][j] || Array2[i+1][j-1]) ||
			(Array1[i][j] || Array2[i+1][j+1]) ||
			(Array1[i][j] || Array2[i+1][j])){
				sortie[i][j] = 255;
			}
			else{
				sortie[i][j] = 0;
			}
		}
	}
}


int main(){
	time_t start, end;
	// Variables
	int* nrl = (int*) malloc(sizeof(int)); 
	int* nrh = (int*) malloc(sizeof(int)); 
	int* ncl = (int*) malloc(sizeof(int)); 
	int* nch = (int*) malloc(sizeof(int));
	char string[24] = "car3/car_3000.pgm";
	char stringE[24] = "sortie/SortieE.pgm";
	short image_nb = 3000;


	// Chargement et initialisationdes matrices
	uint8** matrice = LoadPGM_ui8matrix(string, nrl, nrh, ncl, nch);
	uint8** M_Array = matrice;
	uint8** V_Array = ui8matrix((long)*nrl, (long)*nrh, (long)*ncl, (long)*nch);
	set_ui8matrix(V_Array, *nrl, *nrh, *ncl, *nch, V_MIN);
	uint8** Output_Array = ui8matrix((long)*nrl, (long)*nrh, (long)*ncl, (long)*nch);
	uint8** E_Array = ui8matrix((long)*nrl, (long)*nrh, (long)*ncl, (long)*nch);
	uint8** Morphed_Array = ui8matrix((long)*nrl, (long)*nrh, (long)*ncl, (long)*nch);


	// Registres MIPP pour SIMD
	mipp::Reg<int16_t> array; 				// Registre matrice image de base
	mipp::Reg<int16_t> arraymoy; 			// Registre moyenne de l'image
	mipp::Reg<int16_t> sortie;				// Matrice de sortie de l'image
	mipp::Reg<int16_t> arrayV;				// Matrice V de l'image
	mipp::Reg<int16_t> arrayE;				// Matrice E 
	mipp::Reg<int16_t> n_sd 	= N_SD;		// Registre rempli remplie de N pour les calculs
	mipp::Reg<int16_t> MAX 		= V_MAX;	// Registre rempli remplie de V_MAX pour les calculs
	mipp::Reg<int16_t> MIN 		= V_MIN;	// Registre rempli remplie de V_MIN pour les calculs
	mipp::Reg<int16_t> val 		= 255.0;	// Registre rempli de 255 pour les calculs
	mipp::Reg<int16_t> one 		= 1.0;		// Registre rempli de 1 pour les calculs
	mipp::Reg<int16_t> zero 	= 0.0;		// Registre rempli de 0 pour les calculs

	start = clock();
	for(short m = 0; m < 199; m++){ // 200 images
		for (short i = 0; i<*nrh; i++){
			for (short j = 0; j<*(nch); j=j+sizeof(int16_t)){

				// Matrice Moyenne
				array.load((int16_t*)&matrice[i][j]);
				arraymoy.load((int16_t*)&M_Array[i][j]);
				arraymoy = mipp::blend(mipp::add(arraymoy,one), mipp::sub(arraymoy,one), array > arraymoy);
				arraymoy.store((int16_t*)&M_Array[i][j]);
				
				// Matrice de sortie
				array.load((int16_t*)&matrice[i][j]);
				arraymoy.load((int16_t*)&M_Array[i][j]);
				sortie =(mipp::sub(array, arraymoy));
				sortie = mipp::blend(sortie, mipp::sub(zero, sortie), sortie > zero);	
				sortie.store((int16_t*) &Output_Array[i][j]);

				// Matrice V
				arrayV.load((int16_t*)&V_Array[i][j]);
				sortie.load((int16_t*)&Output_Array[i][j]);
				arrayV = mipp::blend(mipp::add(arrayV,one), mipp::sub(arrayV,one), n_sd*sortie > arrayV); // mul not defind for int8_t so using uint16_t
				mipp::max(mipp::min(arrayV, MAX),MIN).store((int16_t*)&V_Array[i][j]);

				// Matrice E
				arrayV.load((int16_t*)&V_Array[i][j]);
				arrayE.load((int16_t*)&E_Array[i][j]);
				sortie.load((int16_t*)&Output_Array[i][j]);
				arrayE = mipp::blend(mipp::add(val, val), mipp::add(zero, zero), sortie > arrayV);
				arrayE.store((int16_t*)&E_Array[i][j]);
			}
		}

		// Morphologie mathématique
		for (short i = 1; i<*nrh-1; i++){
			for (short j = 1; j<*nch-1; j++){

				// Erosion 1
				if (
				(E_Array[i][j] && E_Array[i-1][j-1]) &&
				(E_Array[i][j] && E_Array[i-1][j])	 &&
				(E_Array[i][j] && E_Array[i-1][j+1]) &&
				(E_Array[i][j] && E_Array[i][j+1])   &&
				(E_Array[i][j] && E_Array[i][j-1])   &&
				(E_Array[i][j] && E_Array[i+1][j-1]) &&
				(E_Array[i][j] && E_Array[i+1][j+1]) &&
				(E_Array[i][j] && E_Array[i+1][j])){
					Morphed_Array[i][j] = 255;
				}
				else{
					Morphed_Array[i][j] = 0;
				}
				// Dilatation 1
				if (
				(Morphed_Array[i][j] || E_Array[i-1][j-1]) ||
				(Morphed_Array[i][j] || E_Array[i-1][j])   ||
				(Morphed_Array[i][j] || E_Array[i-1][j+1]) ||
				(Morphed_Array[i][j] || E_Array[i][j+1])   ||
				(Morphed_Array[i][j] || E_Array[i][j-1])   ||
				(Morphed_Array[i][j] || E_Array[i+1][j-1]) ||
				(Morphed_Array[i][j] || E_Array[i+1][j+1]) ||
				(Morphed_Array[i][j] || E_Array[i+1][j])){
					Morphed_Array[i][j] = 255;
				}
				else{
					Morphed_Array[i][j] = 0;
				}
				// Dilatation 2
				if (
				(Morphed_Array[i][j] || E_Array[i-1][j-1]) ||
				(Morphed_Array[i][j] || E_Array[i-1][j])   ||
				(Morphed_Array[i][j] || E_Array[i-1][j+1]) ||
				(Morphed_Array[i][j] || E_Array[i][j+1])   ||
				(Morphed_Array[i][j] || E_Array[i][j-1])   ||
				(Morphed_Array[i][j] || E_Array[i+1][j-1]) ||
				(Morphed_Array[i][j] || E_Array[i+1][j+1]) ||
				(Morphed_Array[i][j] || E_Array[i+1][j])){
					Morphed_Array[i][j] = 255;
				}
				else{
					Morphed_Array[i][j] = 0;
				}
				// Erosion 2
				if (
				(Morphed_Array[i][j] && E_Array[i-1][j-1]) 	&&
				(Morphed_Array[i][j] && E_Array[i-1][j])	&&
				(Morphed_Array[i][j] && E_Array[i-1][j+1]) 	&&
				(Morphed_Array[i][j] && E_Array[i][j+1])   	&&
				(Morphed_Array[i][j] && E_Array[i][j-1])   	&&
				(Morphed_Array[i][j] && E_Array[i+1][j-1]) 	&&
				(Morphed_Array[i][j] && E_Array[i+1][j+1]) 	&&
				(Morphed_Array[i][j] && E_Array[i+1][j])){
					Morphed_Array[i][j] = 255;
				}
				else{
					Morphed_Array[i][j] = 0;
				}
			}
		}

		

		// Saving the image
		sprintf(string, "car3/car_%d.pgm", image_nb);
		matrice = LoadPGM_ui8matrix(string, nrl, nrh, ncl, nch);
		sprintf(stringE, "sortie/SortieE_%d.pgm", image_nb);
		image_nb += 1;
		SavePGM_ui8matrix(Morphed_Array, *nrl, *nrh, *ncl, *nch, stringE);


	}
	end = clock();
	// Time calculation
	double time = (double)(end - start) / (CLOCKS_PER_SEC);
	printf("%f s\n", time);
	return 0;
}