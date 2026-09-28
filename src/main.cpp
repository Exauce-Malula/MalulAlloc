#include <iostream>
#include "../header/malulAlloc.hpp"

int main(){
    MalulAlloc::Arena arena = MalulAlloc::initArena(4096);

    int* y = (int*)MalulAlloc::pushMemArena(&arena, sizeof(int) * 5);
    

    for (int i = 0; i < 5; i++){
        y[i] = i;
    }

    for (int i = 0; i < 5; i++){
        printf("%d\n", y[i]);
    }

    MalulAlloc::clearArena(&arena); 
    MalulAlloc::freeArena(&arena);
    return 0;
}