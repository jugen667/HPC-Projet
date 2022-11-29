// Algorithme SD 
// Authors : Julien GENTY - Youssef LAKHDHAR

#include "nrutil.h"
#include "nrio2.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 		2
#define V_MIN 	1
#define V_MAX 	254


// Max function
int max(int a, int b){
	if (a >= b){
		return a;
	}
	else{
		return b;
	}
}

// Min function
int min(int a, int b){
	if (a <= b){
		return a;
	}
	else{
		return b;
	}
}

// Mean Matrix Update
void mean_matrix_update(uint8** matrice_image, uint8** matrice_moyenne, int* nrh, int* nch){
	for (short i = 0; i<*nrh; i++){
		for (short j = 0; j<*nch; j++){
			if (matrice_moyenne[i][j] < matrice_image[i][j]){
				matrice_moyenne[i][j] += 1;
			}
			else if (matrice_moyenne[i][j] > matrice_image[i][j]){
				matrice_moyenne[i][j] -= 1;
			}
			else{
				matrice_moyenne[i][j] = matrice_moyenne[i][j];
			}
		}
	}
}

// Difference Computation
void difference_computation(uint8** matrice_image, uint8** matrice_moyenne, uint8** matrice_sortie, int* nrh, int* nch){
	for (short i = 0; i<*nrh; i++){
		for (short j = 0; j<*nch; j++){
			matrice_sortie[i][j] = abs(matrice_moyenne[i][j] - matrice_image[i][j]);
		}
	}
}

// Update and clamping
void clamping(uint8** matrice_V, uint8** matrice_sortie, int* nrh, int* nch){
	for (short i = 0; i<*nrh; i++){
		for (short j = 0; j<*nch; j++){
			if (matrice_V[i][j] < N*matrice_sortie[i][j]){
				matrice_V[i][j] += 1;
			}
			else if (matrice_V[i][j] > N*matrice_sortie[i][j]){
				matrice_V[i][j] -= 1;
			}
			else{
				matrice_V[i][j] = matrice_V[i][j];
			}
			matrice_V[i][j] = max(min(matrice_V[i][j], V_MAX), V_MIN);
		}
	}
}

void E_estimation(uint8** matrice_V, uint8** matrice_sortie, uint8** matrice_E, int* nrh, int* nch){
	for (short i = 0; i<*nrh; i++){
		for (short j = 0; j<*nch; j++){
			if (matrice_sortie[i][j] < matrice_V[i][j]){
				matrice_E[i][j] = 0;
			}
			else{
				matrice_E[i][j] = 255;
			}
		}
	}
}

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

	start = clock();
	for(short m = 0; m < 199; m++){ // 200 images
		// Meam matrix update
		mean_matrix_update(matrice, M_Array, nrh, nch);

		// Difference Computation
		difference_computation(matrice, M_Array, Output_Array, nrh, nch);

		// Update and clamping
		clamping(V_Array, Output_Array, nrh, nch);

		// E Estimation
		E_estimation(V_Array, Output_Array, E_Array, nrh, nch);

		// Erosion 1
		erosion(E_Array, E_Array, Morphed_Array, nrh, nch);

		// // Dilatation 1
		dilatation(Morphed_Array, E_Array, Morphed_Array, nrh, nch);

		// Dilatation 2
		dilatation(Morphed_Array, E_Array, Morphed_Array, nrh, nch);

		// Erosion 2
		erosion(Morphed_Array, E_Array, Morphed_Array, nrh, nch);

		// Saving the image
		image_nb += 1;
		sprintf(string, "car3/car_%d.pgm", image_nb);
		matrice = LoadPGM_ui8matrix(string, nrl, nrh, ncl, nch);
		sprintf(stringE, "sortie/SortieE_%d.pgm", image_nb);
		SavePGM_ui8matrix(Morphed_Array, *nrl, *nrh, *ncl, *nch, stringE);


	}
	end = clock();

	// Time calculation
	double time = (double)(end - start) / (CLOCKS_PER_SEC);
	printf("%f s\n", time);

	return 0;
}