#ifndef MALULALLOC_H
#define MALULALLOC_H

#include <iostream>
#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>

#define PAGE 4096

namespace MalulAlloc{

    typedef struct Arena{               // Struct declared for an arena object.
        unsigned char* bytes;           // The total bytes allocated for the arena.
        unsigned int totalBytes;        // The total number of bytes allocated for the arena to use.
        unsigned int utilisedBytes;     // The number of bytes occupied by objects.
    }Arena;

    Arena initArena(unsigned int numberOfBytes){                    // Function which initialises an arena (with garbage values).
        Arena arena = {nullptr, numberOfBytes, 0};                  // Declares empty arena.
        if (numberOfBytes == 0){                                    // Returns an empty arena object if the number of bytes provided is equal to 0.
            return arena;
        }

        if ((numberOfBytes % 4) != 0){                              // Ensures the total allocated memory is aligned.
            numberOfBytes = numberOfBytes + (numberOfBytes % 4);
        }

        arena.bytes = (unsigned char*)mmap(NULL, numberOfBytes, PROT_READ | PROT_WRITE, 
                                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);            // Kernel allocates memory to an address (decided by the kernel), is read and write available.
        
        if (arena.bytes == MAP_FAILED){     // Exits if memory allocation fails.
            arena.bytes = NULL;
            printf("Memory allocation failed!\n");
            exit(1);
        }

        printf("Memory allocation successful! \n");
        return arena;   // Returns arena with specified allocated number of bytes.
    }

    void freeArena(Arena* arena){                               // Frees the total memory within arena.
        if (munmap(arena->bytes, arena->totalBytes) != 0){      // If munmap fails, error message printed and program exits.
            printf("Freeing memory failed!\n");
            exit(1);
        }

        arena->bytes = nullptr;                                 // Set equal to nullptr to avoid dangling pointer.
        arena->totalBytes = 0;                                  // Total bytes set equal to zero.
        arena->utilisedBytes = 0;                               // Utilised bytes also set equal to zero.
        printf("Memory freed!");
    }

    void* pushMemArena(Arena* arena, unsigned int size){                        // Pushes memory to the arena.
        if (arena->bytes == nullptr){                                           // Nullptr handling.
            printf("Unable to push memory, arena is NULL'\n");
            return nullptr;
        }
        
        if (size % 4 != 0){                                                     // Creates padding if the memory is not a multiple of 4.
            size = size + (size % 4);
        }

        if ((arena->utilisedBytes + size) > arena->totalBytes){                 // Prevents memory being added if the arena is full or will exceed the max size if the memory is pushed.
            printf("Unable to push memory, arena has ran out of memory.\n");
            return nullptr;                                                     // Returns a nullptr to indicate that memory has not been allocated.
        }

        unsigned char* pointToFreeMem = (arena->bytes + arena->utilisedBytes);  // Pointer which points to free memory.
        arena->utilisedBytes = arena->utilisedBytes + size;                     // Utilised size updated accordingly.
        return pointToFreeMem;                                                  // The pointer to the available memory is returned. 
    }

    void clearArena(Arena* arena){                          // Clears all memory to zero.
        std::memset(arena->bytes, 0, arena->totalBytes);    // Sets all memory locations equal to zero.
        arena->utilisedBytes = 0;                           // Utilised bytes accordingly set to zero.
    }

    typedef struct Malul_AllocLL{
        unsigned char* memory;
        struct Malul_AllocLL* next;
        unsigned int utilisedBytes;
    }Malul_AllocLL;

    Malul_AllocLL* globalMemory = nullptr;
        

    void* malula_alloc(unsigned int size){                  // Simulates the malloc function.
        if (size == 0){                                     // Base case where a nullptr is returned if the size is zero.
            return nullptr;
        }

        Malul_AllocLL* currentNode = globalMemory;
        while (currentNode != nullptr){
            currentNode = currentNode->next;
        }

        // Kernel allocates memory to an address (decided by the kernel), is read and write available.
        currentNode = (Malul_AllocLL*)mmap(NULL, sizeof(Malul_AllocLL), PROT_READ | PROT_WRITE, 
                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);                                        
        
        if (currentNode == MAP_FAILED){
            printf("\nNode allocation failed!\n");
            return nullptr;
            exit(1);
        }

        // Kernel allocates memory to an address (decided by the kernel), is read and write available.
        currentNode->memory = (unsigned char*)mmap(NULL, size, PROT_READ | PROT_WRITE, 
                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);   

        if (currentNode->memory == MAP_FAILED){
            printf("\nMemory allocation failed!\n");
            return nullptr;
            exit(1);
        }

        currentNode->utilisedBytes = size;
        currentNode->next = nullptr;

        return currentNode->memory;

    }

    void malula_free(void* object){
        if (object == nullptr){
            printf("\nUnable to free nullptr!\n");
            exit(1);
        }
        

        Malul_AllocLL* currentNode = globalMemory;
        Malul_AllocLL* prevNode = nullptr;
        Malul_AllocLL* nextNode = nullptr;

        

        while ((currentNode != nullptr) && (currentNode->memory != object)){
            prevNode = currentNode;
            currentNode = currentNode->next;
            nextNode = currentNode->next;
        }

        printf("\n%d\n", currentNode->utilisedBytes);

        if (prevNode != nullptr){
            prevNode->next = nextNode;
        }
        
        unsigned char* memory = currentNode->memory;

        if (munmap(memory, currentNode->utilisedBytes) != 0){      // If munmap fails, error message printed and program exits.
            printf("Freeing memory failed!\n");
            exit(1);
        }

        currentNode->memory = nullptr;
        if (munmap(currentNode, sizeof(Malul_AllocLL)) != 0){      // If munmap fails, error message printed and program exits.
            printf("Freeing node failed!\n");
            exit(1);
        }

        currentNode = nullptr;
        
    }
    
}

#endif