#include <stdlib.h>
#include "../include/generator.h"
#include "../include/solver.h"

///
/// Utilities
///


static void shuffle_array(int* arr, int size){
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


///
/// Grid Inititalization
///


static void insert_first_row(struct SudokuState* sudokuState,int* arr){
	for (int i = 0; i < SIZE; i++){
		sudokuState->grid[i] = arr[i];
	}
}

static void reset_domain(struct SudokuState* sudokuState){
	for (int i = 0; i < CELLS; i++){
		sudokuState->domain[i] = FULL_MASK;
	}
}

void init_sudoku(struct SudokuState* sudokuState){
	for (int i = 0; i < CELLS; i++){
		sudokuState->grid[i] = 0;
		sudokuState->domain[i] = FULL_MASK;
	}
}


///
/// Puzzle Mutation
///


static void remove_values(struct SudokuState* sudokuState, int num){
	int indicies[num]; int isUnique; int randIndex;
	for (int i = 0; i < num; i++){
		isUnique = 0;
		while (!isUnique){
			randIndex = (rand()) % CELLS;
			isUnique = 1;
			for (int j = 0; j < i; j++){
				if (indicies[j] == randIndex){
					isUnique = 0;
					break;
				}
			}
		}
		indicies[i] = randIndex;
		sudokuState->grid[randIndex] = 0;
	}
}


///
/// Puzzle Generation
///

// Does not guarantee uniqueness or all possible puzzle states
void generate_sudoku(struct SudokuState* sudokuState, int nRemove){
	int row[SIZE];
	for (int i = 0; i < SIZE; i++){
		row[i] = i+1;
	}
	shuffle_array(row,SIZE);
	insert_first_row(sudokuState,row);
	
	solve(sudokuState);

	reset_domain(sudokuState);
	remove_values(sudokuState,nRemove);
}
