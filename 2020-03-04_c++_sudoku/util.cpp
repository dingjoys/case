
#include <vector>
#include <string>
#include <cstdlib>
#include <algorithm>
#include <time.h>
#include "util.h"

using namespace std;


// template <typename T>
// string printArray(T *array, int length, T exception){
//     string buff = "";
//     for (int i = 0; i < length; i++)
//     {
//         if (array[i] == exception)
//         {
//             buff += "  ";
//         }
//         else
//             buff += to_string(array[i]) + " ";
//     }

//     return buff;
// }

string printIntArray(int *array, int length)
{
    string buff = "";
    for (int i = 0; i < length; i++)
    {
        if (array[i] == -1)
        {
            buff += "  ";
        }
        else
            buff += to_string(array[i]) + " ";
    }

    return buff;
}

void generateRandomIndex(int seed, int randIndex[], int length)
{
    if (seed == -1)
        srand((unsigned)time(NULL));
    else
        srand(seed);
    vector<int> initIndex(length);
    int rands[length - 1];
    for (int i = 0; i < length; i++)
    {
        initIndex[i] = i;
    }
    for (int i = 0; i < length - 1; i++)
    {
        rands[i] = rand() % (length - i);
    }
    for (int i = 0; i < length - 1; i++)
    {
        int delta = rands[i];
        int tmpIndex = initIndex[delta];
        randIndex[i] = tmpIndex;
        remove(begin(initIndex), end(initIndex), tmpIndex);
    }

    randIndex[length - 1] = initIndex[0];
}
