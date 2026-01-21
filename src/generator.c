#include <stdlib.h>

#include "../include/generator.h"
#include "../include/solver.h"

void init_sudoku(struct SudokuState* sudokuState){
	for (int i = 0; i < CELLS; i++){
		sudokuState->grid[i] = 0;
		sudokuState->domain[i] = FULL_MASK;
	}
}

void shuffle_array(int* arr, int size){
	if (size > 1){
		int j,temp;
		for (int i = size-1; i > 0; i--){
			j = (rand()) % (i+1);
			temp = arr[j];
			arr[j] = arr[i];
			arr[i] = temp;
		}
	}
}

void insert_row(struct SudokuState* sudokuState,int* arr){
	for (int i = 0; i < SIZE; i++){
		sudokuState->grid[i] = arr[i];
	}
}

void reset_domain(struct SudokuState* sudokuState){
	for (int i = 0; i < CELLS; i++){
		sudokuState->domain[i] = FULL_MASK;
	}
}

void remove_values(struct SudokuState* sudokuState,int size, int num){
	int indicies[num]; int isUnique; int randIndex;
	for (int i = 0; i < num; i++){
		isUnique = 0;
		while (!isUnique){
			randIndex = (rand()) % size;
			for (int j = 0; j < i; j++){
				if (indicies[j] == randIndex) break;
			}
			isUnique = 1;
		}
		indicies[i] = randIndex;
		sudokuState->grid[randIndex] = 0;
	}
}

void generate_sudoku(struct SudokuState* sudokuState, int nRemove){
	int row[SIZE];
	for (int i = 0; i < SIZE; i++){
		row[i] = i+1;
	}
	shuffle_array(row,SIZE);
	insert_row(sudokuState,row);
	
	solve(sudokuState);

	reset_domain(sudokuState);
	remove_values(sudokuState,CELLS,nRemove);
}
