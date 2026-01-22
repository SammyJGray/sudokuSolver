#ifndef GENERATOR_H
#define GENERATOR_H

#include "./sudoku.h"

void init_sudoku(struct SudokuState* sudokuState);

int generate_sudoku(struct SudokuState* sudokuState, int nRemove);

#endif
