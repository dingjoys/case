#include "Cell.h"
#include "Game.h"
#include <iostream>
#include <string>
#include "util.h"
using namespace std;

bool Box::isFilled()
{
    bool filled = true;
    for (int i = 0; i < dimension; i++)
    {
        for (int j = 0; j < dimension; j++)
        {
            filled = filled &&cells[i][j].isFilled();
        }
    }
    return filled;
}

Cell *Box::get(int row, int column)
{
    return &(cells[row][column]);
}

Cell *Box::getRow(int row)
{
    return this->cells[row];
}

int *Box::getRowValue(int row)
{
    static int value[20];
    for (int i = 0; i < dimension; i++)
    {
        value[i] = cells[row][i].getValue();
    }
    return value;
}

void Box::print()
{
    string buff = "";
    for (int j = 0; j < dimension; j++)
    {
        int *tmpArray = getRowValue(j);
        buff += printIntArray(tmpArray, dimension);
        buff += "\r\n";
    }
    cout << buff << endl;
}

// void *Cell::getEmptyCells()
// {
// }

void Box::init(int row, int column, int dimension, Game *game)
{
    this->dimension = dimension;
    this->row = row;
    this->column = column;
    this->game = game;
    cells = new Cell *[dimension];
    for (int i = 0; i < dimension; i++)
    {
        cells[i] = new Cell[dimension];
        for (int j = 0; j < dimension; j++)
        {
            cells[i][j].init(i, j, dimension, this, game);
            // this->data[i][j] = i * dimension + j;
        }
    }
}

int Box::process()
{
    if (isFilled())
    {
        return false;
    }
    int stat;
    for (int i = 0; i < dimension; i++)
    {
        for (int j = 0; j < dimension; j++)
        {
            stat = get(i, j)->process();

            if (stat == 1 || stat == -1)
                return stat;
        }
    }

    return 0;
}