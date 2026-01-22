#include <stdio.h>
#include "../include/utils.h"
#include "../include/sudoku.h"

void print_grid(int* grid){
	for (int i = 0; i < SIZE; i++){
		for (int j = 0; j < SIZE; j++){
			printf("%2d ",grid[i*SIZE+j]);
		}
		printf("\n");
	}

	printf("\n");
}

void print_array(int* arr, int size){
	for (int i = 0; i < size; i++){
		printf("%d ",arr[i]);
	}
	printf("\n\n");
}
