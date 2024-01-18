/*  ==================================  */
/*    * Developer: Zayd Abu Sneineh     */
/*    * Date     : Jan 16 2024          */
/* ===================================  */

#include <ncurses.h> /* printf, stdscr, initscr, raw, nodelay, cbreak, nonl, intrflush, keypad, curs_set */
#include <stdlib.h>  /* EXIT_FAILURE, exit, srand, malloc, free ,system, rand  */
#include <ctype.h>   /* isdigit */
#include <unistd.h>  /* sleep */
#include <time.h>    /* time */

#include "colors_definitions.h"
#include "sudoku.h"

#define SUDOKU_DIMENSION 9
/*  ==================================  */
/*  ENUMERATIONS & TYPES                */
/*  ==================================  */
typedef enum
{
    INITIATE_FROM_MAIN,
    INITIATE_FROM_INITIALIZATION
} print_location_t;

struct Sudoku_Grid
{
    unsigned int board[SUDOKU_DIMENSION][SUDOKU_DIMENSION];
    size_t board_size;
    unsigned int populated_cells_count;
    unsigned int current_row;
    unsigned int current_col;
};

/*  ==================================  */
/*	Global Variables                    */
/*  ==================================  */
unsigned int solved_board[SUDOKU_DIMENSION][SUDOKU_DIMENSION];
unsigned int mask[SUDOKU_DIMENSION][SUDOKU_DIMENSION];
print_location_t print_location = INITIATE_FROM_MAIN;

/*  ==================================  */
/*  Daclaration Static Functions        */
/*  ==================================  */
static sudoku_grid_t *CreateSudokuGrid();
static void DestroySudokuGrid(sudoku_grid_t *sudoku_grid);
static void InitializeSudokuGrid(sudoku_grid_t *sudoku_grid, int difficulty_level);
static int InitializeSudokuGridByUser(sudoku_grid_t *sudoku_grid);
static void PrintSudokuGrid(sudoku_grid_t *sudoku_grid);
static void GetCoordinates(sudoku_grid_t *sudoku_grid, size_t *square_y, size_t *square_x);
static void GetBoxCenter(size_t y, size_t x, size_t *y_center, size_t *x_center);
static int IsNumberInBox(sudoku_grid_t *sudoku_grid, unsigned int number, size_t y, size_t x);
static int IsNumberInRowLine(sudoku_grid_t *sudoku_grid, unsigned int number, size_t x);
static int IsNumberInColLine(sudoku_grid_t *sudoku_grid, unsigned int number, size_t y);
static int IsLegalValue(sudoku_grid_t *sudoku_grid, unsigned int number, size_t y, size_t x);
static void MoveCursor(sudoku_grid_t *sudoku_grid, int direction);
static void RemoveNumber(sudoku_grid_t *sudoku_grid);
static void AddNumber(sudoku_grid_t *sudoku_grid, unsigned int number);
static int SolveSudoku(sudoku_grid_t *sudoku_grid, size_t row, size_t col);
static int HasPossibleValue(sudoku_grid_t *sudoku_grid, size_t row, size_t col);
static int AllCellsHavePossibleValues(sudoku_grid_t *sudoku_grid);
static unsigned int GetPopulatedCellsCount(int difficulty_level);

/*  =================================   */
/*  API Functions Implementation        */
/*  =================================   */
void InitiateSudokuGame()
{
    sudoku_grid_t *sudoku = CreateSudokuGrid();

    size_t row = 0;
    size_t col = 0;

    int input = 0;
    int flage = 0;
    int digit = 0;

    int difficulty_level;

    printf("Choose difficulty level then press Enter:\r\n");
    printf("1. Easy\r\n");
    printf("2. Medium\r\n");
    printf("3. Hard\r\n");
    printf("4. Expert\r\n");
    printf("5. Extreme\r\n");

    scanf("%d", &difficulty_level);

    initscr(); /* Initialize ncurses */
    raw();
    nodelay(stdscr, TRUE);
    cbreak();
    noecho(); /* Don't echo input */
    nonl();
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE); /* Enable special keys, like arrows */
    curs_set(0);          /* Hide the cursor */

    srand((unsigned int)time(NULL));

    InitializeSudokuGrid(sudoku, difficulty_level);

    while ('q' != (input = getch()))
    {
        if (!flage)
        {
            PrintSudokuGrid(sudoku);
            flage = 1;
        }

        if (-1 == input)
        {
            continue;
        }

        switch (input)
        {
        case KEY_UP:
            MoveCursor(sudoku, -1); /* Move up */
            break;
        case KEY_DOWN:
            MoveCursor(sudoku, 1); /* Move down */
            break;
        case KEY_LEFT:
            MoveCursor(sudoku, -2); /* Move left */
            break;
        case KEY_RIGHT:
            MoveCursor(sudoku, 2); /* Move right */
            break;
        case 's':
            for (row = 0; row < sudoku->board_size; ++row)
            {
                for (col = 0; col < sudoku->board_size; ++col)
                {
                    sudoku->board[row][col] = solved_board[row][col];
                }
            }
            break;
        case '0': /* Allow the user to input '0' to clear a cell */
            RemoveNumber(sudoku);
            break;
        default:
            /* Check if the input is a digit (1-9) and add it to the board */
            if (isdigit(input))
            {
                digit = input - '0';
                AddNumber(sudoku, digit);
            }
            break;
        }

        /* Update the display or perform other tasks as needed */
        PrintSudokuGrid(sudoku);
    }

    endwin(); /* End ncurses mode */

    DestroySudokuGrid(sudoku);
}

int SolveSudokuGrid()
{
    sudoku_grid_t *sudoku = CreateSudokuGrid();

    size_t row = 0;
    size_t col = 0;

    int input = 0;
    int flage = 0;
    int digit = 0;

    initscr(); /* Initialize ncurses */
    raw();
    nodelay(stdscr, TRUE);
    cbreak();
    noecho(); /* Don't echo input */
    nonl();
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE); /* Enable special keys, like arrows */
    curs_set(0);          /* Hide the cursor */

    srand((unsigned int)time(NULL));

    if (InitializeSudokuGridByUser(sudoku))
    {
        system("clear");
        printf("The initialized board by the user has no solution.\r\n");
        return 1;
    }

    fflush(stdin); /* Clear input buffer */

    while ('q' != (input = getch()))
    {
        if (!flage)
        {
            PrintSudokuGrid(sudoku);
            flage = 1;
        }

        if (-1 == input)
        {
            continue;
        }

        switch (input)
        {
        case KEY_UP:
            MoveCursor(sudoku, -1); /* Move up */
            break;
        case KEY_DOWN:
            MoveCursor(sudoku, 1); /* Move down */
            break;
        case KEY_LEFT:
            MoveCursor(sudoku, -2); /* Move left */
            break;
        case KEY_RIGHT:
            MoveCursor(sudoku, 2); /* Move right */
            break;
        case 's':
            for (row = 0; row < sudoku->board_size; ++row)
            {
                for (col = 0; col < sudoku->board_size; ++col)
                {
                    sudoku->board[row][col] = solved_board[row][col];
                }
            }
            break;
        case '0': /* Allow the user to input '0' to clear a cell */
            RemoveNumber(sudoku);
            break;
        default:
            /* Check if the input is a digit (1-9) and add it to the board */
            if (isdigit(input))
            {
                digit = input - '0';
                AddNumber(sudoku, digit);
            }
            break;
        }

        /* Update the display or perform other tasks as needed */
        PrintSudokuGrid(sudoku);
    }

    endwin(); /* End ncurses mode */

    DestroySudokuGrid(sudoku);

    return 0;
}

/*  =================================   */
/*  Static Functions Implementation     */
/*  =================================   */
static sudoku_grid_t *CreateSudokuGrid()
{
    size_t col = 0;
    size_t row = 0;

    sudoku_grid_t *sudoku_grid = (sudoku_grid_t *)malloc(sizeof(sudoku_grid_t));
    if (NULL == sudoku_grid)
    {
        fprintf(stderr, "Memory allocation failed.\r\n");
        exit(EXIT_FAILURE);
    }

    sudoku_grid->board_size = SUDOKU_DIMENSION;
    sudoku_grid->populated_cells_count = 0;
    sudoku_grid->current_col = 0;
    sudoku_grid->current_row = 0;

    for (row = 0; row < sudoku_grid->board_size; ++row)
    {
        for (col = 0; col < sudoku_grid->board_size; ++col)
        {
            sudoku_grid->board[row][col] = 0;
        }
    }

    return sudoku_grid;
}

static void GetCoordinates(sudoku_grid_t *sudoku_grid, size_t *row, size_t *col)
{
    *row = sudoku_grid->current_row;
    *col = sudoku_grid->current_col;
}

static void MoveCursor(sudoku_grid_t *sudoku_grid, int direction)
{
    switch (direction)
    {
    case -1: /* Move up */
        if (sudoku_grid->current_row > 0)
        {
            --sudoku_grid->current_row;
        }
        break;
    case 1: /* Move down */
        if (sudoku_grid->current_row < sudoku_grid->board_size - 1)
        {
            ++sudoku_grid->current_row;
        }
        break;
    case -2: /* Move left */
        if (sudoku_grid->current_col > 0)
        {
            --sudoku_grid->current_col;
        }
        break;
    case 2: /* Move right */
        if (sudoku_grid->current_col < sudoku_grid->board_size - 1)
        {
            ++sudoku_grid->current_col;
        }
        break;
    }
}

static void PrintSudokuGrid(sudoku_grid_t *sudoku_grid)
{
    size_t row = 0;
    size_t col = 0;

    size_t current_row = 0;
    size_t current_col = 0;

    unsigned int num = 0;

    system("clear");

    GetCoordinates(sudoku_grid, &current_row, &current_col);

    if (INITIATE_FROM_INITIALIZATION == print_location)
    {
        printf("You are initializing the board now...\r\n\n\n");
    }
    printf("Enter the Sudoku grid numbers 1-9 (0 for empty cells):\r\n\n");

    printf("Possible values: ");

    for (num = 1; num <= sudoku_grid->board_size; ++num)
    {
        if (0 == sudoku_grid->board[current_row][current_col])
        {
            if (IsLegalValue(sudoku_grid, num, current_row, current_col))
            {
                printf("%d ", num);
            }
        }
    }
    printf("\r\n\r\n");

    for (row = 0; row < sudoku_grid->board_size; ++row)
    {
        if (0 == row % 3)
        {
            printf("-------------------------------\r\n");
        }
        for (col = 0; col < sudoku_grid->board_size; ++col)
        {
            if (0 == col % 3)
            {
                printf(" ");
            }

            if (row == sudoku_grid->current_row && col == sudoku_grid->current_col && 0 == sudoku_grid->board[row][col])
            {
                printf(ANSI_COLOR_BG_GREEN "| |" ANSI_COLOR_RESET); /* Highlight the selected cell */
            }
            else if (row == sudoku_grid->current_row && col == sudoku_grid->current_col && 0 != sudoku_grid->board[row][col])
            {
                printf(ANSI_COLOR_BG_RED "|%d|" ANSI_COLOR_RESET, sudoku_grid->board[row][col]);
            }

            else
            {
                if (0 == sudoku_grid->board[row][col])
                {
                    printf("| |");
                }
                else
                {
                    printf("|%d|", sudoku_grid->board[row][col]);
                }
            }
        }

        printf("\r\n");
    }

    printf("-------------------------------\r\n");

    if (INITIATE_FROM_INITIALIZATION == print_location)
    {
        printf("\n\n\nPress \"r\" to move to the solving the sudoku grid.\r\n");
        printf("If you pressed \"r\" and you still in initializing the board so the provided board has no solution.\r\n");
    }
    else
    {
        printf("\n\n\nPress \"s\" to to get the solved sudoku grid.\r\n");
        printf("Press \"q\" to quit the game.\r\n");
    }
}

static void GetBoxCenter(size_t row, size_t col, size_t *row_center, size_t *col_center)
{
    *row_center = ((row / 3) * 3) + 1;
    *col_center = ((col / 3) * 3) + 1;
}

static int IsNumberInBox(sudoku_grid_t *sudoku_grid, unsigned int number, size_t row_center, size_t col_center)
{
    if (number == sudoku_grid->board[row_center - 1][col_center - 1])
    {
        return 1;
    }
    if (number == sudoku_grid->board[row_center - 1][col_center])
    {
        return 1;
    }
    if (number == sudoku_grid->board[row_center - 1][col_center + 1])
    {
        return 1;
    }
    if (number == sudoku_grid->board[row_center][col_center - 1])
    {
        return 1;
    }
    if (number == sudoku_grid->board[row_center][col_center])
    {
        return 1;
    }
    if (number == sudoku_grid->board[row_center][col_center + 1])
    {
        return 1;
    }
    if (number == sudoku_grid->board[row_center + 1][col_center - 1])
    {
        return 1;
    }
    if (number == sudoku_grid->board[row_center + 1][col_center])
    {
        return 1;
    }
    if (number == sudoku_grid->board[row_center + 1][col_center + 1])
    {
        return 1;
    }

    return 0;
}

static int IsNumberInRowLine(sudoku_grid_t *sudoku_grid, unsigned int number, size_t row)
{
    size_t col = 0;

    for (col = 0; col < sudoku_grid->board_size; ++col)
    {
        if (number == sudoku_grid->board[col][row])
        {
            return 1;
        }
    }

    return 0;
}

static int IsNumberInColLine(sudoku_grid_t *sudoku_grid, unsigned int number, size_t col)
{
    size_t row = 0;

    for (row = 0; row < sudoku_grid->board_size; ++row)
    {
        if (number == sudoku_grid->board[col][row])
        {
            return 1;
        }
    }

    return 0;
}

static int IsLegalValue(sudoku_grid_t *sudoku_grid, unsigned int number, size_t row, size_t col)
{
    size_t row_center = 0;
    size_t col_center = 0;

    GetBoxCenter(row, col, &row_center, &col_center);

    if ((!IsNumberInBox(sudoku_grid, number, row_center, col_center)) && (!IsNumberInRowLine(sudoku_grid, number, col)) && (!IsNumberInColLine(sudoku_grid, number, row)))
    {
        return 1;
    }

    return 0;
}

static void RemoveNumber(sudoku_grid_t *sudoku_grid)
{
    size_t row = 0, col = 0;

    GetCoordinates(sudoku_grid, &row, &col);

    if ((0 != sudoku_grid->board[row][col]) && (0 == mask[row][col]))
    {
        sudoku_grid->board[row][col] = 0;
        --sudoku_grid->populated_cells_count;
    }
}

static void AddNumber(sudoku_grid_t *sudoku_grid, unsigned int number)
{
    size_t row = 0, col = 0;

    GetCoordinates(sudoku_grid, &row, &col);

    if ((0 == sudoku_grid->board[row][col]) && (IsLegalValue(sudoku_grid, number, row, col)))
    {
        sudoku_grid->board[row][col] = number;
        ++sudoku_grid->populated_cells_count;
    }
}

static void InitializeSudokuGrid(sudoku_grid_t *sudoku_grid, int difficulty_level)
{
    size_t row = 0;
    size_t col = 0;

    unsigned int number = 0;

    while (1)
    {
        row = 0;
        col = 0;
        number = 0;

        /* Reset the board and counters */
        sudoku_grid->populated_cells_count = 0;
        sudoku_grid->current_row = 0;
        sudoku_grid->current_col = 0;

        for (row = 0; row < sudoku_grid->board_size; ++row)
        {
            for (col = 0; col < sudoku_grid->board_size; ++col)
            {
                mask[row][col] = 0;
                solved_board[row][col] = 0;
                sudoku_grid->board[row][col] = 0;
            }
        }

        /* Populate the board based on difficulty level */
        while (sudoku_grid->populated_cells_count < GetPopulatedCellsCount(difficulty_level))
        {
            col = (rand() % SUDOKU_DIMENSION);
            row = (rand() % SUDOKU_DIMENSION);

            if (0 == sudoku_grid->board[row][col])
            {
                number = (rand() % SUDOKU_DIMENSION) + 1;

                if (IsLegalValue(sudoku_grid, number, row, col))
                {
                    mask[row][col] = 1;
                    solved_board[row][col] = number;
                    sudoku_grid->board[row][col] = number;

                    ++sudoku_grid->populated_cells_count;
                }
            }
        }

        /* Check if the generated board has a solution */
        if ((AllCellsHavePossibleValues(sudoku_grid)) && (SolveSudoku(sudoku_grid, 0, 0)))
        {
            break;
        }
    }
}

static int InitializeSudokuGridByUser(sudoku_grid_t *sudoku_grid)
{
    size_t row = 0;
    size_t col = 0;

    int flage = 0;
    int input = 0;
    int digit;
    int solved = 0;

    /* Reset the board and counters */
    sudoku_grid->populated_cells_count = 0;
    sudoku_grid->current_row = 0;
    sudoku_grid->current_col = 0;

    for (row = 0; row < sudoku_grid->board_size; ++row)
    {
        for (col = 0; col < sudoku_grid->board_size; ++col)
        {
            mask[row][col] = 0;
            solved_board[row][col] = 0;
            sudoku_grid->board[row][col] = 0;
        }
    }

    row = 0;
    col = 0;
    print_location = INITIATE_FROM_INITIALIZATION;

    while (1)
    {
        while ('r' != (input = getch()))
        {
            if (!flage)
            {
                PrintSudokuGrid(sudoku_grid);
                flage = 1;
            }

            if (-1 == input)
            {
                continue;
            }

            switch (input)
            {
            case KEY_UP:
                MoveCursor(sudoku_grid, -1); /* Move up */
                break;
            case KEY_DOWN:
                MoveCursor(sudoku_grid, 1); /* Move down */
                break;
            case KEY_LEFT:
                MoveCursor(sudoku_grid, -2); /* Move left */
                break;
            case KEY_RIGHT:
                MoveCursor(sudoku_grid, 2); /* Move right */
                break;
            case '0': /* Allow the user to input '0' to clear a cell */
                GetCoordinates(sudoku_grid, &row, &col);

                mask[row][col] = 0;
                solved_board[row][col] = 0;
                sudoku_grid->board[row][col] = 0;

                --sudoku_grid->populated_cells_count;

                break;
            default:
                GetCoordinates(sudoku_grid, &row, &col);

                /* Check if the input is a digit (1-9) and add it to the board */
                if (isdigit(input))
                {
                    digit = input - '0';

                    mask[row][col] = 1;
                    solved_board[row][col] = digit;
                    AddNumber(sudoku_grid, digit);

                    ++sudoku_grid->populated_cells_count;
                }
                break;
            }

            /* Update the display or perform other tasks as needed */
            PrintSudokuGrid(sudoku_grid);
        }
        /* Check if the generated board has a solution */
        solved = SolveSudoku(sudoku_grid, 0, 0);

        system("clear");
        printf("LOADING ...\n");

        sleep(1);

        if ((AllCellsHavePossibleValues(sudoku_grid)) && (1 == solved))
        {
            break;
        }
        else
        {
            return !solved;
        }
    }
    print_location = INITIATE_FROM_MAIN;

    return (!solved);
}

static void DestroySudokuGrid(sudoku_grid_t *sudoku_grid)
{
    free(sudoku_grid);
    sudoku_grid = NULL;
}

static int SolveSudoku(sudoku_grid_t *sudoku_grid, size_t row, size_t col)
{
    unsigned int num = 1;

    if (row == sudoku_grid->board_size)
    {
        row = 0;
        if (++col == sudoku_grid->board_size)
        {
            return 1; /* Entire board is filled, a solution is found */
        }
    }

    if (0 == sudoku_grid->board[row][col])
    {
        for (num = 1; num <= sudoku_grid->board_size; ++num)
        {
            if (IsLegalValue(sudoku_grid, num, row, col))
            {
                sudoku_grid->board[row][col] = num;
                solved_board[row][col] = num;

                if (SolveSudoku(sudoku_grid, row + 1, col))
                {
                    sudoku_grid->board[row][col] = 0;
                    return 1; /* If a solution is found, return 1 */
                }

                sudoku_grid->board[row][col] = 0; /* If no solution, backtrack */
                solved_board[row][col] = 0;
            }
        }

        return 0; /* If no valid number can be placed, trigger backtracking */
    }

    return SolveSudoku(sudoku_grid, row + 1, col); /* Move to the next row */
}

static int HasPossibleValue(sudoku_grid_t *sudoku_grid, size_t row, size_t col)
{
    size_t num = 0;

    for (num = 1; num <= sudoku_grid->board_size; ++num)
    {
        if (IsLegalValue(sudoku_grid, num, row, col))
        {
            return 1;
        }
    }

    return 0;
}

static int AllCellsHavePossibleValues(sudoku_grid_t *sudoku_grid)
{
    size_t row = 0;
    size_t col = 0;
    for (row = 0; row < sudoku_grid->board_size; ++row)
    {
        for (col = 0; col < sudoku_grid->board_size; ++col)
        {
            if (0 == sudoku_grid->board[row][col] && !HasPossibleValue(sudoku_grid, row, col))
            {
                return 0;
            }
        }
    }

    return 1;
}

static unsigned int GetPopulatedCellsCount(int difficulty_level)
{
    switch (difficulty_level)
    {
    case EASY:
        return 42;
    case MEDIUM:
        return 38;
    case HARD:
        return 30;
    case EXPERT:
        return 22;
    case EXTREME:
        return 17;
    default:
        fprintf(stderr, "Invalid difficulty level.\n");
        endwin(); /* End ncurses mode */
        exit(EXIT_FAILURE);
    }
}
