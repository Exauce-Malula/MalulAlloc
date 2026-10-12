#include <iostream>
#include <cstring>
#include "../header/malulAlloc.hpp"

/*
    TODO:
    Check for memory leaks whenever freeing memory. 
    Presumedly fixed linked list when freeing, need to add calloc and realloc.
    Add comments explaining code.
    Fix padding within pushMemArena.
*/

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



    char* string = (char*)MalulAlloc::malula_alloc((sizeof(char)) * 128);                               // allocates 128 bytes for a string. 
    strcpy(string, "\nHello, my name is Exauce! This is a test to see if my malloc function works!\n");    
    printf("%s", string);

    int* array = (int*)MalulAlloc::malula_alloc(sizeof(int) * 10);

    MalulAlloc::malula_free(array);
    MalulAlloc::malula_free(string);
    return 0;
}