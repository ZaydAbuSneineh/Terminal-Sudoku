/**
 * @file sudoku.h
 * @brief Sudoku Game Interface
 *
 * This header file provides the interface for the Sudoku game,
 * including the enumeration for difficulty levels, the structure
 * representing the Sudoku grid, and function prototypes for the
 * main Sudoku game functions.
 *
 * @author [Zayd Abu Sneineh]
 */

#ifndef SUDOKU_H
#define SUDOKU_H

/**
 * @enum level
 * Enumeration for difficulty levels in Sudoku.
 * It includes EASY, MEDIUM, HARD, EXPERT, and EXTREME levels.
 */
enum level
{
    EASY = 1,
    MEDIUM = 2,
    HARD = 3,
    EXPERT = 4,
    EXTREME = 5
};

/**
 * @typedef sudoku_grid_t
 * Typedef for the Sudoku grid structure.
 * The structure is defined in the implementation files.
 */
typedef struct Sudoku_Grid sudoku_grid_t;

/**
 * @brief Take a Sudoku grid from the user.
 *
 * This function allows the user to input a Sudoku grid
 * based on the chosen difficulty level.
 */
void InitiateSudokuGame();

/**
 * @brief Give a Sudoku grid to the user.
 *
 * This function initializes a Sudoku grid and allows the
 * user to interactively fill in or solve the puzzle.
 *
 * @return 0 on successful completion.
 */
int SolveSudokuGrid();

#endif /* SUDOKU_H */
