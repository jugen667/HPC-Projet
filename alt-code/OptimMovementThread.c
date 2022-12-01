// Algorithme SD - Optimisation Multi-Threading 
// Authors : Julien GENTY - Youssef LAKHDHAR

#include "nrutil.h"
#include "nrio2.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define NBR_THREADS 2
#define N 		2
#define V_MIN 	1
#define V_MAX 	254


// Structure pour les arguments de fonctions threadées
typedef struct argument_thread {
	uint8** matrice_image;
	uint8** matrice_moyenne;
	uint8** matrice_sortie;
	uint8** matrice_V;
	uint8** matrice_E;
	uint8** Array1;
	uint8** Array2;
	uint8** sortie;
	int nrh;
	int nch;
} thread_args;

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
void *mean_matrix_update(void* args){
	for (short i = 0; i<((thread_args*)args)->nrh; i++){
		for (short j = 0; j<((thread_args*)args)->nch; j++){
			if (((thread_args*)args)->matrice_moyenne[i][j] < ((thread_args*)args)->matrice_image[i][j]){
				((thread_args*)args)->matrice_moyenne[i][j] += 1;
			}
			else if (((thread_args*)args)->matrice_moyenne[i][j] > ((thread_args*)args)->matrice_image[i][j]){
				((thread_args*)args)->matrice_moyenne[i][j] -= 1;
			}
			else{
				((thread_args*)args)->matrice_moyenne[i][j] = ((thread_args*)args)->matrice_moyenne[i][j];
			}
		}
	}
	pthread_exit(NULL);
	return NULL;
}


// Difference Computation
void* difference_computation(void* args){
	for (short i = 0; i<((thread_args*)args)->nrh; i++){
		for (short j = 0; j<((thread_args*)args)->nch; j++){
			((thread_args*)args)->matrice_sortie[i][j] = abs(((thread_args*)args)->matrice_moyenne[i][j] - ((thread_args*)args)->matrice_image[i][j]);
			
		}
	}
	pthread_exit(NULL);
	return NULL;
}

// Update and clamping
void *clamping(void* args){
	for (short i = 0; i<((thread_args*)args)->nrh; i++){
		for (short j = 0; j<((thread_args*)args)->nch; j++){
			if (((thread_args*)args)->matrice_V[i][j] < N*((thread_args*)args)->matrice_sortie[i][j]){
				((thread_args*)args)->matrice_V[i][j] += 1;
			}
			else if (((thread_args*)args)->matrice_V[i][j] > N*((thread_args*)args)->matrice_sortie[i][j]){
				((thread_args*)args)->matrice_V[i][j] -= 1;
			}
			else{
				((thread_args*)args)->matrice_V[i][j] = ((thread_args*)args)->matrice_V[i][j];
			}
			((thread_args*)args)->matrice_V[i][j] = max(min(((thread_args*)args)->matrice_V[i][j], V_MAX), V_MIN);
		}
	}
	pthread_exit(NULL);
	return NULL;
}

void *E_estimation(void * args){
	for (short i = 0; i<((thread_args*)args)->nrh; i++){
		for (short j = 0; j<((thread_args*)args)->nch; j++){
			if (((thread_args*)args)->matrice_sortie[i][j] < ((thread_args*)args)->matrice_V[i][j]){
				((thread_args*)args)->matrice_E[i][j] = 0;
			}
			else{
				((thread_args*)args)->matrice_E[i][j] = 255;
			}
		}
	}
	pthread_exit(NULL);
	return NULL;
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

void erosion(uint8** Array1, uint8** Array2, uint8** sortie, int* nrh, int* nch){
	for (short i = 1; i<*nrh-1; i++){
		for (short j = 1; j<*nch-1; j++){
			if (
			(Array1[i][j] && Array2[i-1][j-1]) &&
			(Array1[i][j] && Array2[i-1][j])	&&
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

	// Variables des threads
	pthread_t pid[NBR_THREADS];
	// pthread_t pid2;
	// pthread_t pid3;
	// pthread_t pid4;
	thread_args thread_argument[NBR_THREADS];
	

	start = clock();
	for(short m = 1; m < 200; m++){ // 200 images
		
		
		thread_argument->matrice_E = E_Array;

		for(short x = 0; x < NBR_THREADS; x++){
			thread_argument[x].nrh = *nrh;
			thread_argument[x].nch = *nch;
			thread_argument[x].matrice_image = matrice;
			thread_argument[x].matrice_moyenne = M_Array;
			pthread_create(&pid[x], NULL, mean_matrix_update, &thread_argument[x]);
		}
		for(short x = 0; x < NBR_THREADS; x++){
			pthread_join(pid[x], NULL);
		}

		for(short x = 0; x < NBR_THREADS; x++){
			thread_argument[x].nrh = *nrh;
			thread_argument[x].nch = *nch;
			thread_argument[x].matrice_image = matrice;
			thread_argument[x].matrice_moyenne = M_Array;
			thread_argument[x].matrice_sortie = Output_Array;

			pthread_create(&pid[x], NULL, difference_computation, &thread_argument[x]);
		}
		for(short x = 0; x < NBR_THREADS; x++){
			pthread_join(pid[x], NULL);
		}

		for(short x = 0; x < NBR_THREADS; x++){
			thread_argument[x].nrh = *nrh;
			thread_argument[x].nch = *nch;
			thread_argument[x].matrice_V = V_Array;
			thread_argument[x].matrice_sortie = Output_Array;

			pthread_create(&pid[x], NULL, clamping, &thread_argument[x]);
		}
		for(short x = 0; x < NBR_THREADS; x++){
			pthread_join(pid[x], NULL);
		}

		for(short x = 0; x < NBR_THREADS; x++){
			thread_argument[x].nrh = *nrh;
			thread_argument[x].nch = *nch;
			thread_argument[x].matrice_V = V_Array;
			thread_argument[x].matrice_E = E_Array;

			pthread_create(&pid[x], NULL, E_estimation, &thread_argument[x]);
		}
		for(short x = 0; x < NBR_THREADS; x++){
			pthread_join(pid[x], NULL);
		}

		// Erosion 1
		erosion(E_Array, Morphed_Array, Morphed_Array, nrh, nch);

		// Dilatation 1
		dilatation(Morphed_Array, E_Array, Morphed_Array, nrh, nch);

		// Dilatation 2
		dilatation(Morphed_Array, E_Array, Morphed_Array, nrh, nch);

		// Erosion 2
		erosion(Morphed_Array, E_Array, Morphed_Array, nrh, nch);

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