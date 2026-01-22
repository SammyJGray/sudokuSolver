#ifndef SOLVER_H
#define SOLVER_H

#include "./sudoku.h"

int bit_scan(uint32_t x);

int bit_count(uint32_t x);

int reduce_domain(struct SudokuState* sudokuState, int index, int num);

typedef int (*peer_fn)(struct SudokuState* sudokuState, int peerIndex, int value);

int for_each_peer(struct SudokuState* sudokuState, int index, int value, peer_fn fn);

int propagate_peer(struct SudokuState* sudokuState, int peerIndex, int value);

int propagate(struct SudokuState* sudokuState, int index);

int peer_conflict(struct SudokuState* sudokuState,int peerIndex, int value);

int assign_cell(struct SudokuState* sudokuState, int index, int num);

int most_constrained(struct SudokuState* sudokuState);

int guess(struct SudokuState* sudokuState);

int solve(struct SudokuState* sudokuState);
#endif
