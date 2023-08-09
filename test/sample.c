// #include "structs.cpp"
#include "util.h"

int print_function(char *str) {
    if (!str)
    {
        return -1;
    } 
    else if (*str == "hello")
    {
        int a = 1, b = 2;
        int c = util_add(a, b);
        return c;
    }
    else
        return 0;
}

int main() {
    int result = 0;
    result = print_function("Hello, sample");
    return result;
}