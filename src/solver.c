#include <string.h>
#include <stdint.h>

#include "../include/sudoku.h"

int propagate(struct SudokuState* sudokuState, int index);

int assign_cell(struct SudokuState* sudokuState, int index, int num);

int bit_scan(uint32_t mask, int n){
	for (int i = 0; i < n; i++){
		if (mask & (1 << i)) return i;
	}
	return -1;
}

int reduce_domain(struct SudokuState* sudokuState, int index, int num){
	uint32_t mask = FULL_MASK & ~(1 << (num-1));	

	// update domain
	sudokuState->domain[index] &= mask;	
	uint32_t domain = sudokuState->domain[index];

	// validate
	if (domain == 0) return 0;

	// forced assignment
	if ((domain & (domain-1)) == 0){
		int forcedNum = bit_scan(domain,SIZE) + 1;

		if (!assign_cell(sudokuState,index,forcedNum)) return 0;

		if (!propagate(sudokuState,index)) return 0;
	}

	return 1;
}


int propagate(struct SudokuState* sudokuState, int index){
	int num = sudokuState->grid[index];
	if (num == 0) return 1;

	// Row propagation
	int row = index / SIZE;
	for (int x = 0; x < SIZE; x++){
		int nIndex = row*SIZE + x;

		if (index == nIndex) continue;
		if (sudokuState->grid[nIndex] != 0) continue;
		if (!reduce_domain(sudokuState,nIndex,num)) return 0;
	}

	// Col propagation
	int col = index % SIZE;
	for (int y = 0; y < SIZE; y++){
		int nIndex = y*SIZE + col;

		if (index == nIndex) continue;
		if (sudokuState->grid[nIndex] != 0) continue;
		if (!reduce_domain(sudokuState,nIndex,num)) return 0;
	}

	// Box propagation
	int box_row = (row / BOX_SIZE) * BOX_SIZE;
	int box_col = (col / BOX_SIZE) * BOX_SIZE;
	for (int y = 0; y < BOX_SIZE; y++){
		for (int x = 0; x < BOX_SIZE; x++){
			int nIndex = (y+box_row)*SIZE + (x+box_col);
		
			if (index == nIndex) continue;
			if (sudokuState->grid[nIndex] != 0) continue;
			if (!reduce_domain(sudokuState,nIndex,num)) return 0;
		}
	}
		

	return 1;
}
int most_constrained(struct SudokuState* sudokuState){
	int best_index = -1;
	int min_count = SIZE+1;

	for (int i = 0; i < CELLS; i++){
		if (sudokuState->grid[i] != 0) continue;

		int count = 0;
		uint32_t domain = sudokuState->domain[i];

		for (int j = 0; j < SIZE; j++){
			if (domain & (1 << j)) count++;
		}

		if (count < min_count){
			best_index = i;
			min_count = count;

			if (min_count == 1) break;
		}
	}

	return best_index;
}

int assign_cell(struct SudokuState* sudokuState, int index, int num){

	// Check Domain
	uint32_t mask = 1 << (num-1);
	if ((sudokuState->domain[index] & mask) == 0) return 0;

	int row = index / SIZE;
	int col = index % SIZE;
	
	// Check row
	for (int x = 0; x < SIZE; x++){
		int nIndex = row*SIZE + x;
		if (nIndex != index && sudokuState->grid[nIndex] == num) return 0;
	}

	// Check col
	for (int y = 0; y < SIZE; y++){
		int nIndex = y*SIZE + col;
		if (nIndex != index && sudokuState->grid[nIndex] == num) return 0;
	}

	int box_row = (row / BOX_SIZE) * BOX_SIZE;
	int box_col = (col / BOX_SIZE) * BOX_SIZE;
	
	// Check box
	for (int x = 0; x < BOX_SIZE; x++){
		for (int y = 0; y < BOX_SIZE; y++){
			int nIndex = (y+box_row)*SIZE + (x+box_col);
			if (nIndex != index && sudokuState->grid[nIndex] == num) return 0;
		}
	}

	sudokuState->grid[index] = num;
	sudokuState->domain[index] = mask;

	return 1;
}


int guess(struct SudokuState* sudokuState){
	int index = most_constrained(sudokuState);

	if (index == -1) return 1;
	
	uint32_t backupDomain[CELLS];
	int backupGrid[CELLS];
	for (int i = 0; i < SIZE; i++){
		if (sudokuState->domain[index] & (1<<i)){
			memcpy(backupDomain,sudokuState->domain,sizeof(backupDomain));
			memcpy(backupGrid,sudokuState->grid,sizeof(backupGrid));

			if (!assign_cell(sudokuState,index,i+1)) continue;
			
			if (!propagate(sudokuState,index) || !guess(sudokuState)){
				sudokuState->grid[index] = 0;
				memcpy(sudokuState->domain,backupDomain,sizeof(backupDomain));
				memcpy(sudokuState->grid,backupGrid,sizeof(backupGrid));
				continue;
			}

			return 1;
		}
	}

	return 0;
}

int solve(struct SudokuState* sudokuState){
	for (int i = 0; i < CELLS; i++){
		if (sudokuState->grid[i] != 0) propagate(sudokuState,i);
	}

	return guess(sudokuState);
}

