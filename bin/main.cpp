#include <lib/number.h>
#include <iostream>

int main() {
    int2023_t value = from_int(123);
    int2023_t value1 = from_int(123);
    std::cout << (value < value1) << std::endl;

    return 0;
}