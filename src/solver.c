#include <string.h>
#include <stdint.h>

#include "../include/sudoku.h"

static int propagate(struct SudokuState* sudokuState, int index);

static int assign_cell(struct SudokuState* sudokuState, int index, int num);

static int bit_scan(uint32_t x){

	if (x == 0) return -1;

	#if defined(__GNUC__) || defined(__clang__)
		return __builtin_ctz(x);
	#else
		int index = 0;
		while ((x & 1) == 0){
			x >>= 1;
			index++;
		}
		return index;
	#endif
}

static int bit_count(uint32_t x){

	#if defined(__GNUC__) || defined(__clang__)
		return __builtin_popcount(x);

	#else
		int count = 0;
		for (int i = 0; i < SIZE; i++){
			if (x & (1 << i)) count++;
		}
		return count;
	#endif
}

// Function has the ability to reduce domain, and cause forced assignments and propagate
static int reduce_domain(struct SudokuState* sudokuState, int index, int num){
	uint32_t mask = FULL_MASK & ~(1 << (num-1));	

	// update domain
	sudokuState->domain[index] &= mask;	
	uint32_t domain = sudokuState->domain[index];

	// validate
	if (domain == 0) return 0;

	// forced assignment
	if ((domain & (domain-1)) == 0){
		int forcedNum = bit_scan(domain) + 1;

		if (!assign_cell(sudokuState,index,forcedNum)) return 0;

		if (!propagate(sudokuState,index)) return 0;
	}

	return 1;
}

typedef int (*peer_fn)(struct SudokuState* sudokuState, int peerIndex, int value);

static int for_each_peer(struct SudokuState* sudokuState, int index, int value, peer_fn fn){
	int row = index / SIZE;
	for (int x = 0; x < SIZE; x++){
		int peerIndex = row*SIZE + x;
		if (peerIndex != index && !fn(sudokuState,peerIndex,value)) return 0;
	}

	int col = index % SIZE;
	for (int y = 0; y < SIZE; y++){
		int peerIndex = y*SIZE + col;
		if (peerIndex != index && !fn(sudokuState,peerIndex,value)) return 0;
	}

	int box_row = (row / BOX_SIZE) * BOX_SIZE;
	int box_col = (col / BOX_SIZE) * BOX_SIZE;
	for (int y = 0; y < BOX_SIZE; y++){
		for (int x = 0; x < BOX_SIZE; x++){
			int peerIndex = (y+box_row)*SIZE + (x+box_col);
			if (peerIndex != index && !fn(sudokuState,peerIndex,value)) return 0;
		}
	}

	return 1;
}

static int propagate_peer(struct SudokuState* sudokuState, int peerIndex, int value){
	if (sudokuState->grid[peerIndex] != 0) return 1;
	return reduce_domain(sudokuState,peerIndex,value);
}

static int propagate(struct SudokuState* sudokuState, int index){
	int num = sudokuState->grid[index];
	if (num == 0) return 1;

	return for_each_peer(sudokuState,index,num,propagate_peer);	
}

// Returns 0 on peer conflict
static int peer_conflict(struct SudokuState* sudokuState,int peerIndex, int value){
	return (sudokuState->grid[peerIndex] != value);
}

// Function assumes constraints will be propagted later
static int assign_cell(struct SudokuState* sudokuState, int index, int num){

	// Check Domain
	uint32_t mask = 1 << (num-1);
	if ((sudokuState->domain[index] & mask) == 0) return 0;

	if (!for_each_peer(sudokuState,index,num,peer_conflict)) return 0;

	sudokuState->grid[index] = num;
	sudokuState->domain[index] = mask;

	return 1;
}


static int most_constrained(struct SudokuState* sudokuState){
	int best_index = -1;
	int min_count = SIZE+1;

	for (int i = 0; i < CELLS; i++){
		if (sudokuState->grid[i] != 0) continue;

		int count = bit_count(sudokuState->domain[i]);

		if (count < min_count){
			best_index = i;
			min_count = count;

			if (min_count == 1) break;
		}
	}

	return best_index;
}

static int guess(struct SudokuState* sudokuState){
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
		if (sudokuState->grid[i] != 0){
			if (!propagate(sudokuState,i)) return 0;
		}
	}

	return guess(sudokuState);
}

