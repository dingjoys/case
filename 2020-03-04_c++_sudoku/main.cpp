#include <iostream>
#include "Game.h"
#include <chrono>
#include <ctime>
#include <string>
using namespace std;

int main(int argc, char *argv[])
{
    int dimension = 3;
    //How many thread to solve the problem.
    int n = 1;
    //How many cells given.
    int given = 30;
    //Random seed
    int seed = -1;
    if (argc > 1)
        seed = stoi(string(argv[1]));
    if (argc > 2)
        given = stoi(string(argv[2]));
    if (argc > 3)
        n = stoi(string(argv[3]));
    shared_ptr<Game> game(new Game(dimension));
    game->generate(seed, given);
    cout << "Question is : " << endl
         << endl;
    game->print();

    cout << "Solving..." << endl;
    auto start = chrono::system_clock::now();
    game->solve(n);
    auto end = chrono::system_clock::now();
    cout << "Answer is : " << endl
         << endl;

    game->print();

    chrono::duration<double> elapsed_seconds = end - start;
    time_t end_time = chrono::system_clock::to_time_t(end);
    cout << endl
         << "Finished computation at " << ctime(&end_time)
         << "Elapsed time: " << elapsed_seconds.count() << "s" << endl;
}