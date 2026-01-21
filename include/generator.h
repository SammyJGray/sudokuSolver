#ifndef GENERATOR_H
#define GENERATOR_H

#include "./sudoku.h"

void init_sudoku(struct SudokuState* sudokuState);

void shuffle_array(int* arr, int size);

void insert_row(struct SudokuState* sudokuState,int* arr);

void reset_domain(struct SudokuState* sudokuState);

void remove_values(struct SudokuState* sudokuState,int size, int num);

void generate_sudoku(struct SudokuState* sudokuState, int nRemove);

#endif
