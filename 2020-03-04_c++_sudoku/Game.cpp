#include "Game.h"
#include <iostream>
#include <string>
#include "util.h"
#include <future>
#include <unistd.h>
#include <thread>
#include "util.h"
#include <memory>
#include <vector>
#include <chrono>


using namespace std;

bool Game::isFilled()
{
    bool filled = true;
    for (int i = 0; i < dimension; i++)
    {
        for (int j = 0; j < dimension; j++)
        {
            filled = filled && boxes[i][j].isFilled();
        }
    }
    return filled;
}

Box *Game::getRow(int row)
{
    return boxes[row];
}

Box *Game::getColumn(int column)
{
    static Box tmp[20];
    for (int i = 0; i < dimension; i++)
    {
        tmp[i] = boxes[i][column];
    }
    return tmp;
}

Box *Game::get(int row, int column)
{
    return &(boxes[row][column]);
}

void Game::print()
{
    string buff = "---------------------\r\n";
    for (int i = 0; i < dimension; i++)
    {
        Box *tmpBoxes = getRow(i);

        for (int j = 0; j < dimension; j++)
        {
            for (int k = 0; k < dimension; k++)
            {
                int *tmpArray = tmpBoxes[k].getRowValue(j);
                // buff += printIntArray(tmpArray, dimension);
                buff += printArray<int>(tmpArray, dimension, -1);
                buff += "|";
            }
            buff += "\r\n";
        }
        buff += "---------------------\r\n";
    }
    cout << buff << endl;
}

int Game::process()
{
    calculatePossibilities();
    for (int i = 0; i < dimension; i++)
    {
        for (int j = 0; j < dimension; j++)
        {
            int stat = get(i, j)->process();
            if (stat == 1 || stat == -1)
                return stat;
        }
    }
    return 0;
}

int subProcess(Box *box)
{
    // cout << "subprocess start" << endl;
    int stat = box->process();
    // this_thread::sleep_for(chrono::milliseconds(30));
    // cout << "subprocess end" << endl;
    return stat;
}

int Game::asyncProcess(int n)
{
    calculatePossibilities();
    int i = 0;
    int j;
    int k = 0;
    int flag = 0;

    // future<int> ret[n];
    vector<future<int>> ret(n);

    while (i < dimension * dimension)
    {
        k = 0;
        // cout << "async iter start " << i << endl;

        for (j = 0; j < n && i < dimension * dimension; j++)
        {
            int row = i / dimension;
            int column = i % dimension;
            ret[j] = async(subProcess, get(row, column));
            ++i;
            ++k;
        }
        for (j = 0; j < n && j < k; j++)
        {
            int stat = 0;
            if(ret[j].valid())
                stat = ret[j].get();
            // cout<<"subprocess stat "<<stat<<endl;
            if (stat == -1)
            {
                // cout << "async iter interrupt " << i << endl;
                return stat;
            }
            else if (stat == 1)
            {
                flag = 1;
            }
        }
        // cout << "async iter end " << i << endl;
        if (flag == 1)
            return 1;
    }

    return 0;
}

void Game::serialize(int *data)
{
    int index;
    // int data[dimension * dimension * dimension * dimension];
    for (int i = 0; i < dimension; i++)
    {
        for (int j = 0; j < dimension; j++)
        {
            for (int m = 0; m < dimension; m++)
            {
                for (int n = 0; n < dimension; n++)
                {
                    index = (i * dimension + j) * (dimension * dimension) + m * dimension + n;
                    data[index] = get(i, j)->get(m, n)->getValue();
                }
            }
        }
    }
}

void Game::deserialize(int *data)
{
    int index;
    for (int i = 0; i < dimension; i++)
    {
        for (int j = 0; j < dimension; j++)
        {
            for (int m = 0; m < dimension; m++)
            {
                for (int n = 0; n < dimension; n++)
                {
                    index = (i * dimension + j) * (dimension * dimension) + m * dimension + n;

                    if (data[index] != -1)
                    {
                        get(i, j)->get(m, n)->setValue(data[index]);
                    }
                }
            }
        }
    }
    for (int i = 0; i < dimension; i++)
    {
        for (int j = 0; j < dimension; j++)
        {
            for (int m = 0; m < dimension; m++)
            {
                for (int n = 0; n < dimension; n++)
                {
                    get(i, j)->get(m, n)->calculatePossibilites();
                }
            }
        }
    }
}

void Game::findAnEmptyCell(Cell *&cell)
{
    for (int i = 0; i < dimension; i++)
    {
        for (int j = 0; j < dimension; j++)
        {
            for (int m = 0; m < dimension; m++)
            {
                for (int n = 0; n < dimension; n++)
                {
                    Cell *tmp = get(i, j)->get(m, n);
                    if (tmp->getValue() == -1)
                    {
                        cell = tmp;
                        return;
                    }
                }
            }
        }
    }
}

void Game::calculatePossibilities()
{
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            for (int m = 0; m < 2; m++)
            {
                for (int n = 0; n < 2; n++)
                {
                    get(i, j)->get(m, n)->calculatePossibilites();
                }
            }
        }
    }
}

Cell *Game::getCell(int index)
{
    int boxIndex = index / (dimension * dimension);
    int cellIndex = index % (dimension * dimension);
    return get(boxIndex / dimension, boxIndex % dimension)->get(cellIndex / dimension, cellIndex % dimension);
}

int loopGenerate(shared_ptr<Game> game, int dimension, int *index)
{
    int count = 0;
    while (!game->isFilled())
    {
        game->calculatePossibilities();
        int stat = 0;
        for (int i = 0; i < dimension * dimension * dimension * dimension; i++)
        {
            int tmp = game->getCell(index[i])->process();
            if (tmp == 1 || tmp == -1)
            {
                stat = tmp;
                break;
            }
        }
        if (stat == 1)
        {
            if (game->isFilled())
                return 1;
            game->calculatePossibilities();
            stat = game->process();
        }

        if (stat == -1)
            return -1;
        if (stat == 0)
        {
            Cell *emptyCell;
            for (int i = 0; i < dimension * dimension * dimension * dimension; i++)
            {
                if (!game->getCell(index[i])->isFilled())
                {
                    emptyCell = game->getCell(index[i]);
                    break;
                }
            }
            int buff[dimension * dimension * dimension * dimension];
            for (int num : emptyCell->possibilities)
            {
                game->serialize(buff);
                shared_ptr<Game> newGame(new Game(dimension));
                // Game *newGame = new Game(dimension);
                newGame->deserialize(buff);
                newGame->get(emptyCell->box->row, emptyCell->box->column)->get(emptyCell->row, emptyCell->column)->setValue(num);
                newGame->calculatePossibilities();
                int result = loopGenerate(newGame, dimension, index);
                if (result == 1)
                {
                    newGame->serialize(buff);
                    game->deserialize(buff);
                    return 1;
                }
            }
            return -1;
        }
    }
    return 1;
}

void Game::generate(int seed, int givenCount)
{
    cout << "Generating game board..." << endl;
    int randomIndex[dimension * dimension * dimension * dimension];
    generateRandomIndex(seed, randomIndex, dimension * dimension * dimension * dimension);
    int flag = loopGenerate(shared_from_this(), dimension, randomIndex);
    for (int i = dimension * dimension * dimension * dimension - 1; i > givenCount - 1; i--)
    {
        getCell(randomIndex[i])->reset();
    }
    calculatePossibilities();
}

int loopSolve(shared_ptr<Game> game, int dimension, int n)
{
    while (!game->isFilled())
    {
        int stat;
        if (n > 1)
            stat = game->asyncProcess(n);
        else
            stat = game->process();
        if (stat == -1)
            return -1;
        if (stat == 0)
        {
            Cell *randomEmpty;
            game->findAnEmptyCell(randomEmpty);
            int buff[dimension * dimension * dimension * dimension];
            for (int num : randomEmpty->possibilities)
            {
                game->serialize(buff);
                shared_ptr<Game> newGame(new Game(dimension));
                // Game *newGame = new Game(dimension);
                newGame->deserialize(buff);
                newGame->get(randomEmpty->box->row, randomEmpty->box->column)->get(randomEmpty->row, randomEmpty->column)->setValue(num);
                newGame->calculatePossibilities();
                int result = loopSolve(newGame, dimension, n);
                if (result == 1)
                {
                    newGame->serialize(buff);
                    game->deserialize(buff);
                    return 1;
                }
            }
            return -1;
        }
    }
    return 1;
}

bool Game::solve(int n)
{
    int flag = loopSolve(shared_from_this(), dimension, n);
    if (flag == -1)
    {
        return false;
    }
    else
    {
        return true;
    }
}