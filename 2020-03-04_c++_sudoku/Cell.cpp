#include "Cell.h"
#include "Game.h"
#include <vector>
#include <set>

using namespace std;
vector<Cell *> Cell::getRelatedCells()
{
    vector<Cell *> result;
    for (int i = 0; i < dimension; i++)
    {
        for (int j = 0; j < dimension; j++)
        {
            if (i != row || j != column)
            {
                result.push_back(box->get(i, j));
            }
        }
    }
    for (int i = 0; i < dimension; i++)
    {
        for (int j = 0; j < dimension; j++)
        {
            if (i != box->column)
            {
                result.push_back(game->getRow(box->row)[i].get(row, j));
            }
        }
    }
    for (int i = 0; i < dimension; i++)
    {
        for (int j = 0; j < dimension; j++)
        {
            if (i != box->row)
            {
                result.push_back(game->getColumn(box->column)[i].get(j, column));
            }
        }
    }
    return result;
}

void Cell::calculatePossibilites()
{
    if (isFilled())
        return;
    set<int> filled;
    vector<Cell *> related = getRelatedCells();
    for (Cell *p : related)
    {
        if (p->getValue() != -1)
        {
            filled.insert(p->getValue());
        }
    }
    for (set<int>::iterator it = filled.begin(); it != filled.end(); it++)
    {
        for (set<int>::iterator it1 = possibilities.begin(); it1 != possibilities.end(); it1++)
        {
            if (*it1 == *it)
            {
                possibilities.erase(it1);
                break;
            }
        }
    }
}

int Cell::process()
{
    if (isFilled())
        return 0;
    set<int> filled;
    set<int> relatedPossibilities;
    bool relatedFlag = true;
    vector<Cell *> related = getRelatedCells();

    for (Cell *p : related)
    {
        if (p->getValue() != -1)
        {
            filled.insert(p->getValue());
            relatedPossibilities.insert(p->getValue());
        }
        else if (p->possibilities.size() > 0)
        {
            for (int n : p->possibilities)
            {
                relatedPossibilities.insert(n);
            }
        }
        else
        {
            relatedFlag = false;
        }
    }
    for (set<int>::iterator it = filled.begin(); it != filled.end(); it++)
    {
        for (set<int>::iterator it1 = possibilities.begin(); it1 != possibilities.end(); it1++)
        {
            if (*it1 == *it)
            {
                possibilities.erase(it1);
                break;
            }
        }
    }

    if (possibilities.size() == 0 || (relatedFlag && relatedPossibilities.size() < dimension - 1))
    {
        return -1;
    }
    if (possibilities.size() == 1)
    {
        setValue(*(possibilities.begin()));
        return 1;
    }
    if (relatedPossibilities.size() == dimension * dimension - 1)
    {
        for (int i = 1; i <= dimension; i++)
        {
            if (relatedPossibilities.find(i) == relatedPossibilities.end())
            {
                setValue(i);
                return 1;
            }
        }
    }
    return 0;
}

void Cell::init(int row, int column, int dimension, Box *box, Game *game)
{
    this->row = row;
    this->column = column;
    this->dimension = dimension;
    this->box = box;
    this->game = game;
    value = -1;
    // possibilitySize = dimension * dimension;
    for (int i = 0; i < dimension * dimension; i++)
    {
        possibilities.insert(i + 1);
    }
}

void Cell::reset(){
    value = -1;
    // possibilitySize = dimension * dimension;
    for (int i = 0; i < dimension * dimension; i++)
    {
        possibilities.insert(i + 1);
    }
}

bool Cell::isFilled()
{
    return value != -1;
}

int Cell::getValue()
{
    return value;
}

void Cell::setValue(int value)
{
    possibilities.clear();
    this->value = value;
}