#ifndef INCLUDE_H
#define INCLUDE_H

int randint(int low, int high, int seed)
{
    srand(seed);
    int value = rand() % (high + 1 - low) + low;
    return value;
}

#endif
