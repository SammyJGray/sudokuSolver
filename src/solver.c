#include <string.h>
#include <stdint.h>
#include "../include/sudoku.h"

/// ---------------------------------------------------------------
/// Type Definitions
/// ---------------------------------------------------------------


typedef int (*peer_fn)(struct SudokuState* sudokuState, int peerIndex, int value);


/// ---------------------------------------------------------------
/// Declarations
/// ---------------------------------------------------------------



static int for_each_peer(struct SudokuState*, int, int, peer_fn);
static int assign_cell(struct SudokuState*, int, int);
static int propagate(struct SudokuState*, int);
static int guess(struct SudokuState*);


/// ---------------------------------------------------------------
/// Bit Operations
/// ---------------------------------------------------------------

/**
 * bit_scan
 *
 * Returns the index of the least significant set bit
 * If x is 0, returns -1
 */
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

/**
 * bit_count
 *
 * Returns the number of set bits in x
 */
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


/// ---------------------------------------------------------------
/// Peer Iteration
/// ---------------------------------------------------------------

/**
 * for_each_peer
 *
 * Calls the given function for each for every peer of a cell
 * A peer is any cell in the same row, collumn or box
 * Returns 1 if all calls succeed, 0 if any call returns 0
 */
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


/// ---------------------------------------------------------------
/// Constraint Propagation
/// ---------------------------------------------------------------

/**
 * propagate_peer
 *
 * Reduces a peer's domain and may trigger forced assignments
 * A forced assignment can result in a call to assign_digit and propagate
 * Returns 0 if a contradiction appears, otherwise 1
 */
static int propagate_peer(struct SudokuState* sudokuState, int peerIndex, int value){
	if (sudokuState->grid[peerIndex] != 0) return 1;

	uint32_t mask = FULL_MASK & ~(1 << (value-1));

	sudokuState->domain[peerIndex] &= mask;
	uint32_t domain = sudokuState->domain[peerIndex];

	if (domain == 0) return 0;

	if ((domain & (domain-1)) == 0){
		int forcedValue = bit_scan(domain) + 1;

		if (!assign_cell(sudokuState,peerIndex,forcedValue)) return 0;

		if (!propagate(sudokuState,peerIndex)) return 0;
	}

	return 1;
}

/**
* propagate
*
* Applies constraint propagation to all of a cells peers
* Returns 0 if any contradiction appears, otherwise 1
*/
static int propagate(struct SudokuState* sudokuState, int index){
	int num = sudokuState->grid[index];
	if (num == 0) return 1;

	return for_each_peer(sudokuState,index,num,propagate_peer);	
}


/// ---------------------------------------------------------------
/// Search and Assignment
/// ---------------------------------------------------------------


/**
* peer_conflict
*
* Returns 0 if a peer contains the same value, otherwise 1
*/
static int peer_conflict(struct SudokuState* sudokuState,int peerIndex, int value){
	return (sudokuState->grid[peerIndex] != value);
}

/**
* assign_cell
*
* Assigns value to a cell after checking it's domain and constraints from peers
* Returns 1 if assignment occurs
*/
static int assign_cell(struct SudokuState* sudokuState, int index, int num){
	uint32_t mask = 1 << (num-1);
	if ((sudokuState->domain[index] & mask) == 0) return 0;

	if (!for_each_peer(sudokuState,index,num,peer_conflict)) return 0;

	sudokuState->grid[index] = num;
	sudokuState->domain[index] = mask;

	return 1;
}

/**
* most_constrained
*
* Finds an empty cell with the smallest domain size
* Returns -1 if grid is filled
*/
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

/**
* guess
*
* Recursive backtracking search
* Returns 1 on sucess, 0 on failure
*/
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


/// ---------------------------------------------------------------
/// Entry Point
/// ---------------------------------------------------------------

/**
 * solve
 *
 * Solves a sudoku using constraint propagation + backtracking
 * Returns 1 if solved, 0 if unsolvable
 */
int solve(struct SudokuState* sudokuState){
	for (int i = 0; i < CELLS; i++){
		if (sudokuState->grid[i] != 0){
			if (!propagate(sudokuState,i)) return 0;
		}
	}

	return guess(sudokuState);
}
