#ifndef SUDOKU_H
#define SUDOKU_H

#include <stdint.h>

#define BOX_SIZE 3
#define SIZE (BOX_SIZE*BOX_SIZE)
#define CELLS (SIZE*SIZE)
#define FULL_MASK ((1<<SIZE)-1)

struct SudokuState{
	int grid[CELLS];
	uint32_t domain[CELLS];
};

#endif
