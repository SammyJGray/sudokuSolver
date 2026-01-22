#include <stdio.h>
#include <stdlib.h>
#include "../include/solver.h"
#include "../include/generator.h"
#include "../include/sudoku.h"
#include "../include/utils.h"

int main(void) {

	struct SudokuState sudokuState;

	unsigned int seed;
	printf("Enter Seed: ");
	scanf("%u",&seed);
	printf("\nSeed is: %u\n",seed);
	srand(seed);

	init_sudoku(&sudokuState);
	generate_sudoku(&sudokuState,50);
	print_grid(sudokuState.grid);

	solve(&sudokuState);
	print_grid(sudokuState.grid);


	return 0;
}

