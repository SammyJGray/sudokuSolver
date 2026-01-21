# Sudoku Solver

A backtracking Sudoku solver written in C, using constraint propagation and a bit mask based representation for domains.
## Notable Features

- Solves standard Sudoku up to `32x32`
- Uses bit masks to represent and manipulate the domain of each cell
- Uses the Most Constrained Variable heuristic when choosing what cell to guess next

## How it works
### Domain representation
Each cell has a bit mask domain:
- Bit `i` represents whether digit `i+1` is allowed.

Example for `9x9` grid:
```c
001011010 -> {2,4,5,7}
```
Using some simple bit manipulations domains can easily be iterated, reduced, compared and checked for forced assignments.
### Solving algorithm
The main entry point is in the `solve` function.
1. All given digits are iterated through and have their restrictions propagated.
2. The `guess` function is then called:
	- The most constrained unfilled cell is selected
	- If no cell is selected the puzzle is solved and returns 1
	- If not, each value in the cells domain is iterated through
	- It is attempted to assign this value to the cell and propagate
	- If this results in a contradiction, the next value is tried
	- If all values are tried, then the function returns 0
3. If an assignment is successfully made then the guess function is recursively called
4.  Any future contradictions caused by `guess` function will result in the next value being tried

If a value is forced during propagation, then digit is assigned and `propagate` is recursively called.
### Puzzle Generation
A simple Sudoku generator is included. 
- A valid row is generated and shuffled
- The row is placed in an empty grid
- The puzzle is solved
- A fixed number of digits are removed from the puzzle

During this process every domain is set to allow every value.
### Sudoku state
The Sudoku is represented by a `SudokuState` containing:
- An `int` array storing grid values
- A `unint32_t` arraying storing domain values

The grid size can be changed by setting `BOX_SIZE` in the Sudoku header file.
## Build
### Requirements
- `gcc`
- `Make`

### Build release version:
```bash
make
```

### Build debug version:
```bash
make debug
```
## Run
### Run release build:
```bash
make run
```
### Run debug build:
```bash
make run-debug
```
## Limitations
- Sudoku sizes `32x32` or above require changing the type used for representing the domain
- No advanced Sudoku solving techniques are used, like naked pairs

## Future improvements
-Adding more solving techniques
-Adding Sudoku variants
-Sudoku generator with difficulty scaling

## Final thoughts
This was written as a learning experience to explore a few different things.

I always have a desire to do more low level projects in C to give me a stronger foundation in the future and I feel happy that this project satisfied some of that desire. 
I have also been interested in constraint propagation algorithms, and have been wanting to explore this further with the wave collapse algorithm, so this felt like a good first step. 

Finally I have always struggled with the build process, as I always find myself doing things without really knowing why. So after exploring `gcc` a bit and better understanding the pipeline with source files, assembly, object files, linking and executables I started this project. 
I started with using the terminal and manually putting in the `gcc` command every time, and as the project grew I gradually improved my build process until I arrived at the make file I currently have. This was a very satisfying moment for me as I fully understood why I needed every line in my make file.
