#ifndef SOLVER_H
#define SOLVER_H

#include "./sudoku.h"

int is_valid_assigment(struct SudokuState* sudokuState, int i, int num);

int reduce_domain(struct SudokuState* sudokuState, int index, unsigned short mask);

int propagate(struct SudokuState* sudokuState, int i,int num);

int assign_digit(struct SudokuState* sudokuState, int i);

int solve(struct SudokuState* sudokuState);

#endif
