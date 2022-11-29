// Algorithme SD - Optimisations basiques 
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
	uint8** matrice = LoadPGM_ui8matrix(string, nrl, nrh, ncl, nch);
	uint8** M_Array = matrice;
	uint8** V_Array = ui8matrix((long)*nrl, (long)*nrh, (long)*ncl, (long)*nch);
	set_ui8matrix(V_Array, *nrl, *nrh, *ncl, *nch, V_MIN);
	uint8** Output_Array = ui8matrix((long)*nrl, (long)*nrh, (long)*ncl, (long)*nch);
	uint8** E_Array = ui8matrix((long)*nrl, (long)*nrh, (long)*ncl, (long)*nch);
	uint8** Morphed_Array = ui8matrix((long)*nrl, (long)*nrh, (long)*ncl, (long)*nch);

	// time(&start);
	start = clock();
	for(short m = 0; m < 199; m++){ // 200 images
		// Meam matrix update
		for (short i = 0; i<*nrh; i++){
			for (short j = 0; j<*nch; j++){
				if (M_Array[i][j] < matrice[i][j]){
					matrice[i][j] += 1;
				}
				else if (M_Array[i][j] > matrice[i][j]){
					M_Array[i][j] -= 1;
				}
				else{
					M_Array[i][j] = M_Array[i][j];
				}
		// Difference Computation
				Output_Array[i][j] = abs(M_Array[i][j] - matrice[i][j]);

		// Update and clamping

				if (V_Array[i][j] < N*Output_Array[i][j]){
					V_Array[i][j] += 1;
				}
				else if (V_Array[i][j] > N*Output_Array[i][j]){
					V_Array[i][j] -= 1;
				}
				else{
					V_Array[i][j] = V_Array[i][j];
				}
				V_Array[i][j] = max(min(V_Array[i][j], V_MAX), V_MIN);

		// E Estimation
				if (Output_Array[i][j] < V_Array[i][j]){
					E_Array[i][j] = 0;
				}
				else{
					E_Array[i][j] = 255;
				}
			}
		}

		
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
				(Morphed_Array[i][j] && E_Array[i-1][j-1]) &&
				(Morphed_Array[i][j] && E_Array[i-1][j])	 &&
				(Morphed_Array[i][j] && E_Array[i-1][j+1]) &&
				(Morphed_Array[i][j] && E_Array[i][j+1])   &&
				(Morphed_Array[i][j] && E_Array[i][j-1])   &&
				(Morphed_Array[i][j] && E_Array[i+1][j-1]) &&
				(Morphed_Array[i][j] && E_Array[i+1][j+1]) &&
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