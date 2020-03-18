#include "Cell.h"
// #include "Game.h"
#ifndef sudoku_Box
#define sudoku_Box

class Game;
class Box
{
public:
    Cell **cells;
    int row;
    int column;

    Box()
    {
    }
    ~Box()
    {
    }
    void init(int row, int column, int dimension, Game *game);
    Cell *getRow(int row);
    int *getRowValue(int row);
    Cell *getColumn(int column);
    Cell *get(int row, int column);
    bool conflict(int row, int column, int num);
    void print();
    /**
     * Return 1 if altered; -1 if error; 0 if nothing changes.
     */
    int process();
    bool isFilled();

private:
    int dimension;
    Game *game;
};

#endif