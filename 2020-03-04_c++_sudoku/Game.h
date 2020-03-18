
#include "Cell.h"
#include "Box.h"
#include <iostream>
#include <map>
#include <memory>

#ifndef sudoku_board
#define sudoku_board

using namespace std;
class Game : public std::enable_shared_from_this<Game>
{
public:
    Box **boxes;

    Game(int dimension)
    {
        this->dimension = dimension;
        this->boxes = new Box *[dimension];
        for (int i = 0; i < dimension; i++)
        {
            this->boxes[i] = new Box[dimension];
        }
        init();
    }

    ~Game()
    {
    }

    void print();
    bool isFilled();
    int process();
    // n->thread count
    bool solve(int n);
    int asyncProcess(int n);
    void serialize(int *data);
    void deserialize(int *data);
    void findAnEmptyCell(Cell *&cell);
    void generate(int seed, int givenCount);

    int getDimension()
    {
        return dimension;
    }
    Box *getRow(int row);
    Box *getColumn(int column);
    Box *get(int row, int column);
    void calculatePossibilities();
    Cell *getCell(int index);

private:
    int dimension;
    void init()
    {
        for (int i = 0; i < dimension; i++)
        {
            for (int j = 0; j < dimension; j++)
            {
                boxes[i][j].init(i, j, dimension, this);
            }
        }
    }
};

#endif