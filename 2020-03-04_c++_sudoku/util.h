#include <string>

#ifndef sudoku_util
#define sudoku_util
std::string printIntArray(int *array, int length);
void generateRandomIndex(int seed, int randIndex[], int length);

// template <typename T>
// std::string printArray(T *array, int length, T exception);


template <typename T>
std::string printArray(T *array, int length, T exception){
    std::string buff = "";
    for (int i = 0; i < length; i++)
    {
        if (array[i] == exception)
        {
            buff += "  ";
        }
        else
            buff += std::to_string(array[i]) + " ";
    }

    return buff;
}


#endif