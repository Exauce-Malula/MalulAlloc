#include <iostream>
#include "../header/malulAlloc.hpp"

/*
    TODO:
    Resolve freeing memory once the user is done with their object. (might be to do with size checks?)
    Test realloc and calloc.
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
    string = {"\nHello, my name is Exauce! This is a test to see if my malloc function works!\n\0"};    
    printf("%s", string);
    MalulAlloc::malula_free(string);                                        
    return 0;
}