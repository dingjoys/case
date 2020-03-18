#include <memory>
#include <iostream>
#include "Game.h"
#include "util.h"
#include <set>
#include <chrono>
#include <ctime>

using namespace std;

void test_rule1()
{
    unique_ptr<Game> game(new Game(2));
    game->get(0, 0)->get(0, 0)->setValue(1);
    game->get(0, 0)->get(0, 1)->setValue(2);
    game->get(0, 0)->get(1, 0)->setValue(3);
    game->print();
    int stat = game->get(0, 0)->get(1, 1)->process();

    set<int> tmpSet = game->get(0, 0)->get(1, 1)->possibilities;
    cout << "  :  " << stat << "  " << game->get(0, 0)->get(1, 1)->getValue() << endl;
    game->print();
    // for (set<int>::iterator it = tmpSet.begin(); it != tmpSet.end(); it++)
    // {
    //     cout << *it << endl;
    // }
}

void test_rule2()
{
    unique_ptr<Game> game(new Game(2));
    game->get(0, 0)->get(0, 0)->setValue(1);
    game->get(0, 1)->get(1, 0)->setValue(1);
    game->get(1, 0)->get(0, 1)->setValue(1);
    game->print();
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            for (int m = 0; m < 2; m++)
            {
                for (int n = 0; n < 2; n++)
                {
                    game->get(i, j)->get(m, n)->calculatePossibilites();
                }
            }
        }
    }
    int stat = game->get(1, 1)->get(1, 1)->process();
    // cout<<game->get(1, 1)->get(1, 1)->getValue()<<endl;
    cout << "  :  " << stat << "  " << game->get(1, 1)->get(1, 1)->getValue() << endl;
    game->print();
}

void testProcess()
{
    unique_ptr<Game> game(new Game(2));
    game->get(0, 0)->get(0, 0)->setValue(1);
    game->get(0, 0)->get(1, 0)->setValue(3);
    game->get(0, 0)->get(1, 1)->setValue(4);

    game->get(0, 1)->get(0, 0)->setValue(3);
    game->get(0, 1)->get(0, 1)->setValue(4);
    game->get(0, 1)->get(1, 1)->setValue(2);

    game->get(1, 0)->get(0, 0)->setValue(2);
    game->get(1, 0)->get(0, 1)->setValue(1);
    game->get(1, 0)->get(1, 1)->setValue(3);

    game->get(1, 1)->get(0, 0)->setValue(4);
    game->get(1, 1)->get(1, 0)->setValue(2);
    game->get(1, 1)->get(1, 1)->setValue(1);

    game->print();
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            for (int m = 0; m < 2; m++)
            {
                for (int n = 0; n < 2; n++)
                {
                    game->get(i, j)->get(m, n)->calculatePossibilites();
                }
            }
        }
    }
    game->process();
    game->print();
}

void testCopy()
{
    unique_ptr<Game> game(new Game(2));
    game->get(0, 0)->get(0, 0)->setValue(1);
    game->get(0, 1)->get(1, 0)->setValue(1);
    game->get(1, 0)->get(0, 1)->setValue(1);
    game->print();
    unique_ptr<Game> newGame(new Game(2));
    int buff[16];
    game->serialize(buff);
    cout << "--------------" << endl;
    printIntArray(buff, 16);
    cout << "--------------" << endl;
    newGame->deserialize(buff);
    newGame->print();
}

void testGenerateIndex()
{
    int randIndex[81];
    generateRandomIndex(-1, randIndex, 81);
    for (int i = 0; i < 81; i++)
    {
        cout << randIndex[i] << " ";
    }
    cout << endl;
    cout << endl;
    string test = printIntArray(randIndex, 81);
    cout << test << endl;
}

int loopProcess(shared_ptr<Game> game, int dimension)
{
    int count = 0;
    while (!game->isFilled())
    {
        cout << "process start" << endl;
        int stat = game->process();
        cout << "process end" << endl;
        if (stat == -1)
            return -1;
        if (stat == 0)
        {
            cout << "start try:" << endl;
            Cell *randomEmpty;
            game->findAnEmptyCell(randomEmpty);
            int buff[dimension * dimension * dimension * dimension];
            for (int num : randomEmpty->possibilities)
            {
                cout << "start try :" << randomEmpty->box->row << " " << randomEmpty->box->column << " " << randomEmpty->row << " " << randomEmpty->column << " " << num << endl;
                game->serialize(buff);
                shared_ptr<Game> newGame(new Game(dimension));
                // Game *newGame = new Game(dimension);
                newGame->deserialize(buff);
                newGame->get(randomEmpty->box->row, randomEmpty->box->column)->get(randomEmpty->row, randomEmpty->column)->setValue(num);
                newGame->calculatePossibilities();
                newGame->print();
                int result = loopProcess(newGame, dimension);
                cout << "try result:" << result << endl;
                if (result == 1)
                {
                    newGame->serialize(buff);
                    game->deserialize(buff);
                    return 1;
                }
            }
            return -1;
        }
        cout << "loop end" << endl;
    }
    return 1;
}

void testLoopPrcoess()
{
    int dimension = 3;
    // Game *game = new Game(dimension);
    shared_ptr<Game> game(new Game(dimension));
    game->get(0, 0)->get(0, 0)->setValue(8);
    game->get(0, 0)->get(2, 1)->setValue(1);
    game->get(0, 1)->get(0, 0)->setValue(4);
    game->get(0, 1)->get(0, 2)->setValue(6);
    game->get(0, 2)->get(0, 2)->setValue(7);
    game->get(0, 2)->get(1, 0)->setValue(4);
    game->get(0, 2)->get(2, 0)->setValue(6);
    game->get(0, 2)->get(2, 1)->setValue(5);
    game->get(1, 0)->get(0, 0)->setValue(5);
    game->get(1, 0)->get(0, 2)->setValue(9);
    game->get(1, 0)->get(2, 1)->setValue(4);
    game->get(1, 0)->get(2, 2)->setValue(8);
    game->get(1, 1)->get(0, 1)->setValue(3);
    game->get(1, 1)->get(1, 1)->setValue(7);
    game->get(1, 1)->get(2, 1)->setValue(2);
    game->get(1, 2)->get(0, 0)->setValue(7);
    game->get(1, 2)->get(0, 1)->setValue(8);
    game->get(1, 2)->get(2, 0)->setValue(1);
    game->get(1, 2)->get(2, 2)->setValue(3);

    game->get(2, 0)->get(0, 1)->setValue(5);
    game->get(2, 0)->get(0, 2)->setValue(2);
    game->get(2, 0)->get(1, 2)->setValue(1);
    game->get(2, 0)->get(2, 0)->setValue(3);

    game->get(2, 1)->get(2, 0)->setValue(9);
    game->get(2, 1)->get(2, 2)->setValue(2);

    game->get(2, 2)->get(0, 1)->setValue(9);
    game->get(2, 2)->get(2, 2)->setValue(5);
    game->calculatePossibilities();
    game->print();
    int flag = loopProcess(game, dimension);
    if (flag == -1)
    {
        cout << "No answer!" << endl;
    }
    else
    {
        game->print();
    }
}

int asyncLoopProcess(shared_ptr<Game> game, int dimension)
{
    int count = 0;
    while (!game->isFilled())
    {
        int stat = game->asyncProcess(2);
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
                newGame->deserialize(buff);
                newGame->get(randomEmpty->box->row, randomEmpty->box->column)->get(randomEmpty->row, randomEmpty->column)->setValue(num);
                newGame->calculatePossibilities();
                newGame->print();
                int result = asyncLoopProcess(newGame, dimension);
                if (result == 1)
                {
                    newGame->serialize(buff);
                    game->deserialize(buff);
                    return 1;
                }
            }
            return -1;
        }
        cout << "loop end" << endl;
    }
    return 1;
}

void testAsyncLoopProcess()
{
    int dimension = 3;
    // Game *game = new Game(dimension);
    shared_ptr<Game> game(new Game(dimension));
    game->get(0, 0)->get(0, 0)->setValue(8);
    game->get(0, 0)->get(2, 1)->setValue(1);
    game->get(0, 1)->get(0, 0)->setValue(4);
    game->get(0, 1)->get(0, 2)->setValue(6);
    game->get(0, 2)->get(0, 2)->setValue(7);
    game->get(0, 2)->get(1, 0)->setValue(4);
    game->get(0, 2)->get(2, 0)->setValue(6);
    game->get(0, 2)->get(2, 1)->setValue(5);
    game->get(1, 0)->get(0, 0)->setValue(5);
    game->get(1, 0)->get(0, 2)->setValue(9);
    game->get(1, 0)->get(2, 1)->setValue(4);
    game->get(1, 0)->get(2, 2)->setValue(8);
    game->get(1, 1)->get(0, 1)->setValue(3);
    game->get(1, 1)->get(1, 1)->setValue(7);
    game->get(1, 1)->get(2, 1)->setValue(2);
    game->get(1, 2)->get(0, 0)->setValue(7);
    game->get(1, 2)->get(0, 1)->setValue(8);
    game->get(1, 2)->get(2, 0)->setValue(1);
    game->get(1, 2)->get(2, 2)->setValue(3);

    game->get(2, 0)->get(0, 1)->setValue(5);
    game->get(2, 0)->get(0, 2)->setValue(2);
    game->get(2, 0)->get(1, 2)->setValue(1);
    game->get(2, 0)->get(2, 0)->setValue(3);

    game->get(2, 1)->get(2, 0)->setValue(9);
    game->get(2, 1)->get(2, 2)->setValue(2);

    game->get(2, 2)->get(0, 1)->setValue(9);
    game->get(2, 2)->get(2, 2)->setValue(5);
    game->calculatePossibilities();
    game->print();
    auto start = chrono::system_clock::now();
    int flag = asyncLoopProcess(game, dimension);
    auto end = chrono::system_clock::now();
    if (flag == -1)
    {
        cout << "No answer!" << endl;
    }
    else
    {
        game->print();
    }

    chrono::duration<double> elapsed_seconds = end - start;
    time_t end_time = chrono::system_clock::to_time_t(end);

    cout << "finished computation at " << ctime(&end_time)
         << "elapsed time: " << elapsed_seconds.count() << "s\n";
}

void testFromStart()
{
    int dimension = 3;
    // Game *game = new Game(dimension);
    shared_ptr<Game> game(new Game(dimension));
    asyncLoopProcess(game, dimension);
    game->print();
}

void testSolve()
{

    int dimension = 3;
    // Game *game = new Game(dimension);
    shared_ptr<Game> game(new Game(dimension));
    game->get(0, 0)->get(0, 0)->setValue(8);
    game->get(0, 0)->get(2, 1)->setValue(1);
    game->get(0, 1)->get(0, 0)->setValue(4);
    game->get(0, 1)->get(0, 2)->setValue(6);
    game->get(0, 2)->get(0, 2)->setValue(7);
    game->get(0, 2)->get(1, 0)->setValue(4);
    game->get(0, 2)->get(2, 0)->setValue(6);
    game->get(0, 2)->get(2, 1)->setValue(5);
    game->get(1, 0)->get(0, 0)->setValue(5);
    game->get(1, 0)->get(0, 2)->setValue(9);
    game->get(1, 0)->get(2, 1)->setValue(4);
    game->get(1, 0)->get(2, 2)->setValue(8);
    game->get(1, 1)->get(0, 1)->setValue(3);
    game->get(1, 1)->get(1, 1)->setValue(7);
    game->get(1, 1)->get(2, 1)->setValue(2);
    game->get(1, 2)->get(0, 0)->setValue(7);
    game->get(1, 2)->get(0, 1)->setValue(8);
    game->get(1, 2)->get(2, 0)->setValue(1);
    game->get(1, 2)->get(2, 2)->setValue(3);

    game->get(2, 0)->get(0, 1)->setValue(5);
    game->get(2, 0)->get(0, 2)->setValue(2);
    game->get(2, 0)->get(1, 2)->setValue(1);
    game->get(2, 0)->get(2, 0)->setValue(3);

    game->get(2, 1)->get(2, 0)->setValue(9);
    game->get(2, 1)->get(2, 2)->setValue(2);

    game->get(2, 2)->get(0, 1)->setValue(9);
    game->get(2, 2)->get(2, 2)->setValue(5);
    game->calculatePossibilities();
    game->print();

    auto start = chrono::system_clock::now();
    bool flag = game->solve(2);
    cout << "hh" << endl;
    auto end = chrono::system_clock::now();
    if (!flag)
    {
        cout << "No answer!" << endl;
    }
    else
    {
        game->print();
    }

    chrono::duration<double> elapsed_seconds = end - start;
    time_t end_time = chrono::system_clock::to_time_t(end);

    cout << "finished computation at " << ctime(&end_time)
         << "elapsed time: " << elapsed_seconds.count() << "s\n";
}

void testGenerate()
{
    int dimension = 3;
    // Game *game = new Game(dimension);

    shared_ptr<Game> game(new Game(dimension));
    game->generate(30);
    cout << "Question is : " << endl;
    game->print();
}
int main()
{
    // testGenerateIndex();
    testGenerate();
    // test_rule1();
    // testAsyncLoopProcess();
    // testFromStart();
}