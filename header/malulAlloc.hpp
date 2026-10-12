#ifndef MALULALLOC_H
#define MALULALLOC_H

#include <iostream>
#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>


namespace MalulAlloc{

    typedef struct Arena{               // Struct declared for an arena object.
        unsigned char* bytes;           // The total bytes allocated for the arena.
        unsigned int totalBytes;        // The total number of bytes allocated for the arena to use.
        unsigned int utilisedBytes;     // The number of bytes occupied by objects.
    }Arena;

    Arena initArena(unsigned int numberOfBytes){                        // Function which initialises an arena (with garbage values).
        Arena arena = {nullptr, numberOfBytes, 0};                      // Declares empty arena.
        if (numberOfBytes == 0){                                        // Returns an empty arena object if the number of bytes provided is equal to 0.
            return arena;
        }

        if ((numberOfBytes % 16) != 0){                                 // Ensures the total allocated memory is aligned.
            numberOfBytes = numberOfBytes + (16 - (numberOfBytes % 16));
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
        printf("Memory freed!\n");
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

    static Malul_AllocLL* globalMemory = nullptr;      // Global linked list, which keeps track of memory allocated via malloc.
        

    void* malula_alloc(unsigned int size){                  // Simulates the malloc function.
        if (size == 0){                                     // Base case where a nullptr is returned if the size is zero.
            return nullptr;
        }

        // Kernel allocates memory to an address (decided by the kernel), is read and write available.
        Malul_AllocLL* currentNode = (Malul_AllocLL*)mmap(NULL, sizeof(Malul_AllocLL), PROT_READ | PROT_WRITE, 
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
            munmap(currentNode, sizeof(Malul_AllocLL));
            return nullptr;
            exit(1);
        }

        currentNode->utilisedBytes = size;
        currentNode->next = globalMemory;
        globalMemory = currentNode;

        return currentNode->memory;

    }

    void malula_free(void* object){                             // Frees memory via traversing the linked list.
        if (object == nullptr){                                 // Base case, if programmer attempts to free nullptr.
            printf("\nUnable to free nullptr!\n");
            exit(1);
        }
        
        Malul_AllocLL* currentNode = globalMemory;              // Sets current pointer to the globalMemory linked list.
        unsigned char* memory = nullptr;                        // Will point to the memory intended to free within a node.

        if (currentNode->next == nullptr){                             // If there is only one node within the linked list.
            memory = currentNode->memory;                              // Sets current memory pointer to the memory within node.
            if (munmap(memory, currentNode->utilisedBytes) != 0){      // If munmap fails, error message printed and program exits.
                munmap(currentNode, sizeof(Malul_AllocLL));            // Frees node object. 
                printf("Freeing memory failed!\n");
                exit(1);
            }

            currentNode->memory = nullptr;                             // Resolves dangling pointer.
            if (munmap(currentNode, sizeof(Malul_AllocLL)) != 0){      // If munmap fails, error message printed and program exits.
                printf("Freeing node failed!\n");
                exit(1);
            }
        }
        else{                                                                           // For linked list longer than one node.
            Malul_AllocLL* prevNode = nullptr;                                          // Sets prev node to nullptr, before the first node.
            while (currentNode->memory != object){                                      // Traverses until reaches the end of the linked list and until it reaches the memory.
                prevNode = currentNode;
                currentNode = currentNode->next;
                if (currentNode->next == nullptr){
                    exit(1);
                    break;
                }
            }

            /*
            if (object != currentNode->memory){
                printf("Can't free!\n");
                exit(1);
            }
            */

            prevNode->next = currentNode->next;                        // Sets previous node's next pointer to the next node.
            memory = currentNode->memory;
            if (munmap(memory, currentNode->utilisedBytes) != 0){      // If munmap fails, error message printed and program exits.
                munmap(currentNode, sizeof(Malul_AllocLL));
                printf("Freeing memory failed!\n");
                exit(1);
            }

            currentNode->memory = nullptr;                             // Resolves dangling pointer.
            if (munmap(currentNode, sizeof(Malul_AllocLL)) != 0){      // If munmap fails, error message printed and program exits.
                printf("Freeing node failed!\n");
                exit(1);
            }
        }

        memory = nullptr;               // Resolves dangling pointer.
        currentNode = nullptr;          // Resolves dangling pointer.
        printf("Memory freed!\n");
        
    }
    
}

#endif