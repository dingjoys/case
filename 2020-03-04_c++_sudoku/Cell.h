
#include <set>
#include <vector>
// #include "Game.h"
// #include "Box.h"

#ifndef sudoku_cell
#define sudoku_cell

using namespace std;

class Box;
class Game;
class Cell
{
public:
    int row;
    int column;
    Box *box;
    Game *game;

    Cell()
    {
    }

    ~Cell(){

    }
    void init(int row, int column, int dimension, Box *box, Game *game);

    set<int> possibilities;
    // int possibilitySize;
    bool isFilled();
    /**
     * Return 1 if altered; -1 if error; 0 if nothing changes.
     */
    int process();
    void calculatePossibilites();
    void setValue(int value);
    int getValue();
    vector<Cell*> getRelatedCells();    
    void reset();
private:
    int value;
    int dimension;
};

#endif