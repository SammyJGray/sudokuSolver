#ifndef SOLVER_H
#define SOLVER_H

#include "./sudoku.h"

int bit_scan(uint32_t mask, int n);

int reduce_domain(struct SudokuState* sudokuState, int index, int num);

int propagate(struct SudokuState* sudokuState, int index);

int most_constrained(struct SudokuState* sudokuState);

int assign_cell(struct SudokuState* sudokuState, int index, int num);

int guess(struct SudokuState* sudokuState);

int solve(struct SudokuState* sudokuState);

#endif
