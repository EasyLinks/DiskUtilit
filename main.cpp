#include <iostream>
#include <cmath>
#include <windows.h>

const unsigned int CONST_BLOCK_SIZE = 128*1024*1024;

using namespace std;

uint64_t minimumSize (uint64_t CONST_TARGET);
uint64_t getSize(unsigned int MAX_SIZE, long double PERIOD_FACTOR, uint64_t CONST_TARGET);
long double getFactor(unsigned int MAX_WRITES, uint64_t CONST_TARGET, uint64_t SELECTED_TARGET);
uint64_t getFibonacci(uint64_t currentStep);

int main()
{
    for (int i = 0; i < 100; ++i)
    {
        std::cout << i << " Fibonacci number: " << getFibonacci(i) << std::endl;
    }
    uint64_t targetSize = 128 * 1024 * 1024;
    uint64_t maxWrites = 500;
    uint64_t desiredSize;
    long double factor;
    std::cout << "Input maximum size in MiB > ";
    std::cin >> targetSize;
    targetSize *= 1024*1024;
    std::cout << "Input maximum writes number > ";
    std::cin >> maxWrites;
    std::cout << "Input desired size in MiB (minimum " << minimumSize(targetSize)/1024/1024 << " MiB) > ";
    std::cin >> desiredSize;
    desiredSize *= 1024*1024;
    std::cout << "For target size of " << desiredSize/1024/1024 << " MiB factor is " << getFactor(maxWrites, targetSize, desiredSize) << std::endl << " With " << maxWrites << " maximum writes and " << targetSize/1024/1024 << " MiB maximum block size." <<std::endl;
    std::cout << "Please, input target factor > ";
    std::cin >> factor;
    std::cout << "For target factor of " << factor << " used size is " << double(getSize(maxWrites, factor, targetSize))/1024/1024 << " MiB" << std::endl << " With " << maxWrites << " maximum writes and " << targetSize/1024/1024 << " MiB maximum block size." <<std::endl;

}

uint64_t getFibonacci(uint64_t currentStep)
{
    if (currentStep < 1)
    {
        return 0;
    }
    uint64_t fibonacci = 1, fibonacciLast = 1, fibonacciLast2 = 0;
    for (int i = 2; i < currentStep; ++i)
    {
        fibonacciLast2 = fibonacciLast;
        fibonacciLast = fibonacci;
        fibonacci += fibonacciLast2;
        if (fibonacci < fibonacciLast)
        {
            return 0;
        }
    }
    return fibonacci;
}

uint64_t getSize(unsigned int MAX_WRITES, long double PERIOD_FACTOR, uint64_t CONST_TARGET)
{
    uint64_t totalSizeUsed = 0, last = 0, current = 1;
    while (current < CONST_TARGET)
    {
        uint64_t temp = last;
        last = current;
        current += temp;
        if (current > CONST_TARGET) break;
        uint64_t currentStepResult = round(1/exp(current*PERIOD_FACTOR*exp(1)/CONST_TARGET - PERIOD_FACTOR*exp(1)/CONST_TARGET) * (MAX_WRITES - 1) + 1);
        uint64_t totalBackup = totalSizeUsed;
        totalSizeUsed += currentStepResult * current;
    }
    if (current != CONST_TARGET)
    {
        current = CONST_TARGET;
        totalSizeUsed += round(1/exp(current*PERIOD_FACTOR*exp(1)/CONST_TARGET - PERIOD_FACTOR*exp(1)/CONST_TARGET) * (MAX_WRITES - 1) + 1);
    }
    return totalSizeUsed;
}

uint64_t minimumSize (uint64_t CONST_TARGET)
{
    return getSize(1, 1, CONST_TARGET);
}

long double getFactor(unsigned int MAX_WRITES, uint64_t CONST_TARGET, uint64_t SELECTED_TARGET)
{
    if (SELECTED_TARGET < minimumSize(CONST_TARGET)) return 0;
    uint64_t usedSpace;
    long double factor, factorLast, step = 1;
    usedSpace = getSize(MAX_WRITES, factor, CONST_TARGET);
    for (factor = 0; usedSpace > SELECTED_TARGET; factor += step)
    {
        usedSpace = getSize(MAX_WRITES, factor, CONST_TARGET);
        factorLast = factor;
    }
    factorLast -= step;
    factor -= step;
    long double factorDiff = factor - factorLast, factorDiffMem = 0;
    while (factorDiff != factorDiffMem)
    {
        long double factorTemp = (factor + factorLast) / 2;
        usedSpace = getSize(MAX_WRITES, factorTemp, CONST_TARGET);
        if (factor == factorTemp) return factor;
        if (usedSpace == SELECTED_TARGET)
        {
            return factorTemp;
        }
        if (usedSpace > SELECTED_TARGET)
        {
            factorLast = factorTemp;
        }
        else
        {
            factor = factorTemp;
        }
        factorDiffMem = factorDiff;
        factorDiff = factor - factorLast;
    }
    return factor;
}
